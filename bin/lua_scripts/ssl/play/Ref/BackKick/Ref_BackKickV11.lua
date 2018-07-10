-- MRL的后场定位球，用于防守测试
-- by zhyaic 2014.04.07

local FRONT_POS1 = ball.refSyntYPos(CGeoPoint:new_local(175,30))
local FRONT_POS2 = ball.refSyntYPos(CGeoPoint:new_local(180,70))
local CHIP_POS	 = CGeoPoint:new_local(177.5,-55)

gPlayTable.CreatePlay{
	firstState = "start",

	["start"] = {
		switch = function ()
			if  bufcnt( player.toTargetDist("Special") < 30 and
						player.toTargetDist("Leader") < 30, 10, 120) then	
				return "chip"
			end
		end,
		Assister = task.staticGetBall(CHIP_POS),
		Special  = task.goCmuRush(FRONT_POS1),
		Leader	 = task.goCmuRush(FRONT_POS2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{A}[SLMD]"
	},

	["chip"] = {
		switch = function ()
			if  bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 20, 1, 120) then
				return "rush"
			end	
		end,
		Assister = task.chipPass(CHIP_POS),
		Special  = task.goCmuRush(FRONT_POS1),
		Leader	 = task.goCmuRush(FRONT_POS2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ASLMD}"
	},

	["rush"] = {
		switch = function ()
			if  bufcnt(true, 120) then
				return "exit"
			end	
		end,
		Assister = task.chase(),
		Special  = task.goCmuRush(FRONT_POS1),
		Leader	 = task.goCmuRush(FRONT_POS2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ASLMD}"
	},

	name 	   	= "Ref_BackKickV11",
	applicable 	={
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}


