require 'posix'
require 'daemon'

daemon.daemonize()
posix.openlog('hello-log', 'p')
local i = 0
while i do
    posix.syslog(1, i .. ' Hello, World!')
    i = i + 1
end
