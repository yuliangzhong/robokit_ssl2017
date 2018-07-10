-- 两传一射，针对MRL
-- by zhyaic 2014.04.07
-- modified by zhyaic 2014.07.24

-- local DO_BALL_POS_ORG = CGeoPoint:new_local(300, -180)
-- local DO_BALL_POS = ball.refAntiYPos(DO_BALL_POS_ORG)
-- local SHOOT_BALL_POS = ball.refAntiYPos(CGeoPoint:new_local(260, 120))
-- local JAM_POS = ball.jamPos(CGeoPoint:new_local(325, 0),60,10)
-- local TMP_MIDDLE_POS = ball.refSyntYPos(CGeoPoint:new_local(-75, 0))
-- 大场：
local DO_BALL_POS_ORG = CGeoPoint:new_local(260, -250)
local DO_BALL_POS = ball.refAntiYPos(DO_BALL_POS_ORG)
local SHOOT_BALL_POS = ball.refAntiYPos(CGeoPoint:new_local(100, -100))
local JAM_POS = ball.jamPos(CGeoPoint:new_local(325, 0),60,10)
local TMP_MIDDLE_POS = ball.refSyntYPos(CGeoPoint:new_local(-175, 0))

gPlayTable.CreatePlay{
	firstState = "start",

	["start"] = {
		switch = function ()
			if  bufcnt( player.toTargetDist("Special") < 30 and
						player.toTargetDist("Leader") < 30 and
					--player.toTargetDist("Middle") < 30, 10, 120) then
            player.toTargetDist("Middle") < 30, 10, 200) then
				return "firstPass"
			end
		end,
		Assister = task.staticGetBall(DO_BALL_POS_ORG),
		Special  = task.goCmuRush(DO_BALL_POS, player.toBallDir),
		Leader	 = task.goCmuRush(SHOOT_BALL_POS),
		Middle   = task.goCmuRush(JAM_POS),
		Defender = task.singleBack(),
		Goalie   = task.goalie(),
		match    = "{A}[SLMD]"
	},

	["firstPass"] = {
		switch = function ()
			if bufcnt(player.kickBall("Assister") or player.isBallPassed("Assister", "Special"), 1, 120) then
				return "secondPass"
			end
		end,
		-- Assister = task.goAndTurnKick(DO_BALL_POS, 500),
		Assister = task.goAndTurnKick(DO_BALL_POS, 5555+35),
		Leader	 = task.goCmuRush(SHOOT_BALL_POS),
		Middle   = task.goCmuRush(JAM_POS),
		Special  = task.goCmuRush(DO_BALL_POS, player.toBallDir),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ASLMD}"
	},

	["secondPass"] = {
		switch = function ()
			if bufcnt(player.kickBall("Special") or player.isBallPassed("Special", "Leader"), 1, 180) then
				return "shoot"
			elseif bufcnt(true, 200) then
				return "exit"
			end
		end,
		Assister = task.leftBack(),
		Leader	 = task.goCmuRush(SHOOT_BALL_POS),
		Middle   = task.defendMiddle(),
		Special  = task.receivePass("Leader", 5555+30),
		--Special  = task.touchPass(SHOOT_BALL_POS, 5555+30),
    	Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{SLM}[AD]"
	},

	["shoot"] = {
		switch = function ()
			if  bufcnt(player.kickBall("Leader"), 1, 90) then
				return "exit"
			end
		end,
		Assister = task.leftBack(),
		Leader	 = task.shoot(),
		Middle   = task.defendMiddle("Leader"),
		Special  = task.stop(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{SLM}[AD]"
	},

	name 	   	= "Ref_BackKickV10",
	applicable = {
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}
