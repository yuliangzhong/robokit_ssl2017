-- Immortal角球改进版,大场地版
-- by yys 2014.03.12

local POS_1 = ball.refAntiYPos(CGeoPoint:new_local(200, -100))
local POS_2 = ball.refAntiYPos(CGeoPoint:new_local(240, 0))
local POS_3 = ball.refAntiYPos(CGeoPoint:new_local(150, 200))

local Left_POS     = ball.refAntiYPos(CGeoPoint:new_local(315, 25))
local Middle_POS   = ball.refAntiYPos(CGeoPoint:new_local(310, 0))
local Right_POS    = ball.refAntiYPos(CGeoPoint:new_local(315, -25))

local Block_POS_1  = ball.refAntiYPos(CGeoPoint:new_local(310, 50))
local Block_POS_2  = ball.refAntiYPos(CGeoPoint:new_local(305, 30))
local Block_POS_3  = ball.refAntiYPos(CGeoPoint:new_local(360, 120))

local RUN_POS_1    = ball.refAntiYPos(CGeoPoint:new_local(190, 140))
local RUN_POS_2    = ball.refAntiYPos(CGeoPoint:new_local(280, 200))
local CHEAT_POS_1  = ball.refAntiYPos(CGeoPoint:new_local(150, -100))
local CHEAT_POS_2  = ball.refAntiYPos(CGeoPoint:new_local(250, -150))

gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Leader") < 10 and
              player.toTargetDist("Special") < 10, 1, 180) then
      return "run"
    end
  end,
  Assister = task.staticGetBall(CGeoPoint:new_local(200, 0)),
  Special  = task.goCmuRush(POS_3),
  Leader   = task.goCmuRush(POS_1),
  Middle   = task.goCmuRush(POS_2),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{A}{LSM}[D]"
},

["run"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Special") < 10 and
              player.toTargetDist("Leader") < 10, 10, 180) then
      return "goalone"
    end
  end,
  Assister = task.staticGetBall(CGeoPoint:new_local(200, 0)),
  Special  = task.goCmuRush(Right_POS),
  Leader   = task.goCmuRush(Left_POS),
  Middle   = task.goCmuRush(Middle_POS),
  Defender = task.goCmuRush(CHEAT_POS_1),
  Goalie   = task.goalie(),
  match    = "{A}{LSDM}"
},

["goalone"] = {
  switch = function()
    if bufcnt(player.toTargetDist("Middle") < 30, "fast", 120) then
      return "pass"
    end
  end,
  Assister = task.staticGetBall(CGeoPoint:new_local(200, 0)),
  Special  = task.goSimplePos(Block_POS_2),
  Leader   = task.goSimplePos(Block_POS_1),
  Middle   = task.goCmuRush(RUN_POS_1),
  Defender = task.goCmuRush(CHEAT_POS_2),
  Goalie   = task.goalie(),
  match    = "{ALSMD}"
},

["pass"] = {
  switch = function()
    if player.kickBall("Assister") or player.toBallDist("Assister") > 20 then
      return "gofix"
    elseif bufcnt(true, 120) then
      return "exit"
    end
  end,
  Assister = task.chipPass(CGeoPoint:new_local(360, 120),200),
  Special  = task.goSimplePos(Block_POS_2),
  Leader   = task.goSimplePos(Block_POS_3),
  Middle   = task.goTouchPos(RUN_POS_2),
  Defender = task.goCmuRush(CHEAT_POS_1),
  Goalie   = task.goalie(),
  match    = "{ASDLM}"
},

["gofix"] = {
  switch = function()
    if bufcnt(true, 15) then
      return "kick"
    end
  end,
  Assister = task.stop(),
  Special  = task.goSimplePos(Block_POS_2),
  Leader   = task.goSimplePos(Block_POS_3),
  Middle   = task.goCmuRush(RUN_POS_2),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{ASDLM}"
},

["kick"] = {
    switch = function()
    if player.kickBall("Leader") then
      return "finish"
    elseif bufcnt(true, 90) then
      return "exit"
    end
  end,
  Special  = task.rightBack(),
  Assister = task.goPassPos("Leader"),
  Middle   = task.defendMiddle("Leader"),
  Leader   = task.waitTouch(Block_POS_3, 0),
  Defender = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{ALM}[SD]"
},

name = "Ref_CornerKickV23",
applicable = {
  exp = "a",
  a   = true
},
score = 0,
attribute = "attack",
timeout   = 99999
}