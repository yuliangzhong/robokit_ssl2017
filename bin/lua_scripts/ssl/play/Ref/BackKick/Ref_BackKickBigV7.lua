-- by zhyaic 2013.6.29
-- 在对阵屎队时，通过LOG分析其不防开球车，因此可以自动判断选择直接射门骗定位球,大场地版

local AtrPos1 = ball.refSyntYPos(CGeoPoint:new_local(175,75))    --拉人位置一
local AtrPos2 = ball.refSyntYPos(CGeoPoint:new_local(185,105))   --拉人位置二
local TMP_BACK_POS = ball.refAntiYPos(CGeoPoint:new_local(-130,50))
local TMP_DEF_POS1 = ball.refAntiYPos(CGeoPoint:new_local(60,120))
local TMP_DEF_POS2 = ball.refAntiYPos(CGeoPoint:new_local(60,40))

gPlayTable.CreatePlay{
  firstState = "start",

  ["start"] = {
    switch = function ()
      if  bufcnt(player.toTargetDist("Middle") < 50 and
          player.toTargetDist("Leader") < 50 and
          player.toTargetDist("Special") < 50, "normal") then
        return "chooseStrategy"
      end
    end,
    Assister = task.staticGetBall(CGeoPoint:new_local(404.5,-30)),
    Leader   = task.goCmuRush(AtrPos1),
    Special  = task.goCmuRush(AtrPos2),
    Middle   = task.goCmuRush(TMP_BACK_POS),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{A}[LSMD]"
  },

  ["chooseStrategy"] = {
    switch = function ()
      if  bufcnt(player.toTargetDist("Leader") < 20 and
          player.toTargetDist("Special") < 20, "fast") then
        return "kick"
      end
    end,
    Assister = task.staticGetBall(CGeoPoint:new_local(404.5,-30)),
    Leader   = task.goCmuRush(TMP_DEF_POS1),
    Special  = task.goCmuRush(TMP_DEF_POS2),
    Middle   = task.goCmuRush(TMP_BACK_POS),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{AD}[LSM]"
  },

  ["kick"] = {
    switch = function ()
      if  bufcnt(player.kickBall("Middle"), "fast",120) then
        return "exit"
      end
    end,
    Assister = task.chaseFar(),
    Leader   = task.goPassPos("Assister"),
    Special  = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(-170,0))),
    Middle   = task.leftBack(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{A}[MDSL]"
  },

  name = "Ref_BackKickBigV7",
  applicable = {
    exp = "a",
    a   = true
  },
  attribute = "attack",
  timeout = 99999
}