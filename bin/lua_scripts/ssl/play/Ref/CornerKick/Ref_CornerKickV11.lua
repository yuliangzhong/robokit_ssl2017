-- 模拟2013年CMU进MRL的定位球，注意只能针对角球区域（>280）
-- by zhyaic 2014-07-14
-- 2014-07-20 yys 改

local FINAL_SHOOT_POS = CGeoPoint:new_local(250, 273)
local SHOOT_POS = ball.refSyntYPos(FINAL_SHOOT_POS)
-- pos for leader
local LEADER_POS1 = ball.refSyntYPos(CGeoPoint:new_local(-50, 290))
local LEADER_POS2 = ball.refSyntYPos(FINAL_SHOOT_POS)

-- pos for middle
local MIDDLE_POS1 = ball.refAntiYPos(CGeoPoint:new_local(350, 260))

-- pos for special
local SPECIAL_POS1 = ball.refAntiYPos(CGeoPoint:new_local(57, 200))

-- pos for defender
-- change for cmu
-- local DEFENDER_POS1 = ball.refSyntYPos(CGeoPoint:new_local(225,217))
local DEFENDER_POS1 = ball.refAntiYPos(CGeoPoint:new_local(240, 50))
local USE_FAKE_DIR = true

-- 这里比较特殊
local PASS_POS = CGeoPoint:new_local(250, -273)
local TMP_FACE_POS = USE_FAKE_DIR and CGeoPoint:new_local(300, 0) or CGeoPoint:new_local(0, 200)
local FACE_POS = USE_FAKE_DIR and CGeoPoint:new_local(300, 0) or PASS_POS--pos.passForTouch(CGeoPoint:new_local(150,-170))


-- 让挑球的速度更快一点
gPlayTable.CreatePlay{

  firstState = "start",

  ["start"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Leader") < 30 and 
                player.toTargetDist("Special") < 30, 30) then
        return "tmp"
      end
    end,
    Assister = task.staticGetBall(TMP_FACE_POS),
    Leader   = task.goCmuRush(LEADER_POS1),
    Special  = task.goCmuRush(SPECIAL_POS1),
    Middle   = task.goCmuRush(MIDDLE_POS1),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{A}[LSMD]"
  },

  ["tmp"] = {
    switch = function ()
      if bufcnt(player.toTargetDist("Assister") < 10, 40, 180) then
        return "rush"
      end
    end,
    Assister = task.staticGetBall(FACE_POS),
    Leader   = task.goCmuRush(LEADER_POS1),
    Special  = task.goCmuRush(SPECIAL_POS1),
    Middle   = task.goCmuRush(MIDDLE_POS1),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["rush"] = {
    switch = function ()
      local tmpDist = USE_FAKE_DIR and 250 or 150
      if bufcnt(player.toTargetDist("Leader") < tmpDist, "fast", 180) then
        return "pass"
      end
    end,
    Assister = task.staticGetBall(FACE_POS),
    Leader   = task.goCmuRush(SHOOT_POS),
    Special  = task.goCmuRush(SPECIAL_POS1),
    Middle   = task.goCmuRush(MIDDLE_POS1),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{ALSMD}"
  },

  ["pass"] = {
    switch = function ()
      if bufcnt(player.kickBall("Assister") or ball.velMod() > 50, 1, 180) then
        return "fixgoto"
      end
    end,
    Assister = task.goAndTurnKickByGetBall(PASS_POS, 300),
    Leader   = task.goCmuRush(SHOOT_POS, _, 300),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{ALD}[SM]"
  },

  ["fixgoto"] = {
    switch = function ()
      if bufcnt(true, 10) then
        return "shoot"
      end
    end,
    Assister = task.defendMiddle(),
    Leader   = task.goCmuRush(SHOOT_POS, _, 300),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{ALD}[SM]"
  },

  ["shoot"] = {
    switch = function ()
      if bufcnt(player.kickBall("Leader"), 1, 90) then
        return "exit"
      end
    end,
    Assister = task.stop(),
    Leader   = task.touch(),
    Special  = task.leftBack(),
    Middle   = task.rightBack(),
    Defender = task.goCmuRush(DEFENDER_POS1),
    Goalie   = task.goalie(),
    match    = "{ALD}[SM]"
  },

  name = "Ref_CornerKickV11",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}