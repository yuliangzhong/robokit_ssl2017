-- 在靠中间位置的前场定位球
-- by yys 2014-07-10

local FRONT_POS_1 = CGeoPoint:new_local(325, -175)
local FRONT_POS_2 = CGeoPoint:new_local(300, 155)

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

local faceDir = CGeoPoint:new_local(450, 0)
local reflectDir = 0.4

local reflectPos = function ()
  return CGeoPoint:new_local(ball.posX() + 20, ball.posY() + 35)
end

gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 20 and
                player.toTargetDist("Special") < 20, 30, 180) then
        return "temp"
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Leader   = task.goCmuRush(MIDDLE_POS_1, _, 500, flag.allow_dss),
    Special  = task.goCmuRush(MIDDLE_POS_2, _, 500, flag.allow_dss),
    Middle   = task.goCmuRush(MIDDLE_POS_3, _, 500, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{A}{LSMD}"
  },

  ["temp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 10 and
                player.toTargetDist("Special") < 20, "normal", 180) then
        return "judge"
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Leader     = task.goCmuRush(reflectPos, reflectDir, 500, flag.allow_dss),
    Special  = task.goCmuRush(SIDE_POS_1, _, 500, flag.allow_dss),
    Middle   = task.goCmuRush(SIDE_POS_2, _, 500, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["judge"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 5, 20, 180) then
        if false then
          return "directShoot"
        else
          if world:isShootLineBlocked(gRoleNum["Leader"], 100, 1.5) then
            return "PrechipDefenceBox"
          else
            return "PrechipDefenceBox"
            --return "reflect"
          end
        end
      end
    end,
    Assister = task.staticGetBall(faceDir),
    Leader   = task.goCmuRush(reflectPos, reflectDir, 500, flag.allow_dss),
    Special  = task.goCmuRush(FRONT_POS_1, _, 500, flag.allow_dss),
    Middle   = task.goCmuRush(FRONT_POS_2, _, 500, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["reflect"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.goAndTurnKick(pos.reflectPassPos("Leader"), 600, _, 100),
    Leader     = task.stop(),
    Special  = task.goCmuRush(GOBACK_POS_1, _, 500, flag.allow_dss),
    Middle   = task.goCmuRush(GOBACK_POS_2, _, 500, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["directShoot"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.shoot(),
    Leader     = task.defendMiddle(),
    Special  = task.sideBack(),
    Middle   = task.leftBack(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{A}[LSMD]"
  },

  ["PrechipDefenceBox"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 10, 60, 180) then
        return "chipDefenceBox"
      end
    end,
    Assister = task.slowGetBall(CGeoPoint(450, 0)),
    Leader     = task.stop(),
    Special  = task.goCmuRush(FRONT_POS_1, _, 500, flag.allow_dss),
    Middle   = task.goCmuRush(FRONT_POS_2, _, 500, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["chipDefenceBox"] = {
    switch = function ()
      if player.kickBall("Assister") or player.toBallDist("Assister") > 35 then
        return "exit"
      end
    end,
    Assister = task.chipPass(CGeoPoint:new_local(450, 0), 5555+70),
    Leader     = task.defendMiddle(),
    Special  = task.shoot(),
    Middle   = task.rightBack(),
    Defender = task.leftBack(),
    Goalie   = task.goalie(),
    match    = "{AS}[LMD]"
  },

  name = "Ref_FrontKickV8",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}