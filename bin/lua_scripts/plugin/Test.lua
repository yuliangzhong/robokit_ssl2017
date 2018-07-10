-- 定阅某一消息
local c = 100
local json = require("json")

local odometer = Test.setSubscriber("rbk.protocol.Message_Odometer", function (laser)
	c = c -1
	print("ppp")
	local laser_value = json.decode(laser)
	-- print("aaa: ", laser_value.cycle)
end)

local laser = Test.setSubscriber("rbk.protocol.Message_Laser", function (laser)
	c = c - 1
	print("ccc")
	-- 这一步速度较慢
	-- local laser_value = json.decode(laser)
	-- print("bbb: ", laser_value.posX)
end)

local e = Test.setEvent("e")
local speed_publisher = Test.setPublisher("rbk.protocol.Message_NavSpeed")

Test.setService("add", function (a, b)
	return a-c, b+c
end)

-- Test.setService("getPos", function ()
-- 	a = a + 1
-- 	print("bbbbbb")
-- end)

-- print(Test.getPos)

-- 设置主循环函数

Test.setRunFunction(function ()
	e.fire()
	local nav_info = {}
	nav_info.x = 0.1
	nav_info.y = 0
	nav_info.rotate = 0
	speed_publisher.publish(json.encode(nav_info))
	-- speed_publisher.publish("123")
	-- print("bbbbbbb", a)
end,5)