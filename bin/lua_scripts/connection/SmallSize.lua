gReferee = rbk.getPlugin("SSLRefereeBox")
gStrategy = rbk.getPlugin("SSLStrategy")
gGuiDebugger = rbk.getPlugin("SSLGuiDebugger")
gNetProtocol = rbk.getPlugin("NetProtocol")


if rbkConfig.simulation then
	gSimulator = rbk.getPlugin("SSLSimulator")

	gStrategy.connect(gGuiDebugger,"rbk.protocol.Debug_Msgs")
	gStrategy.connect(gSimulator,"rbk.protocol.SRC_Cmd")
	gSimulator.connect(gStrategy,"rbk.protocol.SRC_RawVision")
	gReferee.connect(gStrategy,"SSL_Referee")	
else
	gRadio = rbk.getPlugin("SSLRadio")
	gFuser = rbk.getPlugin("SSLVisionFuser")

	gStrategy.connect(gGuiDebugger,"rbk.protocol.Debug_Msgs")
	gStrategy.connect(gRadio,"rbk.protocol.SRC_Cmd")
	gFuser.connect(gStrategy,"rbk.protocol.SRC_RawVision")
	gReferee.connect(gStrategy,"SSL_Referee")
	gRadio.connect(gStrategy,"rbk.protocol.Robots_Status")
end

gNetProtocol.bindDelegate("manual", function ()
	
end)

gGuiDebugger.start()
gReferee.start()
gStrategy.start()
gNetProtocol.start()
if rbkConfig.simulation then
	gSimulator.start()
else
	gFuser.start()
	gRadio.start()	
end

-- gNetProtocol = rbk.getPlugin("NetProtocol")
-- gEKF = rbk.getPlugin("RobotPosEKF")
-- gObj = rbk.getPlugin("LaserObstacleDetection")
-- gFus =rbk.getPlugin("FusedLaser")
-- gMap = rbk.getPlugin("MapConstructor")
-- gLoc = rbk.getPlugin("MCLoc")
-- gJoystick = rbk.getPlugin("XBox360Joystick")
-- gControl = rbk.getPlugin("RobotActionHandler")
-- gDebug = rbk.getPlugin("QtDebugGui")
-- gMoveFactory = rbk.getPlugin("MoveFactory")
-- gMapLogger = rbk.getPlugin("OnlineMapLogger")
-- gABB = rbk.getPlugin("ABB")

-- if rbkConfig.simulation then
--     -- simulation
-- 	gChassis = rbk.getPlugin("PioneerSim")

-- 	gFus.connect(gMoveFactory,"rbk.protocol.Message_Laser")
-- 	gMap.connect(gMoveFactory,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gMoveFactory,"rbk.protocol.Message_Localization")
-- 	gObj.connect(gMoveFactory,"rbk.protocol.Message_Object")
-- 	gChassis.connect(gMoveFactory,"rbk.protocol.Message_NavSpeed")

-- 	gMap.connect(gFus,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gFus,"rbk.protocol.Message_Localization")

-- 	gFus.connect(gObj,"rbk.protocol.Message_Laser")
-- 	gLoc.connect(gObj,"rbk.protocol.Message_Localization")

-- 	gMap.connect(gLoc, "rbk.protocol.Message_Map")
-- 	gEKF.connect(gLoc,"rbk.protocol.Message_Odometer")
-- 	gEKF.connect(gNetProtocol, "rbk.protocol.Message_NavSpeed")

-- 	gChassis.connect(gEKF,"rbk.protocol.Message_Odometer")
-- 	gChassis.connect(gEKF,"rbk.protocol.Message_IMU")

-- 	gDebug.connect(gControl,"rbk.protocol.Message_Action")

