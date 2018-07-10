-- 用在中场靠中间位置的定位球，主要要考虑到球的y值会在正负之前跳动

local MAKE_POS_ORG    = CGeoPoint:new_local(300, 250)
local RIGHT_POS   = ball.refSyntYPos(CGeoPoint:new_local(280, 250))
local RIGHT_POS2  = ball.refSyntYPos(CGeoPoint:new_local(250, 250))
local MAKE_POS    = ball.refAntiYPos(MAKE_POS_ORG)
local READY_POS   = ball.refSyntYPos(CGeoPoint:new_local(-150, -150))
local SHOOT_POS   = ball.refSyntYPos(CGeoPoint:new_local(70, -150))

gPlayTable.CreatePlay{

firstState = "getready",

["getready"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Special")<30 and
			player.toTargetDist("Leader")<30, "normal") then
			return "startball"
		end
	end,
	Assister = task.staticGetBall(MAKE_POS_ORG),
	Leader   = task.goCmuRush(CGeoPoint:new_local(0,120)),
	Special  = task.goCmuRush(CGeoPoint:new_local(0,-120)),
	Defender = task.leftBack(),
	Middle   = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{A}[LDSM]"
},

["startball"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Special")<30 and
			player.toTargetDist("Leader")<30, 1, 180) then
			return "getball"
		end
	end,
	Assister = task.staticGetBall(MAKE_POS_ORG),
	Middle   = task.goCmuRush(RIGHT_POS2),
	Special  = task.goCmuRush(RIGHT_POS),
	Leader 	 = task.goCmuRush(MAKE_POS, player.toBallDir),
	Defender = task.goCmuRush(READY_POS),
	Goalie   = task.goalie(),
	match    = "{A}[LDSM]"
},

["getball"] = {
	switch = function ()
		if bufcnt(true, 20) then
			return "chippass"
		end
	end,
	Assister = task.slowGetBall(MAKE_POS_ORG),
	Middle   = task.goCmuRush(RIGHT_POS2),
	Special  = task.goCmuRush(RIGHT_POS),
	Leader 	 = task.goCmuRush(MAKE_POS, player.toBallDir),
	Defender = task.goCmuRush(READY_POS),
	Goalie   = task.goalie(),
	match    = "{ALDSM}"
},

["chippass"] = {
	switch = function ()
		if  player.kickBall("Assister") or
			player.toBallDist("Assister") > 20 then
			return "fixgoto"
		end
	end,
	Assister = task.chipPass(MAKE_POS_ORG, 300),
	Middle   = task.goCmuRush(RIGHT_POS2),
	Special  = task.goCmuRush(RIGHT_POS),
	Leader 	 = task.goCmuRush(MAKE_POS, player.toBallDir),
	Defender = task.goCmuRush(SHOOT_POS),
	Goalie   = task.goalie(),
	match    = "{ALDSM}"
},

["fixgoto"] = {
	switch = function ()
		if bufcnt(true, 30) then
			return "makeball"
		end
	end,
	Middle   = task.goCmuRush(RIGHT_POS2),
	Special  = task.goCmuRush(RIGHT_POS),
	Leader 	 = task.goCmuRush(MAKE_POS, player.toBallDir),
	Defender = task.goCmuRush(SHOOT_POS),
	Assister = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{ALDSM}"
},

["makeball"] = {
	switch = function ()
		if 	player.kickBall("Leader") or
			player.isBallPassed("Leader", "Defender") then
			return "gokick"
		elseif bufcnt(true, 150) then
			return "exit"
		end
	end,
	Middle   = task.rightBack(),
	Special  = task.defendMiddle(),
	Leader 	 = task.receivePass("Defender"),
	Defender = task.goCmuRush(SHOOT_POS),
	Assister = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{ALDSM}"
},

["gokick"] = {
    switch = function ()
		if player.kickBall("Defender") then
			return "finish"
		elseif  bufcnt(true, 90) then
			return "exit"
		end
	end,
	Defender = task.touch(),
	Middle   = task.rightBack(),
	Special  = task.defendMiddle(),
	Leader   = task.stop(),
	Assister = task.leftBack(),
	Goalie   = task.goalie(),
	match    = "{LD}(ASM)"
},

name = "Ref_FrontKickV40",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}