local skynet = require "skynet"
local otc = require "otc"



print("Name:", otc.name())

skynet.start(function() skynet.exit() end)