-- 	gMap.connect(gDebug,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gObj.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gFus.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gMoveFactory.connect(gDebug,"rbk.protocol.Message_Debug")

-- 	gMoveFactory.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	gMoveFactory.connect(gNetProtocol, "rbk.protocol.Message_NavPath")
-- 	gMoveFactory.connect(gNetProtocol, "rbk.protocol.Message_MoveStatus")

-- 	gLoc.connect(gNetProtocol, "rbk.protocol.Message_Localization")
-- 	-- gChassis.connect(gNetProtocol,"rbk.protocol.Message_Battery")
-- 	gChassis.connect(gNetProtocol, "rbk.protocol.Message_Laser")

-- 	gChassis.connect(gMapLogger, "rbk.protocol.Message_Laser")
-- 	gEKF.connect(gMapLogger, "rbk.protocol.Message_Odometer")
-- 	gEKF.connect(gMoveFactory, "rbk.protocol.Message_Odometer")

-- 	gChassis.connect(gFus,"rbk.protocol.Message_Laser")
-- 	gChassis.connect(gLoc,"rbk.protocol.Message_Laser")
-- 	gChassis.connect(gMoveFactory, "rbk.protocol.Message_Ultrasonic")

-- else
-- 	-- real
-- 	gChassis = rbk.getPlugin("DSPChassis")
-- 	gLaser = rbk.getPlugin("MultiLaser")
-- 	-- gIMU = rbk.getPlugin("MiniIMU")

-- 	gFus.connect(gMoveFactory,"rbk.protocol.Message_Laser")
-- 	gMap.connect(gMoveFactory,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gMoveFactory,"rbk.protocol.Message_Localization")
-- 	gObj.connect(gMoveFactory,"rbk.protocol.Message_Object")
-- 	gChassis.connect(gMoveFactory,"rbk.protocol.Message_NavSpeed")
-- 	gChassis.connect(gMoveFactory,"rbk.protocol.Message_Battery")
-- 	gChassis.connect(gMoveFactory,"rbk.protocol.Message_DI")

-- 	gMap.connect(gFus,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gFus,"rbk.protocol.Message_Localization")

-- 	gFus.connect(gObj,"rbk.protocol.Message_Laser")
-- 	gLoc.connect(gObj,"rbk.protocol.Message_Localization")

-- 	gMap.connect(gLoc, "rbk.protocol.Message_Map")
-- 	gEKF.connect(gLoc,"rbk.protocol.Message_Odometer")
-- 	gEKF.connect(gNetProtocol, "rbk.protocol.Message_NavSpeed")

-- 	gChassis.connect(gEKF,"rbk.protocol.Message_Odometer")
-- 	gChassis.connect(gEKF,"rbk.protocol.Message_IMU")

-- 	gDebug.connect(gControl,"rbk.protocol.Message_Action")

-- 	gMap.connect(gDebug,"rbk.protocol.Message_Map")
-- 	gLoc.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gObj.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gFus.connect(gDebug,"rbk.protocol.Message_Debug")
-- 	gMoveFactory.connect(gDebug,"rbk.protocol.Message_Debug")

-- 	gMoveFactory.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	gMoveFactory.connect(gNetProtocol, "rbk.protocol.Message_NavPath")
-- 	gMoveFactory.connect(gNetProtocol, "rbk.protocol.Message_MoveStatus")

-- 	gLoc.connect(gNetProtocol, "rbk.protocol.Message_Localization")
-- 	gChassis.connect(gNetProtocol,"rbk.protocol.Message_Battery")
-- 	gChassis.connect(gNetProtocol,"rbk.protocol.Message_DI")
-- 	gChassis.connect(gNetProtocol,"rbk.protocol.Message_DO")
-- 	gChassis.connect(gNetProtocol,"rbk.protocol.Message_Controller")
-- 	gLaser.connect(gNetProtocol, "rbk.protocol.Message_Laser")

-- 	gLaser.connect(gMapLogger, "rbk.protocol.Message_Laser")
-- 	gEKF.connect(gMapLogger, "rbk.protocol.Message_Odometer")
-- 	gEKF.connect(gMoveFactory, "rbk.protocol.Message_Odometer")

-- 	gLaser.connect(gLoc,"rbk.protocol.Message_Laser")
-- 	gLaser.connect(gFus,"rbk.protocol.Message_Laser")
-- end

-- gNetProtocol.bindEvent("task_suspend", function()
-- 	gMoveFactory.suspendTask()
-- end)

-- gNetProtocol.bindEvent("task_resume", function()
-- 	gMoveFactory.resumeTask()
-- end)

-- gNetProtocol.bindEvent("task_cancel", function()
-- 	gMoveFactory.cancelTask()
-- end)

-- gNetProtocol.bindEvent("gyro_cal", function()
--     if not rbkConfig.simulation then
-- 	    gChassis.calGyro()
--     end
-- end)

-- gNetProtocol.bindEvent("reset_dsp", function()
--     if not rbkConfig.simulation then
-- 	    gChassis.resetDSP()
--     end
-- end)

-- gNetProtocol.bindDelegate("get_current_map", function()
-- 	return gMap.getCurrentMapName()
-- end)

-- gNetProtocol.bindDelegate("slam", function(flag)
-- 	if flag == "start" then
-- 		gMapLogger.startScanMap()
-- 	elseif flag == "end" then
-- 		gMapLogger.stopScanMap()
-- 	end
-- end)

-- gNetProtocol.bindDelegate("get_2d", function()
-- 	return gMapLogger.getScanMap()
-- end)

-- gNetProtocol.bindDelegate("loadmap", function(mapName)
-- 	local success = gMap.changeMap(mapName)
-- 	if success then
-- 		rbk.sleep(2000) --TODO
-- 		gLoc.refreshMCLocMap()
-- 	else
-- 		print("change map failed")
-- 	end
-- 	return success
-- end)

-- -- 只重载地图中的元素
-- gNetProtocol.bindDelegate("loadmap_obj", function()
-- 	return gMap.reloadMap()
-- end)

-- gNetProtocol.bindEvent("go_target", function(id, angle, maxSpeed, maxWSpeed, maxAcc, maxWAcc)
--  	print("go_target", id, angle, maxSpeed, maxWSpeed, maxAcc, maxWAcc)
-- 	task.gotoSpecifiedPose(gMoveFactory, id, angle, maxSpeed, maxWSpeed, maxAcc, maxWAcc)
-- end)

-- gNetProtocol.bindEvent("go_point", function(id, x, y, angle, maxSpeed, maxWSpeed, maxAcc, maxWAcc)
-- 	print("go_point", id, x, y, angle, maxSpeed, maxWSpeed, maxAcc, maxWAcc)
-- 	task.smartGotoPosition(gMoveFactory, id, x, y, angle)
-- end)

-- gNetProtocol.bindEvent("translate", function(dist, vx)
-- 	print("translate", dist, vx)
-- 	task.goByOdometer(gMoveFactory, dist, nil, vx, nil)
-- end)

-- gNetProtocol.bindEvent("turn", function(angle, vw)
-- 	print("turn", angle, vw)
-- 	task.goByOdometer(gMoveFactory, nil, angle, nil, vw)
-- end)

-- gNetProtocol.bindEvent("reloc", function(x, y, angle)
-- 	if angle == nil then
-- 		gLoc.relocService(x, y, 2, 0, 180)
-- 	else
-- 		gLoc.relocService(x, y, 2, angle * 180 / math.pi, 20)
-- 	end
-- end)

-- gNetProtocol.bindEvent("go_home", function()
-- 	task.gotoSpecifiedPose(gMoveFactory, "ReturnPoint")
-- end)

-- gNetProtocol.bindEvent("charge", function()
-- 	task.gotoSpecifiedPose(gMoveFactory, "ChargePoint")
-- end)

-- -- 与client连接断开
-- gNetProtocol.bindEvent("connection_lost", function()
-- 	--task.gotoSpecifiedPose(gMoveFactory, "ReturnPoint")
-- end)

-- local map_dir = {
-- 	90, 90, 90, 90, 90,
-- 	90, 90, 90, 90, 90,
-- 	-34.5, 90, 90, 90, 90, 90
-- }

-- gControl.bindEvent("recvAction", function()
-- 	local msg_type = gControl.type()
-- 	local target_x = gControl.target_x()
-- 	local target_y = gControl.target_y()
-- 	local target_angle = gControl.target_angle()
-- 	print("RecvAction Type: ",msg_type)
-- 	if msg_type == "map" then
-- 		gMap.sendMapImmediately()
-- 	elseif msg_type == "reloc" then
-- 		gLoc.relocService(target_x, target_y, 2, 2, 180)
-- 	elseif msg_type == "go_target_id" then
-- 		if gControl.target_id() >= 0 and gControl.target_id() <= 21 then
-- 			print("aaa")
-- 			task.gotoSpecifiedPose(gMoveFactory,
-- 			gControl.target_id(), map_dir[gControl.target_id()+1]*math.pi/180,
-- 			0.3, 0.3, math.pi/6.0, math.pi/6.0)
-- 		print("bbb")

-- 		end
-- 	elseif msg_type == "go" then
-- 		task.smartGotoPosition(gMoveFactory, nil, target_x, target_y, target_angle, 1)
-- 	elseif msg_type == "go_home" then
-- 		print("go home received")
-- 		local charge_x = gMap.getChargePosX("charge_pos_1")
-- 		local charge_y = gMap.getChargePosY("charge_pos_1")
-- 		local charge_dir = gMap.getChargePosDir("charge_pos_1")
-- 		local vx = -0.02
-- 		local vw = -2 * math.pi / 180.0
-- 		if charge_x == 0 and charge_y == 0 and charge_dir == 0 then
-- 			print("No charge Dock Setted")
-- 		else
-- 			task.autoCharge(gMoveFactory,11,charge_x/1000.0,charge_y/1000.0,map_dir[11]*math.pi/180,vx,vw)
-- 		end
-- 	end
-- end)

-- -- 地图加载完成
-- gLoc.bindEvent("relocFinished", function()
-- 	gNetProtocol.setLoadmapStatus(1)
-- end)

-- -- 重定位完成
-- gLoc.bindEvent("relocSuccessed", function ()
-- 	gNetProtocol.setRelocStatus(3) -- 这里只会标记为重定位完成, 需要等用户确认成功
-- end)

-- -- 重定位失败
-- gLoc.bindEvent("relocFailed", function ()
-- 	gNetProtocol.setRelocStatus(3)
-- end)

-- gMoveFactory.bindEvent("targetReached", function()
-- 	gABB.targetReached()
-- end)

-- local is_manual_mode = false
-- local is_joystick_connected = false

-- gJoystick.bindEvent("connected", function ()
-- 	is_joystick_connected = true
-- 	gNetProtocol.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	gMoveFactory.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	gJoystick.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- end)

-- gJoystick.bindEvent("disconnected", function ()
-- 	is_joystick_connected = false
-- 	if is_manual_mode then
-- 		gNetProtocol.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 		gMoveFactory.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	else
-- 		gNetProtocol.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 		gMoveFactory.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	end
-- 	gJoystick.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- end)

-- gNetProtocol.bindDelegate("manual", function ()
-- 	gMoveFactory.cancelTask();
--     is_manual_mode = true
-- 	if not is_joystick_connected then
-- 		gMoveFactory.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 		gNetProtocol.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	end
-- end)

-- gNetProtocol.bindDelegate("auto", function ()
--     is_manual_mode = false
-- 	if not is_joystick_connected then
-- 		gNetProtocol.disconnect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 		gMoveFactory.connect(gChassis, "rbk.protocol.Message_NavSpeed")
-- 	end
-- end)

-- --
-- gABB.bindDelegate("go_target", function(tar_id)
-- 	if tar_id == 4 then
-- 		print("abb: reloc")
-- 		gMap.changeMap("abb_demo_new")
-- 		rbk.sleep(4000)
-- 		gFus.disconnect(gLoc,"rbk.protocol.Message_Laser")
-- 	    gLaser.connect(gLoc,"rbk.protocol.Message_Laser")
-- 	    print("abb: relocing...")
-- 		--gLoc.relocService(1.156, 1.387, 2, 88.4, 20)
-- 		return
-- 	end
-- 	print("abb: target ", tar_id)
-- 	gLaser.disconnect(gLoc,"rbk.protocol.Message_Laser")
-- 	gFus.connect(gLoc,"rbk.protocol.Message_Laser")
-- 	if tar_id == 2 then
-- 		rbk.sleep(2000)
-- 		gChassis.calGyro()
-- 		rbk.sleep(3000)
-- 	end
-- 	if tar_id == 1 then
-- 		print("tar_id: 1")
-- 		task.gotoSpecifiedPose(gMoveFactory, tar_id, _, 0.3)
-- 	elseif tar_id == 2 then
-- 		print("tar_id: 2")
-- 		task.gotoSpecifiedPose(gMoveFactory, tar_id, _, 0.15)
-- 	elseif tar_id == 3 then
-- 		print("tar_id: 3")
-- 		task.gotoSpecifiedPose(gMoveFactory, tar_id, _, 0.05)
-- 	end
-- end)

-- gChassis.start()
-- gLoc.start()
-- gEKF.start()
-- gMap.start()
-- gControl.start()
-- gDebug.start()
-- gJoystick.start()
-- gObj.start()
-- gFus.start()
-- gMoveFactory.start()
-- gMapLogger.start()
-- if not rbkConfig.simulation then
-- 	gLaser.start()
-- end
-- if rbkConfig.useNetProtocol then
-- 	gNetProtocol.start()
-- end
