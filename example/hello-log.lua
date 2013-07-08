require 'daemon'

daemon.daemonize()
f = io.open("/tmp/hello.log", 'w')
local i = 0
while i < 10 do
    f:write(i .. ' Hello, World!\n')
    f:flush()
    i = i + 1
    os.execute('sleep 1')
end
f:close()
