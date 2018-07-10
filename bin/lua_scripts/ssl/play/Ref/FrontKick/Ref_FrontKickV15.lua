-- 在靠中间位置的前场定位球
-- by yys 2014-07-10

local FRONT_POS_1 = CGeoPoint:new_local(285, -175)
local FRONT_POS_2 = CGeoPoint:new_local(260, 155)

local MIDDLE_POS_1 = function ()
  return CGeoPoint:new_local(ball.posX() - 100, ball.posY() + 50)
end
local MIDDLE_POS_2 = CGeoPoint:new_local(70, -160)
local MIDDLE_POS_3 = CGeoPoint:new_local(80, 160)
local MIDDLE_POS_4 = CGeoPoint:new_local(50, 160)

local SIDE_POS_1 = CGeoPoint:new_local(250, -180)
local SIDE_POS_2 = CGeoPoint:new_local(270, 180)
local SIDE_POS_3 = CGeoPoint:new_local(190, 180)

local GOBACK_POS_1 = CGeoPoint:new_local(-100, -180)
local GOBACK_POS_2 = CGeoPoint:new_local(-80, 180)
local GOBACK_POS_3 = CGeoPoint:new_local(-110, 180)

local faceDir = 0
local reflectDir = 0.4

local reflectPos = function ()
  return CGeoPoint:new_local(ball.posX() + 20, ball.posY() + 35)
end

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 20 and
                player.toTargetDist("Special") < 20, 30, 180) then
        return "temp"
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Tier     = task.goCmuRush(MIDDLE_POS_1),
    Special  = task.goCmuRush(MIDDLE_POS_2),
    Middle   = task.goCmuRush(MIDDLE_POS_3),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{A}{SMD}"
  },

  ["temp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 10 and
                player.toTargetDist("Special") < 20, "normal", 180) then
        return "judge"
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Tier     = task.goCmuRush(reflectPos, reflectDir),
    Special  = task.goCmuRush(SIDE_POS_1),
    Middle   = task.goCmuRush(SIDE_POS_2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["judge"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Tier") < 5, 20, 180) then
        if not world:isShootLineBlocked(gRoleNum["Assister"], 130, 1.5) then
          return "directShoot"
        else
          if world:isShootLineBlocked(gRoleNum["Tier"], 100, 1.5) then
            return "PrechipDefenceBox"
          else
            return "reflect"
          end
        end
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Tier     = task.goCmuRush(reflectPos, reflectDir),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(FRONT_POS_2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["reflect"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.goAndTurnKick(pos.reflectPassPos("Tier"), 600, _, 100),
    Tier     = task.stop(),
    Special  = task.goCmuRush(GOBACK_POS_1),
    Middle   = task.goCmuRush(GOBACK_POS_2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["directShoot"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.shoot(),
    Tier     = task.defendMiddle(),
    Special  = task.sideBack(),
    Middle   = task.leftBack(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{A}[SMD]"
  },

  ["PrechipDefenceBox"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 10, 20, 180) then
        return "chipDefenceBox"
      end
    end,
    Assister = task.slowGetBall(CGeoPoint(325, 0)),
    Tier     = task.stop(),
    Special  = task.goCmuRush(FRONT_POS_1),
    Middle   = task.goCmuRush(FRONT_POS_2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ASMD}"
  },

  ["chipDefenceBox"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.chipPass(CGeoPoint:new_local(325, 0), 180),
    Tier     = task.defendMiddle(),
    Special  = task.shoot(),
    Middle   = task.rightBack(),
    Defender = task.leftBack(),
    Goalie   = task.goalie(),
    match    = "{AS}[MD]"
  },

  name = "Ref_FrontKickV15",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}