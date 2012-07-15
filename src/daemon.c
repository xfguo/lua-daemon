#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lua-daemon.h"

#define DAEMON_LIBNAME "daemon"
/* Don't chdir("/") */
#define DAEMON_OPTION_NO_CHDIR 01
/* Don't close all open files */
#define DAEMON_OPTION_NO_CLOSE_FILES 02
/* Don't reopen stdin, stdout, and sterr to /dev/null */
#define DAEMON_OPTION_NO_REOPEN_STD_FDS 04
/* don't do a umask(0) */
#define DAEMON_OPTION_NO_UMASK0 010
#define DAEMON_MAX_FDS_TO_CLOSE 8192

typedef struct daemon_option
{
    char *name;
    int value;
} daemon_option;

static daemon_option daemon_argopts[] =
{
    {"nochdir", DAEMON_OPTION_NO_CHDIR},
    {"noclose", DAEMON_OPTION_NO_CLOSE_FILES},
    {"nostdfds", DAEMON_OPTION_NO_REOPEN_STD_FDS},
    {"noumask0", DAEMON_OPTION_NO_UMASK0},
    {NULL, 0}
};

static int
pusherror(lua_State *L, const char *info)
{
    lua_pushnil(L);
    if (info == NULL)
        lua_pushstring(L, strerror(errno));
    else
        lua_pushfstring(L, "%s: %s", info, strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
 }

static int
getoptions(lua_State *L, const char *optstr)
{
    int op = 0;
    int opsaved = 0;
    char *optstr2 = NULL;
    char *opt = NULL;
    char *saved = NULL;
    daemon_option *option = NULL;

    if (optstr != NULL)
    {
        optstr2 = strdup(optstr);
        opt = strtok_r(optstr2, ",", &saved);
        while (opt != NULL) {
            for (option = &daemon_argopts[0]; option->name != NULL; option++) {
                opsaved = op;
                if (strcmp(option->name, opt) == 0) {
                    op |= option->value;
                    break;
                }
            }
            /* user specified invalid option */
            if (opsaved == op)
                return luaL_error(L, "bad option: '%s'", opt);
            opt = strtok_r(NULL, ",", &saved);
        }
    }
    return op;
}

static int
daemon_daemonize(lua_State *L)
{
    const char *optstr = luaL_optstring(L, 1, NULL);
    int opts = getoptions(L, optstr);
    int maxfd = DAEMON_MAX_FDS_TO_CLOSE;
    int fd = 0;
    int retval = 0;

    /* become background process. child continues; parent terminates. */
    switch(fork()) {
    case -1: 
        return pusherror(L, "fork");
    case  0: 
        break;
    default: 
        _exit(-1);
    }
    
    /* become leader of new session */
    if (setsid() == -1)
        pusherror(L, "setsid");

    /* ensure we are not session leader */
    switch(fork()) {
    case -1: 
        return pusherror(L, "fork");
    case  0: 
        break;
    default: 
        _exit(-1);
    }

    /* clear file mode creation mask */
    if (!(opts & DAEMON_OPTION_NO_UMASK0))
        (void) umask(0);

    /* change to root directory */
    if (!(opts & DAEMON_OPTION_NO_CHDIR))
        if (chdir("/") != 0)
            return pusherror(L, "chdir");

    /* close all open files.  if FD limit is indeterminate, attempt
     * 'best-guess' */
    if (!(opts & DAEMON_OPTION_NO_CLOSE_FILES)) {
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)
           maxfd = DAEMON_MAX_FDS_TO_CLOSE;
        for (fd = 0; fd < maxfd; fd++)
            (void) close(fd);
    }

    if (!(opts & DAEMON_OPTION_NO_REOPEN_STD_FDS)) {
        /* reopen std fds to /dev/null */
        (void) close(STDIN_FILENO);
        fd = open("/dev/null", O_RDWR);
        /* fd should be 0 */
        if (fd != STDIN_FILENO)
            return pusherror(L, "dup2");
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return pusherror(L, "dup2");
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return pusherror(L, "dup2");
    }

    return retval;
}

static const luaL_Reg R[] =
{
    {"daemonize", daemon_daemonize},
    {NULL, NULL}
};

LUALIB_API int
luaopen_daemon(lua_State *L)
{
    luaL_register(L, DAEMON_LIBNAME, R);
    return 1;
}
