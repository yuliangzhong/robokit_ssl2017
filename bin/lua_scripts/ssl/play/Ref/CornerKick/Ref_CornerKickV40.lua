-- 2012年进SKUBA战术的加强版，前面跑位一模一样，后面的冲刺换成后卫前冲
-- by zhyaic 2013.6.16
-- 2014-07-20 yys 改

local FINAL_SHOOT_POS = CGeoPoint:new_local(270,120)
local MIDDEL_FAKE_POS = ball.refAntiYPos(CGeoPoint:new_local(100,200))
local TMP_POS   = ball.refAntiYPos(CGeoPoint:new_local(220,0))
local SYNT_POS  = ball.refSyntYPos(CGeoPoint:new_local(220,200))
local ANTIPOS_1 = ball.refAntiYPos(CGeoPoint:new_local(120,120))
local ANTIPOS_2 = ball.refAntiYPos(CGeoPoint:new_local(200,120))
local ANTIPOS_3 = ball.refAntiYPos(FINAL_SHOOT_POS)
local CHIP_POS  = pos.passForTouch(FINAL_SHOOT_POS)
local DEFENDER_READY_POS = ball.refAntiYPos(CGeoPoint:new_local(-150,100))

local ANTI_FAKE_POS = ball.refAntiYPos(CGeoPoint:new_local(415,160))

gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Special") < 60 and
		   player.toTargetDist("Leader") < 60,  "normal", 180) then
			return "goalone"
		end
	end,
	Assister = task.staticGetBall(CHIP_POS),
	Special  = task.goCmuRush(TMP_POS),
	Leader   = task.goCmuRush(SYNT_POS),
	Middle   = task.goCmuRush(MIDDEL_FAKE_POS),
	Defender = task.goCmuRush(DEFENDER_READY_POS),
	Goalie   = task.goalie(),
	match    = "{A}{DLSM}"
},

["goalone"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Defender") < 60, "fast", 180) then
			return "goget"
		end
	end,
	Assister = task.slowGetBall(CHIP_POS),
	Defender = task.goCmuRush(ANTIPOS_1),
	Special  = task.goCmuRush(SYNT_POS),
	Middle   = task.goCmuRush(ANTI_FAKE_POS),
	Leader   = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{DALSM}"
},

["goget"] = {
    switch = function ()
		if bufcnt(player.toTargetDist("Defender") < 40, "fast", 180) then
			return "gopass"
		end
	end,
	Defender = task.goCmuRush(ANTIPOS_2),
	Assister = task.slowGetBall(CHIP_POS),
	Special  = task.goCmuRush(SYNT_POS),
	Middle   = task.goCmuRush(ANTI_FAKE_POS),
	Leader   = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{DALSM}"
},

["gopass"] = {
    switch = function ()
		if bufcnt(player.kickBall("Assister") or
				  player.toBallDist("Assister") > 30,"fast") then
			return "goopen"
		elseif  bufcnt(true, 120) then
			return "exit"
		end
	end,
	Defender = task.goCmuRush(ANTIPOS_3),
	Assister = task.chipPass(CHIP_POS, 300),
	Special  = task.goCmuRush(SYNT_POS),
	Middle   = task.goCmuRush(ANTI_FAKE_POS),
	Leader   = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{DALSM}"
},

["goopen"] = {
    switch = function ()
		if bufcnt(player.toTargetDist("Defender") < 35, "fast", 180) then
			return "gokick"
		end
	end,
	Defender = task.goCmuRush(ball.refAntiYPos(FINAL_SHOOT_POS)),
	Assister = task.goPassPos("Defender"),
	Special  = task.defendMiddle(),
	Middle   = task.goCmuRush(ANTI_FAKE_POS),
	Leader   = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{DALSM}"
},

["gokick"] = {
    switch = function ()
		if player.kickBall("Defender") then
			return "finish"
		elseif bufcnt(true, 90) then
			return "exit"
		end
	end,
	Defender = task.touch(),
	Assister = task.defendMiddle("Defender"),
	Middle   = task.goCmuRush(ANTI_FAKE_POS),
	Special  = task.leftBack(),
	Leader   = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{DM}(ALS)"
},

name = "Ref_CornerKickV40",
applicable ={
	exp = "a",
	a = true
},
score = 0,
attribute = "attack",
timeout = 99999
}