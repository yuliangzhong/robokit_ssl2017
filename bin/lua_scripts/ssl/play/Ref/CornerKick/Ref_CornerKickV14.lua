-- 挡板角球, 需要在比较靠后的位置使用
-- by yys 2014-07-09
-- 2014-07-20 yys 改

local FRONT_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(285, 175))
local FRONT_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(260, 155))
local FRONT_POS_3 = ball.refAntiYPos(CGeoPoint:new_local(290, 100))

local GOALIE_POS  = ball.refAntiYPos(CGeoPoint:new_local(0, 0))
local PASS_POS_FOR_GOALIE = pos.passForTouch(GOALIE_POS)

local MIDDLE_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(280, 0))
local MIDDLE_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(200, 150))
local MIDDLE_POS_3 = ball.refAntiYPos(CGeoPoint:new_local(150, 100))
local MIDDLE_POS_4 = ball.refAntiYPos(CGeoPoint:new_local(20, 50))

local SIDE_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(320, 150))
local SIDE_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(270, 150))
local SIDE_POS_3 = ball.refAntiYPos(CGeoPoint:new_local(150, -150))

local PRE_CHIP_POS = function ()
  return (ball.refAntiYPos(CGeoPoint:new_local(ball.posX() + 10, -(math.abs(ball.posY()) + 15))))()
end

local reflectDir = 0.95

local dangerous = true

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 30 and
                player.toTargetDist("Special") < 30, 10, 180) then
        return "temp"
      end
    end,
    Assister = task.staticGetBall(PASS_POS_FOR_GOALIE),
    Tier     = task.goCmuRush(MIDDLE_POS_1),
    Special  = task.goCmuRush(MIDDLE_POS_2),
    Middle   = task.goCmuRush(MIDDLE_POS_3),
    Defender = task.goCmuRush(SIDE_POS_3),
    Goalie   = task.goalie(),
    match    = "{A}{SMD}"
  },

  ["temp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 20 and
                player.toTargetDist("Special") < 20, "normal", 180) then
        return "prepare"
      end
    end,
    Assister = task.staticGetBall(PASS_POS_FOR_GOALIE),
    Tier     = task.goCmuRush(pos.reflectPos(-45, 0), dir.reflectDir(reflectDir)),
    Special  = task.goCmuRush(SIDE_POS_1),
    Middle   = task.goCmuRush(SIDE_POS_2),
    Defender = task.goCmuRush(FRONT_POS_3),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(GOALIE_POS),
    match    = "{ASMD}"
  },

  ["prepare"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 5, 20, 180) then
        if (not dangerous) and (not world:isPassLineBlocked(gRoleNum["Goalie"])) and (not world:isShootLineBlocked(gRoleNum["Goalie"], 200)) then
          return "reflectreflect"
        elseif world:isShootLineBlocked(gRoleNum["Tier"], 100, 1.5) then
          return "reflect"
        else
          return "reflect"
        end
      end
    end,
    Assister = task.staticGetBall(pos.reflectPassPos("Tier")),
    Tier     = task.goCmuRush(pos.reflectPos(-45, 0), dir.reflectDir(reflectDir)),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(FRONT_POS_2),
    Defender = task.goCmuRush(FRONT_POS_3),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(GOALIE_POS),
    match    = "{ASMD}"
  },

  ["reflect"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.goAndTurnKick(pos.reflectPassPos("Tier"), 800),
    Tier     = task.stop(),
    Special  = task.goSupportPos("Tier"),
    Middle   = task.rightBack(),
    Defender = task.leftBack(),
    Goalie   = task.goalie(),
    match    = "{AS}[MD]"
  },

  ["PrechipDefenceBox"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 10, 20, 180) then
        return "PrechipDefenceBox2"
      end
    end,
    Assister = task.goCmuRush(PRE_CHIP_POS),
    Tier     = task.stop(),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(FRONT_POS_2),
    Defender = task.goCmuRush(FRONT_POS_3),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["PrechipDefenceBox2"] = {
    switch = function ()
      if bufcnt(true, 30) then
        return "chipDefenceBox"
      end
    end,
    Assister = task.slowGetBall(CGeoPoint:new_local(280, 0)),
    Tier     = task.stop(),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(FRONT_POS_2),
    Defender = task.goCmuRush(FRONT_POS_3),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["chipDefenceBox"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.goAndTurnChip(CGeoPoint:new_local(325, 0), 400),
    Tier     = task.stop(),
    Special  = task.defendMiddle(),
    Middle   = task.rightBack(),
    Defender = task.leftBack(),
    Goalie   = task.goalie(),
    match    = "{AS}[MD]"
  },

  ["passToGoalie"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 30 then
        return "goalieTouch"
      end
    end,
    Assister = task.goAndTurnKick(PASS_POS_FOR_GOALIE, 500),
    Tier     = task.stop(),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-100, 190))),
    Defender = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-80, 170))),
    Goalie   = task.goCmuRush(GOALIE_POS),
    match    = "{AS}[MD]"
  },

  ["goalieTouch"] = {
    switch = function ()
      if player.kickBall("Goalie") then
        return "goalieReturn"
      elseif bufcnt(true, 75) then
        return "goalieReturn"
      end
    end,
    Assister = task.stop(),
    Tier     = task.goCmuRush(ball.refAntiYPos(CGeoPoint(-120, -180))),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-150, 190))),
    Defender = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-130, 170))),
    Goalie   = task.touch(),
    match    = "{AS}[MD]"
  },

  ["goalieReturn"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Goalie") < 20, "fast", 80) then
        return "exit"
      end
    end,
    Assister = task.advance(),
    Tier     = task.defendMiddle(),
    Special  = task.sideBack(),
    Middle   = task.rightBack(),
    Defender = task.leftBack(),
    Goalie   = task.goalie(),
    match    = "{A}[SMD]"
  },

  name = "Ref_CornerKickV14",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}