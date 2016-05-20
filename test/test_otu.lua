local skynet = require "skynet"
require "skynet.manager"

skynet.start(function()
	otu = skynet.launch("otu" , skynet.address(skynet.self()), 8053, 0, 1000000)
--	gate = skynet.launch("gate" , "S" , skynet.address(skynet.self()), port, 0, max_agent, buffer)
--	skynet.send(gate,"text", "start")
	skynet.exit()
end)
