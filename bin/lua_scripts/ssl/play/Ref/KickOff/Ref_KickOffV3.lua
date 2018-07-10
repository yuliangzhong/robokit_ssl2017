-- 两传一射开球开球
-- by yys 2014-07-11

local RECEIVER_POS = CGeoPoint:new_local(-8, -140)
local RECEIVER_POS_2 = CGeoPoint:new_local(5,-140)
local TOUCH_POS = CGeoPoint:new_local(245, 190)
local MIDDLE_POS = CGeoPoint:new_local(-10, 70)
local PASS_POS = pos.passForTouch(TOUCH_POS)

gPlayTable.CreatePlay{
firstState = "start",

["start"] = {
  switch = function ()
    if cond.isNormalStart() then
      return "temp"
    end
  end,
  Kicker   = task.staticGetBall(CGeoPoint:new_local(0, -180), false),
  Assister = task.goCmuRush(CGeoPoint:new_local(-15, 180)),
  Special = task.goCmuRush(CGeoPoint:new_local(-80, -50), player.backBallDir),
  Middle   = task.goCmuRush(CGeoPoint:new_local(-170, 180), _, 200),
  Defender = task.goCmuRush(CGeoPoint:new_local(-15, -180)),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

["temp"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Special") < 20, 20, 180) then
      return "ready"
    end
  end,
  Kicker   = task.staticGetBall(RECEIVER_POS_2, false),
  Assister = task.goCmuRush(MIDDLE_POS),
  Special = task.goCmuRush(RECEIVER_POS, player.toBallDir),
  Middle   = task.goCmuRush(CGeoPoint:new_local(-15, 180)),
  Defender = task.goCmuRush(CGeoPoint:new_local(-7, -180)),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

["ready"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Special") < 5, 20, 120) then
      return "pass"
    end
  end,
  Kicker   = task.staticGetBall(RECEIVER_POS_2, false),
  Assister = task.goCmuRush(MIDDLE_POS),
  Special = task.goCmuRush(RECEIVER_POS, player.toBallDir),
  Middle   = task.goCmuRush(CGeoPoint:new_local(-7, 180)),
  Defender = task.goCmuRush(CGeoPoint:new_local(-3, -200)),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

["pass"] = {
  switch = function ()
    if player.kickBall("Kicker") or player.toBallDist("Kicker") > 25 then
      return "SecondPass"
    end
  end,
  Kicker   = task.goAndTurnKickWithoutAnti(RECEIVER_POS_2, 5555+20),
  Assister = task.goCmuRush(MIDDLE_POS),
  Special = task.goCmuRush(RECEIVER_POS_2, player.toBallDir),
  Middle   = task.goCmuRush(TOUCH_POS),
  Defender = task.goCmuRush(CGeoPoint:new_local(50, -170), player.toBallDir),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

["SecondPass"] = {
  switch = function ()
    if bufcnt(player.kickBall("Special") or player.isBallPassed("Special", "Middle"), "fast") then
      return "shoot"
    end
    if  bufcnt(true, 100) then
      return "exit"
    end
  end,
  Kicker   = task.leftBack(),
  Assister = task.rightBack(),
  Special  = task.receivePass(PASS_POS, 5555+20),
  Middle   = task.goCmuRush(TOUCH_POS),
  Defender = task.goCmuRush(CGeoPoint:new_local(200, -120), player.toBallDir),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

["shoot"] = {
  switch = function ()
    if bufcnt(player.kickBall("Middle"), "fast", 100) then
      return "exit"
    end
  end,
  Kicker   = task.leftBack(),
  Assister = task.rightBack(),
  Special = task.defendMiddle(),
  Middle   = task.waitTouch(TOUCH_POS,0),
  Defender = task.goCmuRush(CGeoPoint:new_local(130, -120)),
  Goalie   = task.goalie(),
  match    = "{SAMD}"
},

name = "Ref_KickOffV3",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}