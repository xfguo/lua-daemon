#ifndef LUA_DAEMON_H
#define LUA_DAEMON_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

LUALIB_API int
luaopen_daemon(lua_State *L);

#endif /* ! LUA_DAEMON_H */
