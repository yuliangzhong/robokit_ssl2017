-- local json = require("json")
-- local cjson = require("cjson")
require(gRobotModel)

local FrontAndSideDivideAngle = 56.65
local LaserFrontBlockDist = 0.55
local LaserSideBlockDist = 0.38
local laser_blocked = false

local localization = RobotStatus.setSubscriber("rbk.protocol.Message_Localization")
local laser = RobotStatus.setSubscriber("rbk.protocol.Message_Laser")

local changeToBlocked = RobotStatus.setEvent("changeToBlocked")
local changeToNoBlocked = RobotStatus.setEvent("changeToNoBlocked")

RobotStatus.setRunFunction(function ()
	local loc_data = cjson.decode(localization.data())
	local laser_data = cjson.decode(laser.data())

	local total_scan = 0
	local front_scan = 0
	local side_scan = 0
	for i=1, #laser_data.beams do
		local angle = laser_data.beams[i].angle
		local dist = laser_data.beams[i].dist
		-- print(angle, dist)
		if laser_data.beams[i].valid then
			if math.abs(angle) < FrontAndSideDivideAngle and dist < LaserFrontBlockDist then
				front_scan = front_scan + 1
			elseif math.abs(angle) >= FrontAndSideDivideAngle and dist < LaserSideBlockDist then
				side_scan = side_scan + 1
			end
		end
	end
	total_scan = front_scan + side_scan
	if total_scan > 2 then
		if not laser_blocked then
			changeToBlocked.fire()
		end
		laser_blocked = true
	else
		if laser_blocked then
			changeToNoBlocked.fire()
		end
		laser_blocked = false
	end
end,20)