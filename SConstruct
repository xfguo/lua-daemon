import os

AddOption(
    '--lua-version',
    dest='lua_ver',
    default = '5.2',
    nargs = 1,
    type = 'string',
    action = 'store',
    help = 'Lua Version 5.2 or 5.1'
)


if os.environ.has_key('CFLAGS'):
    ext_cflags = os.environ['CFLAGS']
else:
    ext_cflags = ''

if GetOption('lua_ver') == '5.2':
    cflags = "-I/usr/include/lua5.2"
else:
    cflags = "-I/usr/include/lua5.1"

src = [
    './src/daemon.c',
]

env = Environment(
    ENV = {
        'PATH' : os.environ['PATH'],
    },
    CFLAGS = ' '.join([ext_cflags, cflags, '-I./include']),
    LUA_VER = GetOption('lua_ver'),
)

shared_objs = env.SharedObject(src)
shared_lib = env.SharedLibrary('daemon.so', shared_objs, SHLIBPREFIX = '')
shared_install = env.Install('/usr/local/lib/$LUA_VER', shared_lib)

static_objs = env.StaticObject(src)
static_lib = env.StaticLibrary('daemon.a', static_objs)
static_install = env.Install('/usr/local/lib/', static_lib)

man_install = env.Install('/usr/share/man/man3', './share/lua-daemon.3')

env.Default([static_lib, shared_lib])

env.Alias('install', [static_install, shared_install, man_install])


