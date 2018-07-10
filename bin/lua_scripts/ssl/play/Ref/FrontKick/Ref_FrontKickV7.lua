-- by zhyaic 2014-06-26
-- 利用反向挑球进行射门，传球力度和射门力度是要考虑的

local FINAL_SHOOT_POS  = CGeoPoint:new_local(CGeoPoint:new_local(273,20))
local TMP_RECEIVER_POS = ball.refAntiYPos(CGeoPoint:new_local(-50,0))
local SHOOT_POS_1 = ball.refAntiYPos(FINAL_SHOOT_POS)
local BOUND_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(233,42))
local BOUND_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(173,20))
local SIDE_POS  = ball.refAntiYPos(CGeoPoint:new_local(40,170))
local JAM_POS = ball.jamPos(CGeoPoint:new_local(0,200),55,-15)

local BOUND_POS = function (r)
  return function ()
    return player.pos(r) + Utils.Polar2Vector(24,3.14)
  end
end

local USE_CHIP = true

gPlayTable.CreatePlay{

  firstState = "tmp",

  ["tmp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 20, 20, 180) then
          return "start"
      end
    end,
    Assister = USE_CHIP and task.staticGetBall(FINAL_SHOOT_POS) or task.staticGetBall(CGeoPoint:new_local(0,200)),
    Middle   = task.goCmuRush(BOUND_POS_1),
    Leader   = task.goCmuRush(TMP_RECEIVER_POS),
    Special  = task.goCmuRush(SIDE_POS),
    Defender = task.goCmuRush(JAM_POS),
    Goalie   = task.goalie(),
    match    = "{A}[LMDS]"
  },

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 20 and
          player.toTargetDist("Middle") < 20, 20, 180) then
          return "pass"
      end
    end,
    Assister = USE_CHIP and task.slowGetBall(FINAL_SHOOT_POS) or task.staticGetBall(CGeoPoint:new_local(0,200)),
    Middle   = task.goCmuRush(BOUND_POS_1),
    Leader   = task.goCmuRush(SHOOT_POS_1,dir.playerToBall),
    Special  = task.goCmuRush(SIDE_POS),
    Defender = task.goCmuRush(JAM_POS),
    Goalie   = task.goalie(),
    match    = "{A}[LMDS]"
  },

  ["pass"] = {
    switch = function ()
      if player.kickBall("Assister") or player.isBallPassed("Assister","Leader") then
        if USE_CHIP then
          return "fix"
        else
          return "touch"
        end
      elseif bufcnt(true, 120) then
        return "exit"
      end
    end,
    Assister = USE_CHIP and task.chipPass(FINAL_SHOOT_POS, 180) or task.goAndTurnKick(FINAL_SHOOT_POS),
    Middle   = task.goCmuRush(BOUND_POS_1),
    Leader   = task.goCmuRush(SHOOT_POS_1,dir.playerToBall),
    Special  = task.goCmuRush(SIDE_POS),
    Defender = task.goCmuRush(JAM_POS),
    Goalie   = task.goalie(),
    match    = "{ALMDS}"
  },

  ["fix"] = {
    switch = function ()
      if bufcnt(true, 30)then
        return "touch"
      end
    end,
    Leader   = task.goCmuRush(SHOOT_POS_1,dir.playerToBall),
    Middle   = task.goCmuRush(BOUND_POS("Leader"),dir.specified(0)),
    Assister = task.leftBack(),
    Special  = task.rightBack(),
    Defender = task.defendMiddle(),
    Goalie   = task.goalie(),
    match    = "{L}{M}[DAS]"
  },

  ["touch"] = {
    switch = function ()
      if bufcnt(player.kickBall("Leader"), 1, 120)then
        return "stop"
      end
    end,
    Leader   = task.receiveChip(dir.specified(180),cp.specified(9999)),
    Middle   = task.goCmuRush(BOUND_POS("Leader"),dir.specified(0)),
    Assister = task.leftBack(),
    Special  = task.rightBack(),
    Defender = task.defendMiddle(),
    Goalie   = task.goalie(),
    match    = "{L}{M}[DAS]"
  },

  ["stop"] = {
    switch = function ()
      if  bufcnt(true, 20) then
        return "exit"
      end
    end,
    Leader   = task.stop(),
    Middle   = task.goCmuRush(BOUND_POS("Leader"),dir.specified(0)),
    Assister = task.leftBack(),
    Special  = task.rightBack(),
    Defender = task.defendMiddle(),
    Goalie   = task.goalie(),
    match    = "{L}{M}[DAS]"
  },

  name = "Ref_FrontKickV7",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}