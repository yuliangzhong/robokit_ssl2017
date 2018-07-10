--直接向对方门里挑一脚

local ChipDir = function ( p )  --第一次挑传传球方向
	return function (  )
		return (ball.refSyntYPos(p)() - ball.pos()):dir()
	end
end

local RushPos_Leader = ball.refAntiYPos(CGeoPoint:new_local(150,150))
local RushPos_Special = ball.refAntiYPos(CGeoPoint:new_local(100,100))

gPlayTable.CreatePlay{
	firstState = "Start",
	["Start"] = {
		switch = function()
			 if  bufcnt(player.toTargetDist("Assister") < 30 and player.toTargetDist("Special") < 30, 40, 120) then
			 	return "crazyChip"
			 end
		end,
		Leader   = task.goCmuRush(RushPos_Leader),
		Assister = task.slowGetBall(CGeoPoint:new_local(300,0)),
		Special  = task.goCmuRush(RushPos_Special),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{A}[MDLS]"
	},
	["crazyChip"] = {
		switch = function()
			if  bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 20, "normal",120) then
			 	return "Fix"
			 end
		end,
		Leader   = task.goCmuRush(RushPos_Leader),
		Assister = task.chipPass(ChipDir(CGeoPoint:new_local(300,0)),9999),
		Special  = task.goCmuRush(RushPos_Special),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{AMD}[LS]"
	},
	["Fix"] = {
	switch = function()
		if  bufcnt(true,20) then
		 	return "Advance"
		 end
	end,
		Leader   = task.goCmuRush(RushPos_Leader),
		Assister = task.defendMiddle(),
		Special  = task.goCmuRush(RushPos_Special),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{AMD}[LS]"
	},
	["Advance"] = {
	switch = function()
		if  bufcnt(true,20) then
		 	return "exit"
		 end
	end,
		Leader   = task.advance(),
		Assister = task.defendMiddle(),
		Special  = task.advance(),
		Middle   = task.leftBack(),
		Defender = task.rightBack(),
		Goalie   = task.goalie(),
		match    = "{AMD}[LS]"
	},
	name 	   	= "Ref_BackKickV6",
	applicable 	={
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}
