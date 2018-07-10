local FIRST_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(190, -150))
local FIRST_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(-100, 170))

local SENCOND_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(190, -50))
local SENCOND_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(150, 70))

local THIRD_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(-43, -26))


gPlayTable.CreatePlay{

	firstState = "start",

	["start"] = {
		switch = function() 
			if bufcnt(player.toTargetDist("Leader") < 20 and
				      player.toTargetDist("Special") < 20, "normal", 120) then
				return "runPos"
			end 
		end,
		Assister = task.staticGetBall(CGeoPoint:new_local(300, 0)),
		Leader   = task.goCmuRush(FIRST_POS_1),
		Special  = task.goCmuRush(FIRST_POS_2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{A}{LSMD}"
	},

	["runPos"] = {
		switch = function() 
			if bufcnt(player.toTargetDist("Leader") < 20 and
				      player.toTargetDist("Special") < 20, "normal", 120) then
				return "passBall"
			end 
		end,
		Assister = task.slowGetBall(SENCOND_POS_2()),
		Leader   = task.goCmuRush(SENCOND_POS_1),
		Special  = task.goCmuRush(SENCOND_POS_2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALSMD}"
	},

	["passBall"] = {
		switch = function() 
			if bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 20, "fast", 120) then
				return "receivePass"
			end 
		end,
		Assister = task.chipPass(SENCOND_POS_2(), 240),
		Leader   = task.goCmuRush(SENCOND_POS_1),
		Special  = task.goCmuRush(SENCOND_POS_2),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALSMD}"
	},

	["receivePass"] = {
		switch = function() 
			if bufcnt(player.kickBall("Special"), "fast", 120) then
				return "touch"
			end 
		end,
		Assister = task.goCmuRush(THIRD_POS_1),
		Leader   = task.goCmuRush(SENCOND_POS_1),
		Special  = task.receivePass(CGeoPoint:new_local(0, 0), 300),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALSMD}"
	},

	["touch"] = {
		switch = function() 
			if bufcnt(player.kickBall("Special"), "fast", 120) then
				return "finish"
			else bufcnt(true, 80)
			end 
		end,
		Assister = task.touch(),
		Leader   = task.goCmuRush(SENCOND_POS_1),
		Special  = task.stop(),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALSMD}"
	},
	
	name 	   	= "Ref_BackKickTest",
	applicable 	= {
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}
