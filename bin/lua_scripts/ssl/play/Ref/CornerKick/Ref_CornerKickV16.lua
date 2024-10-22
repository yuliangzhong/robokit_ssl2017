--by Fantasy
local FINAL_SHOOT_POS = CGeoPoint:new_local(360, 90)
local PASS_POS   = pos.passForTouch(FINAL_SHOOT_POS)
local ANTI_POS_1 = ball.refAntiYPos(CGeoPoint:new_local(257, -182))
local ANTI_POS_2 = ball.refAntiYPos(CGeoPoint:new_local(210, -150))
local ANTI_POS_3 = ball.refAntiYPos(CGeoPoint:new_local(330, -170))
local SYNT_POS_1 = ball.refSyntYPos(CGeoPoint:new_local(160, 160))
local SYNT_POS_2 = ball.refSyntYPos(CGeoPoint:new_local(190, 185))
local BACK_WAIT_POS = ball.refAntiYPos(CGeoPoint:new_local(-80, -30))

local TMP_SHOOT_POS = {
  ball.refAntiYPos(CGeoPoint:new_local(186, -103)),
  ball.refAntiYPos(CGeoPoint:new_local(140, 110)),
  ball.refAntiYPos(CGeoPoint:new_local(290, 130)),
  ball.refAntiYPos(CGeoPoint:new_local(350, 110))
}

local TMP_FAKER_POS= {
  ball.refAntiYPos(CGeoPoint:new_local(275, 140)),
  ball.refAntiYPos(CGeoPoint:new_local(300, -60)),
  ball.refAntiYPos(CGeoPoint:new_local(160, -100))
}


local GOALIE_MULTI_POS = {
  ball.refAntiYPos(CGeoPoint:new_local(-80, 140)),
  ball.refAntiYPos(CGeoPoint:new_local(60, 140))
}

local dangerous = true
local USE_CHIP  = true

gPlayTable.CreatePlay{
  firstState = "start",
  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 20 and
                player.toTargetDist("Defender") < 20 and
                player.toTargetDist("Goalie") < 20, 30, 180) then
        return "runTmpPos"
      end
    end,
    Assister = task.staticGetBall(PASS_POS),
    Leader   = task.goCmuRush(TMP_FAKER_POS[1], _, 600, flag.allow_dss),
    Special  = task.goCmuRush(ANTI_POS_1, _, 600, flag.allow_dss),
    Middle   = task.goCmuRush(ANTI_POS_2, _, 600, flag.allow_dss),
    Defender = task.goCmuRush(TMP_SHOOT_POS[1]),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(ANTI_POS_1, _, 350),
    match    = "{A}{LDMS}"
  },

  ["runTmpPos"] = {
    switch = function ()
      if player.toPointDist("Defender", TMP_SHOOT_POS[3]()) < 30 then
        return "passBall"
      end
    end,
    Assister = USE_CHIP and task.slowGetBall(PASS_POS) or task.staticGetBall(PASS_POS),
    Leader   = task.goCmuRush(TMP_FAKER_POS[2], _, 600, flag.allow_dss),
    Special  = task.goCmuRush(ANTI_POS_1, _, 600, flag.allow_dss),
    Middle   = task.goCmuRush(ANTI_POS_2, _, 600, flag.allow_dss),
    Defender = task.runMultiPos(TMP_SHOOT_POS, false, 40),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(ANTI_POS_1, _, 350),
    match    = "{ALDMS}"
  },

  ["passBall"] = {
    switch = function ()
       if player.kickBall("Assister") or player.toBallDist("Assister") > 30 then
        return "waitBall"
      end
    end,
    Assister = USE_CHIP and task.chipPass(PASS_POS, 5555+40) or task.goAndTurnKick(PASS_POS, 450),
    Leader   = task.goCmuRush(TMP_FAKER_POS[3], _, 600, flag.allow_dss),
    Special  = task.goCmuRush(ANTI_POS_3, _, 600, flag.allow_dss),
    Middle   = task.goCmuRush(ANTI_POS_2, _, 600, flag.allow_dss),
    Defender = task.continue(),
    Goalie   = task.goalie(),
    match    = "{ALDMS}"
  },

  ["waitBall"] = {
    switch = function ()
      if bufcnt(player.toPointDist("Defender", TMP_SHOOT_POS[4]()) < 20, "fast", 100) then
        return "shootBall"
      end
    end,
    Assister = task.stop(),
    Leader   = task.rightBack(),
    Special  = task.goCmuRush(ANTI_POS_1, _, 600, flag.allow_dss),
    Middle   = task.leftBack(),
    Defender = task.continue(),
    Goalie   = task.goalie(),
    match    = "{ALD}[MS]"
  },

  ["shootBall"] = {
    switch = function ()
      if bufcnt(player.kickBall("Defender"), "fast", 100)  then
        return "exit"
      end
    end,
    Assister = task.goSupportPos("Defender"),
    Leader   = task.goCmuRush(SYNT_POS_2, _, 600, flag.allow_dss),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.waitTouch(TMP_SHOOT_POS[4], 0),
    Goalie   = task.goalie(),
    match    = "{ALD}[MS]"
  },

  name = "Ref_CornerKickV16",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}