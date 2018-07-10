-- 模拟SKUBA打Immortal时的一招，让后卫拉反边进行射门
-- 打Immortal时实际效果可能并不好，关键在于转身的速度要快
-- by zhyaic 2013.6.17

local TMP_POS1  = ball.refAntiYPos(CGeoPoint:new_local(0,0))
local SHOOT_POS = function()
	return CGeoPoint:new_local(ball.refPosX() - 50, ball.refAntiYDir(150)())
end

local TOUCH_POS_1 = ball.refSyntYPos(CGeoPoint:new_local(130,35))
local TOUCH_POS_2 = ball.refSyntYPos(CGeoPoint:new_local(250,150))

gPlayTable.CreatePlay{

firstState = "quickback",

["quickback"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Middle") < 50 and
				player.toTargetDist("Special") < 30, "normal") then
			return "gopos"
		end
	end,
	Assister = task.staticGetBall(CGeoPoint:new_local(450,0)),
	Middle   = task.goCmuRush(ball.jamPos(CGeoPoint:new_local(325,0), 60, 10)),
	Special  = task.goCmuRush(TMP_POS1),
	Leader   = task.leftBack(),
	Defender = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{A}(MS)(LD)"
},

["gopos"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Leader") < 150, "fast", 180) then
			return "gopass"
		end
	end,
	Assister = task.staticGetBall(CGeoPoint:new_local(450,0)),
	Middle   = task.goCmuRush(ball.jamPos(CGeoPoint:new_local(325,0), 60, 10)),
	Special  = task.goCmuRush(ball.jamPos(CGeoPoint:new_local(325,25), 80, 25)),
	Leader   = task.goCmuRush(SHOOT_POS),
	Defender = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{A}{MS}(LD)"
},

["gopass"] = {
	switch = function ()
		if bufcnt(player.kickBall("Assister") or
				  player.isBallPassed("Assister","Leader"), "fast")then
			return "gokick"
		elseif bufcnt(true, 90) then
			return "exit"
		end
	end,
	Assister = task.goAndTurnKick(pos.passForTouch(SHOOT_POS), 400),
	Middle   = task.goCmuRush(ball.jamPos(CGeoPoint:new_local(325,0), 60, 10)),
	Special  = task.touchBetweenPos(TOUCH_POS_1,TOUCH_POS_2),
	Leader   = task.goCmuRush(SHOOT_POS),
	Defender = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{A}{MSLD}"
},

["gokick"] = {
	switch = function ()
		if bufcnt(player.kickBall("Leader"), "fast", 60) then
			return "exit"
		end
	end,
	Special  = task.touchBetweenPos(TOUCH_POS_1,TOUCH_POS_2),
	Middle   = task.defendMiddle("Leader"),
	Leader   = task.touch(),
	Assister = task.rightBack(),
	Defender = task.leftBack(),
	Goalie   = task.goalie(),
	match    = "{LS}(MAD)"
},

name = "Ref_MiddleKickV3",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}