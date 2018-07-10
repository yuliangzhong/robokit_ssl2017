local RECEIVE_POS  = CGeoPoint:new_local(320,-220)

local RECEIVE_GO_POS = ball.refAntiYPos(RECEIVE_POS)

local RUN_POS_STATE_1_1 = ball.refAntiYPos(CGeoPoint:new_local(250,-150))
local RUN_POS_STATE_1_2 = ball.refAntiYPos(CGeoPoint:new_local(200,-150))
local RUN_POS_STATE_1_3 = ball.refAntiYPos(CGeoPoint:new_local(150,-150))
local RUN_POS_STATE_1_4 = ball.refAntiYPos(CGeoPoint:new_local(100,-150))

local RUN_POS_STATE_2_1 = ball.refAntiYPos(CGeoPoint:new_local(300,-150))
local RUN_POS_STATE_2_2 = ball.refAntiYPos(CGeoPoint:new_local(320,-85))
local RUN_POS_STATE_2_3 = ball.refAntiYPos(CGeoPoint:new_local(200,150))
local RUN_POS_STATE_2_4 = ball.refAntiYPos(CGeoPoint:new_local(150,150))

local RUN_POS_STATE_3_1 = ball.refAntiYPos(CGeoPoint:new_local(300,200))
local RUN_POS_STATE_3_2 = ball.refAntiYPos(CGeoPoint:new_local(390,-150))
local RUN_POS_STATE_3_3 = ball.refAntiYPos(CGeoPoint:new_local(200,200))
local RUN_POS_STATE_3_4 = ball.refAntiYPos(CGeoPoint:new_local(150,200))

local RUN_POS_STATE_4_1 = ball.refAntiYPos(CGeoPoint:new_local(200,200))
local RUN_POS_STATE_4_2 = ball.refAntiYPos(CGeoPoint:new_local(380,-115))
local RUN_POS_STATE_4_3 = ball.refAntiYPos(CGeoPoint:new_local(100,200))
local RUN_POS_STATE_4_4 = ball.refAntiYPos(CGeoPoint:new_local(50,200))

local PASS_POS = pos.passForTouch(CGeoPoint:new_local(120,150))

local USE_CHIP = true

gPlayTable.CreatePlay{

firstState = "State1",

["State1"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Leader") < 15 and
			      player.toTargetDist("Special") < 15, 1, 180) then
			return "State2"
		end
	end,
	Assister = USE_CHIP and task.staticGetBall(RECEIVE_POS) or task.staticGetBall(RECEIVE_GO_POS),
	Leader   = task.goCmuRush(RUN_POS_STATE_1_1, dir.playerToBall),
	Special  = task.goCmuRush(RUN_POS_STATE_1_2),
	Middle   = task.goCmuRush(RUN_POS_STATE_1_3),
	Defender = task.goCmuRush(RUN_POS_STATE_1_4),
	Goalie   = task.goalie(),
	match    = "{A}[LSMD]"
},

["State2"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Leader") < 15 and
			      player.toTargetDist("Special") < 30, 1, 120) then
			return "State3"
		end
	end,
	Assister = USE_CHIP and task.staticGetBall(RECEIVE_POS) or task.staticGetBall(RECEIVE_GO_POS),
	Leader   = task.goCmuRush(RUN_POS_STATE_2_1, dir.playerToBall),
	Special  = task.goCmuRush(RUN_POS_STATE_2_2, dir.playerToBall),
	Middle   = task.goCmuRush(RUN_POS_STATE_2_3),
	Defender = task.goCmuRush(RUN_POS_STATE_2_4),
	Goalie   = task.goalie(),
	match    = "{A}{LSMD}"
},

["State3"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Leader") < 20, 1, 120) then
			return "State4"
		end
	end,
	Assister = USE_CHIP and task.staticGetBall(RECEIVE_POS) or task.staticGetBall(RECEIVE_GO_POS),
	Leader   = task.goCmuRush(RECEIVE_GO_POS, dir.playerToBall),
	Special  = task.goCmuRush(RUN_POS_STATE_3_2, dir.playerToBall),
	Middle   = task.goCmuRush(RUN_POS_STATE_3_3),
	Defender = task.goCmuRush(RUN_POS_STATE_3_4),
	Goalie   = task.goalie(),
	match    = "{A}{LSMD}"
},

["State4"] = {
	switch = function ()
		if bufcnt(player.isBallPassed("Assister", "Leader"), 1, 150) then
			return "Receive"
		end
	end,
	Assister = USE_CHIP and task.chipPass(RECEIVE_POS, 5555+40, false) or task.goAndTurnKick(RECEIVE_GO_POS, 560),
	Leader   = task.goCmuRush(RECEIVE_GO_POS, dir.playerToBall),
	Special  = task.goCmuRush(RUN_POS_STATE_4_2),
	Middle   = task.leftBack(),
	Defender = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{A}{LS}[MD]"
},

["Receive"] = {
	switch = function ()
		if bufcnt(player.isBallPassed("Leader", "Assister"), "fast", 150) then
			return "Fixgoto"
		end
	end,
	Assister = task.goCmuRush(PASS_POS),
	Leader   = task.receivePass(PASS_POS, 5555+20),
	Special  = task.goCmuRush(RUN_POS_STATE_4_2),
	Middle   = task.leftBack(),
	Defender = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{ALS}[MD]"
},

["Fixgoto"] = {
    switch = function ()
		if bufcnt(true, 5) then
			return "Shoot"
		end
	end,
	Assister = task.goCmuRush(PASS_POS),
	Leader   = task.goSupportPos("Assister"),
	Special  = task.goCmuRush(RUN_POS_STATE_4_2),
	Middle   = task.leftBack(),
	Defender = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{ALS}[DM]"
},

["Shoot"] = {
    switch = function ()
		if bufcnt(player.kickBall("Assister"), 1, 150) then
			return "exit"
		end
	end,
	Assister = task.shoot(),
	Leader   = task.goSupportPos("Assister"),
	Special  = task.goCmuRush(RUN_POS_STATE_4_2),
	Middle   = task.leftBack("Special"),
	Defender = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{ALS}[DM]"
},

name = "Ref_MiddleKickV7",
applicable = {
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
