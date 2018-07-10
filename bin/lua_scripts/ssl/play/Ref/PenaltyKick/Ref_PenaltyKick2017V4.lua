local SIDE_POS, preparePoint, INTER_POS = pos.refStopAroundBall()
local  ACC = 200
local STOP_FLAG = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS = bit:_or(STOP_FLAG, flag.allow_dss)

local goal      = CGeoPoint:new_local(0,100)

local penaltyV4Param = {
	flatKickPower = 5555+16,
	chipKickPower = 45,
	faltKickDirection = math.pi/6,
	mode = 5555+5
}

gPlayTable.CreatePlay{

firstState = "prepare",

	["prepare"] = {
		switch = function ()
			if bufcnt(player.toPointDist("Goalie",preparePoint)<50,"fast") then
				return "goto"
			end
		end,
		Goalie = task.goCmuRush(preparePoint, dir.playerToBall, ACC, STOP_DSS),
		match = ""
	},

	["goto"] = {
		switch = function ()
			if cond.isNormalStart() then
				--return "tempRun1"
				return "KickOut"
			-- elseif cond.isGameOn() then
			-- 	return "exit"
			end
		end,
		Goalie   = task.goBackBall(penaltyV4Param.faltKickDirection,20),
		match    = ""
	},

	-- ["KickOut"] = {
	-- switch = function ()
	-- 	-- if cond.isGameOn() then
	-- 	-- 	return "exit"
	-- 	-- end
	-- end,
	-- Goalie    = task.penaltyKick2017V4(penaltyV4Param["flatKickPower"],penaltyV4Param["chipKickPower"],penaltyV4Param["faltKickDirection"],penaltyV4Param["mode"]),
	-- match = ""
	-- },

	["KickOut"] = {
		switch = function ()
			if bufcnt(player.kickBall("Goalie"),"fast") then
				return "tempRun1"
			end
		end,
		Goalie = task.passToDir(dir.specified(penaltyV4Param.faltKickDirection*180/math.pi),penaltyV4Param["flatKickPower"]),
		match = ""
	},

	["tempRun1"] = {
		switch =function ()
			if bufcnt(ball.posX()>-80,"fast") then
				return "shoot"
			end
		end,
		Goalie = task.goBackBall(CGeoPoint:new_local(450,0),15),
		match = ""
	},

	["shoot"] = {
		switch = function()
		end,
		Goalie = task.shoot(),
		match = ""
	},


	name = "Ref_PenaltyKick2017V4",

	applicable ={
		exp = "a",
		a = true
				},

	attribute = "attack",
	timeout = 99999
}
