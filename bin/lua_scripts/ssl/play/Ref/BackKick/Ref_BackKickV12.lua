-- by yys 2014.06.27
-- 利用反射板

local TIER_TEMP_POS = function ()
  return (ball.refAntiYPos(CGeoPoint:new_local(ball.posX() - 20, -(math.abs(ball.posY()) - 30))))()
end
local TIER_TEMP_POS2 = function ()
  return (ball.refAntiYPos(CGeoPoint:new_local(ball.posX() + 20, -(math.abs(ball.posY()) - 40))))()
end
local FRONT_POS1 = ball.refAntiYPos(CGeoPoint:new_local(175, 20))
local FRONT_POS2 = ball.refAntiYPos(CGeoPoint:new_local(100, 70))
local FRONT_POS3 = ball.refAntiYPos(CGeoPoint:new_local(230, -100))
local MIDDLE_POS = ball.refAntiYPos(CGeoPoint:new_local(8, 140))

local FACE_DIR = ball.refAntiYDir(dir.specified(163.5)())

gPlayTable.CreatePlay{
  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 20, 60, 120) then
        return "temp"
      end
    end,
    Assister = task.staticGetBall(FACE_DIR),
    Tier     = task.goCmuRush(TIER_TEMP_POS2, player.toBallDir),
    Leader   = task.goCmuRush(FRONT_POS1, player.toBallDir),
    Middle   = task.goCmuRush(MIDDLE_POS, player.toBallDir),
    Defender = task.sideBack(),
    Goalie   = task.goalie(),
    match    = "{A}{LMD}"
  },

  ["temp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 20 and
                player.toTargetDist("Leader") < 20, "normal", 120) then
        return "reflectPos"
      end
    end,
    Assister = task.staticGetBall(FACE_DIR),
    Tier     = task.goCmuRush(TIER_TEMP_POS, player.toBallDir),
    Leader   = task.goCmuRush(FRONT_POS2, player.toBallDir),
    Middle   = task.goCmuRush(MIDDLE_POS, player.toBallDir),
    Defender = task.sideBack(),
    Goalie   = task.goalie(),
    match    = "{ALMD}"
  },

  ["reflectPos"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 20 and
                player.toTargetDist("Tier") < 5 and
                player.toTargetDist("Leader") < 20, 30, 120) then
        return "fix"
      end
    end,
    Assister = task.staticGetBall(FACE_DIR),
    Tier     = task.goCmuRush(pos.reflectPos(-30, -5), dir.reflectDir(-1.7)),
    Leader   = task.goCmuRush(FRONT_POS2, player.toBallDir),
    Middle   = task.goCmuRush(MIDDLE_POS, player.toBallDir),
    Defender = task.sideBack(),
    Goalie   = task.goalie(),
    match    = "{ALMD}"
  },

  ["fix"] = {
    switch = function ()
      if bufcnt(true, 30) then
        return "pass"
      end
    end,
    Assister = task.staticGetBall(FACE_DIR),
    Tier     = task.goCmuRush(pos.reflectPos(-30, -5), dir.reflectDir(-1.7)),
    Leader   = task.goCmuRush(FRONT_POS2, player.toBallDir),
    Middle   = task.goCmuRush(MIDDLE_POS, player.toBallDir),
    Defender = task.sideBack(),
    Goalie   = task.goalie(),
    match    = "{ALMD}"
  },

  ["pass"] = {
    switch = function ()
      if  bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 35, 2) then
        return "shoot"
      elseif bufcnt(true, 90) then
        return "exit"
      end
    end,
    Assister = task.goAndTurnKick(pos.reflectPassPos("Tier"), kp.forReflect(), _, 50),
    Tier     = task.stop(),
    Leader   = task.goCmuRush(FRONT_POS2, player.toBallDir),
    Middle   = task.leftBack(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{AL}[MD]"
  },

  ["shoot"] = {
    switch = function ()
      if player.kickBall("Leader") then
        return "finish"
      elseif bufcnt(true, 120) then
        return "exit"
      end
    end,
    Assister = task.sideBack(),
    Tier     = task.defendMiddle(),
    Leader   = task.shoot(),
    Middle   = task.leftBack(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{L}[AMD]"
  },

  name        = "Ref_BackKickV12",
  applicable  = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout   = 99999
}


