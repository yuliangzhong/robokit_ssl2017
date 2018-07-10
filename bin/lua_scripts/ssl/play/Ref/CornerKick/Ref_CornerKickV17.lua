-- by yys 2015-07-11

local FINAL_SHOOT_POS = CGeoPoint:new_local(350, 80)
local PASS_POS   = pos.passForTouch(FINAL_SHOOT_POS)

local SYNT_POS_1 = ball.refSyntYPos(CGeoPoint:new_local(290, 250))
local SYNT_POS_2 = ball.refSyntYPos(CGeoPoint:new_local(200, 250))
local SYNT_POS_3 = ball.refSyntYPos(CGeoPoint:new_local(100, 250))

local BACK_WAIT_POS = ball.refAntiYPos(CGeoPoint:new_local(20, 80))

local TMP_SHOOT_POS = {
  ball.refAntiYPos(CGeoPoint:new_local(50, -150)),
  ball.refAntiYPos(CGeoPoint:new_local(210, 20)),
  ball.refAntiYPos(CGeoPoint:new_local(300, 150)),
  ball.refAntiYPos(FINAL_SHOOT_POS)
}

local dangerous = true
local USE_CHIP  = true

gPlayTable.CreatePlay{
  firstState = "start",
  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 30 and
                player.toTargetDist("Defender") < 30, 1, 180) then
        return "runTmpPos"
      end
    end,
    Assister = task.staticGetBall(PASS_POS),
    Leader   = task.goCmuRush(SYNT_POS_1),
    Special  = task.goCmuRush(SYNT_POS_2, _, _, flag.allow_dss),
    Middle   = task.goCmuRush(SYNT_POS_3, _, _, flag.allow_dss),
    Defender = task.singleBack(),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(BACK_WAIT_POS,_,500),
    match    = "{A}{LDMS}"
  },

  ["runTmpPos"] = {
    switch = function ()
      if player.toPointDist("Defender", TMP_SHOOT_POS[2]()) < 50 then
        return "dribble"
      end
    end,
    Assister = task.staticGetBall(PASS_POS),
    Leader   = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Special  = task.goCmuRush(SYNT_POS_2, _, _, flag.allow_dss),
    Middle   = task.goCmuRush(SYNT_POS_3, _, _, flag.allow_dss),
    Defender = task.runMultiPos(TMP_SHOOT_POS, false, 40),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(BACK_WAIT_POS,_,500),
    match    = "{ALDMS}"
  },

  ["dribble"] = {
    switch = function ()
      if player.toPointDist("Defender", TMP_SHOOT_POS[3]()) < 40 then
        return "passBall"
      end
    end,
    Assister = USE_CHIP and task.slowGetBall(PASS_POS) or task.staticGetBall(PASS_POS),
    Leader   = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Special  = task.goCmuRush(SYNT_POS_2, _, _, flag.allow_dss),
    Middle   = task.goCmuRush(SYNT_POS_3, _, _, flag.allow_dss),
    Defender = task.continue(),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(CGeoPoint:new_local(-400,0), 3.14, 300, flag.allow_dss),
    match    = "{ALDMS}"
  },

  ["passBall"] = {
    switch = function ()
       if player.kickBall("Assister") or player.toBallDist("Assister") > 30 then
        return "waitBall"
      end
    end,
    Assister = USE_CHIP and task.chipPass(PASS_POS, 5555+70) or task.goAndTurnKick(PASS_POS, 450),
    Leader   = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Special  = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Middle   = task.goCmuRush(SYNT_POS_3, _, _, flag.allow_dss),
    Defender = task.continue(),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(CGeoPoint:new_local(-400,0), 3.14, 300, flag.allow_dss),
    match    = "{ALDMS}"
  },

  ["waitBall"] = {
    switch = function ()
      if bufcnt(player.toPointDist("Defender", TMP_SHOOT_POS[4]()) < 40, "fast", 60) then
        return "shootBall"
      end
    end,
    Assister = task.stop(),
    Leader   = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.continue(),
    Goalie   = dangerous and task.goalie() or task.goCmuRush(CGeoPoint:new_local(-400,0), 3.14, 200, flag.allow_dss),
    match    = "{ALD}[MS]"
  },

  ["shootBall"] = {
    switch = function ()
      if bufcnt(player.kickBall("Defender"), "fast", 100)  then
        return "exit"
      end
    end,
    Assister = task.goSupportPos("Defender"),
    Leader   = task.goCmuRush(SYNT_POS_1, _, _, flag.allow_dss),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.waitTouch(TMP_SHOOT_POS[4], 0),
    Goalie   = task.goalie(),
    match    = "{ALD}[MS]"
  },

  name = "Ref_CornerKickV17",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}