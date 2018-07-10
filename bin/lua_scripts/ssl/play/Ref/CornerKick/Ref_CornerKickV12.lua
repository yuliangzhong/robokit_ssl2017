-- by yys 2015.04.22

local Left_POS     = ball.refAntiYPos(CGeoPoint:new_local(300, 40))
local Middle_POS   = ball.refAntiYPos(CGeoPoint:new_local(295, 15))
local Right_POS    = ball.refAntiYPos(CGeoPoint:new_local(295, -15))
local Other_POS    = ball.refAntiYPos(CGeoPoint:new_local(300, -40))

local Block_POS_1  = ball.refAntiYPos(CGeoPoint:new_local(330, 15))
local Block_POS_2  = ball.refAntiYPos(CGeoPoint:new_local(330, -15))

local RUN_POS_1    = {
  ball.refAntiYPos(CGeoPoint:new_local(210, 15)),
  ball.refAntiYPos(CGeoPoint:new_local(80, 120)),
  ball.refAntiYPos(CGeoPoint:new_local(150, 200)),
}
local RUN_POS_2    = {
  ball.refAntiYPos(CGeoPoint:new_local(210, -15)),
  ball.refAntiYPos(CGeoPoint:new_local(30, -120)),
  ball.refAntiYPos(CGeoPoint:new_local(220, -200)),
}

local PASS_POS = pos.passForTouch(CGeoPoint:new_local(330, 15))

gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Middle") < 10 and
              player.toTargetDist("Special") < 10 and
              player.toTargetDist("Leader") < 10 and
              player.toTargetDist("Defender") < 10, 120, 240) then
      return "goalone"
    end
  end,
  Assister = task.staticGetBall(PASS_POS),
  Special  = task.goCmuRush(Left_POS, _, 400, flag.allow_dss),
  Leader   = task.goCmuRush(Middle_POS, _, 400, flag.allow_dss),
  Middle   = task.goCmuRush(Right_POS, _, 400, flag.allow_dss),
  Defender = task.goCmuRush(Other_POS, _, 400, flag.allow_dss),
  Goalie   = task.goalie(),
  match    = "{A}{LSDM}"
},

["goalone"] = {
  switch = function()
    if bufcnt(player.toPointDist("Middle", RUN_POS_2[3]()) < 130, 1, 120) then
      return "pass"
    end
  end,
  Assister = task.slowGetBall(PASS_POS),
  Special  = task.goSimplePos(Block_POS_1),
  Leader   = task.runMultiPos(RUN_POS_1, false, 30),
  Middle   = task.runMultiPos(RUN_POS_2, false, 40),
  Defender = task.goSimplePos(Block_POS_2),
  Goalie   = task.goalie(),
  match    = "{ALSMD}"
},

["pass"] = {
  switch = function()
    if player.kickBall("Assister") or player.toBallDist("Assister") > 20 then
      return "kick"
    elseif bufcnt(true, 120) then
      return "exit"
    end
  end,
  Assister = task.chipPass(PASS_POS, 1),
  Special  = task.goSimplePos(Block_POS_1),
  Leader   = task.leftBack(),
  Middle   = task.continue(),
  Defender = task.goSimplePos(Block_POS_2),
  Goalie   = task.goalie(),
  match    = "{ASDM}[L]"
},

["kick"] = {
    switch = function()
    if player.kickBall("Special") then
      return "finish"
    elseif bufcnt(true, 90) then
      return "exit"
    end
  end,
  Assister = task.goSupportPos("Special"),
  Special  = task.waitTouch(Block_POS_1, 0),
  Leader   = task.leftBack(),
  Middle   = task.rightBack(),
  Defender = task.waitTouch(Block_POS_2, 0),
  Goalie   = task.goalie(),
  match    = "{ADS}[LM]"
},

name = "Ref_CornerKickV12",
applicable = {
  exp = "a",
  a   = true
},
score = 0,
attribute = "attack",
timeout   = 99999
}