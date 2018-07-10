--开球车上前射门
-- local MakeBallPos = ball.refAntiYPos(CGeoPoint:new_local(130, 0))
-- local AtrPos1 = ball.refAntiYPos(CGeoPoint(170, 180))
-- local AtrPos2 = ball.refAntiYPos(CGeoPoint:new_local(220, 180))
-- local ShootBallPos = ball.refSyntYPos(CGeoPoint:new_local(20, 180))

local MakeBallPos = ball.refAntiYPos(CGeoPoint:new_local(-100, 0))
local AtrPos1 = ball.refAntiYPos(CGeoPoint(270, 280))
local AtrPos2 = ball.refAntiYPos(CGeoPoint:new_local(220, 180))
local ShootBallPos = ball.refSyntYPos(CGeoPoint:new_local(20, 180))
gPlayTable.CreatePlay{
	firstState = "Start",

	["Start"] = {
		switch = function()
			if bufcnt(player.toTargetDist("Leader") < 30 and player.InfoControlled("Assister")
								--and player.toTargetDist("Special") < 30, 80, 120) then
                and player.toTargetDist("Special") < 30, 80, 200) then
				return "FirstPass"
			end
		end,
		Leader   = task.goCmuRush(MakeBallPos, player.toBallDir),
		Assister = task.slowGetBall(MakeBallPos, false),
		Special  = task.goCmuRush(AtrPos1),
		Middle   = task.goCmuRush(AtrPos2),
		Defender = task.singleBack(),
		Goalie   = task.goalie(),
		match    = "{A}{LMDS}"
	},

	["FirstPass"] = {
		switch = function()
			if bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 30, "normal", 180) then
			 	return "FixGo"
			elseif bufcnt(true, 160) then
				return "exit"
			end
		end,
		Leader   = task.goCmuRush(MakeBallPos, player.toBallDir),
		-- Assister = task.chipPass(MakeBallPos, 200, false),
		Assister = task.chipPass(MakeBallPos,9999, false),
		Special  = task.goCmuRush(AtrPos1),
		Middle   = task.goCmuRush(AtrPos2),
		Defender = task.singleBack(),
		Goalie   = task.goalie(),
		match    = "{ALMDS}"
	},

	["FixGo"] = {
		switch = function()
			if bufcnt(true, 30) then
			 	return "SecondPass"
			end
		end,
		Leader   = task.goCmuRush(MakeBallPos, player.toBallDir),
		Assister = task.goCmuRush(ShootBallPos),
		Special  = task.leftBack(),
		Middle   = task.defendMiddle(),
		Defender = task.singleBack(),
		Goalie   = task.goalie(),
		match    = "{ALMDS}"
	},

	["SecondPass"] = {
		switch = function()
			if bufcnt(player.kickBall("Leader") or player.isBallPassed("Leader", "Assister"), "fast") then
			 	return "Kick"
			elseif bufcnt(true, 120) then
			 	return "exit"
			end
		end,
		Leader   = task.receivePass("Assister"),
		Assister = task.goCmuRush(ShootBallPos),
		Special  = task.leftBack(),
		Middle   = task.defendMiddle(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALMDS}"
	},

	["Kick"] = {
		switch = function()
			if bufcnt(player.kickBall("Assister"), "fast", 100) then
        return "exit"
			end
		end,
		Leader   = task.leftBack(),
		Assister = task.shoot(),
		Special  = task.leftBack(),
		Middle   = task.defendMiddle(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{ALMDS}"
	},
	name 	   	= "Ref_BackKickV5",
	applicable = {
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}
