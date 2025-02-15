-- 后卫冲进禁区射门
-- by yys 2015.03.17
local ball_Change_Pos = function ()
  local tmpX
  local tmpY

  local UpdatePos = function ()
    tmpX = ball.posX() - 60
    tmpY = ball.posY() - 30
    if tmpX > 280 then
      tmpX = 280
    end
    if tmpY > 0 then
      tmpY = ball.posY() - 30
    else
      tmpY = ball.posY() + 30
    end
    if tmpY > 230 then
      tmpY = 230
    end
    if tmpY < -230 then
      tmpY = -230
    end
  end

  local pos_BallChange = function ()
    UpdatePos()
    return CGeoPoint:new_local(tmpX, tmpY)
  end

  return pos_BallChange
end

local BALL_CHANGE_POS = ball_Change_Pos()

local FINAL_SHOOT_POS = CGeoPoint:new_local(352, 135)
local ANTIPOS_1 = ball.refAntiYPos(CGeoPoint:new_local(0, 230))
local ANTIPOS_2 = ball.refAntiYPos(CGeoPoint:new_local(260, 170))
local ANTIPOS_3 = ball.refAntiYPos(FINAL_SHOOT_POS)
local CHIP_POS  = pos.passForTouch(FINAL_SHOOT_POS)

gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Special") < 60 and
              player.toTargetDist("Leader") < 60, "normal", 180) then
      return "ready"
    end
  end,
  Assister = task.staticGetBall(CHIP_POS),
  Special  = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(280, 180))),
  Leader   = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(270, 50))),
  Middle   = task.stop(),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{A}{DLSM}"
},

["ready"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Special") < 60 and
              player.toTargetDist("Leader") < 60, "normal", 180) then
      return "goalone"
    end
  end,
  Assister = task.staticGetBall(CHIP_POS),
  Special  = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(250, 220))),
  Leader   = task.goCmuRush(BALL_CHANGE_POS),
  Middle   = task.goCmuRush(CGeoPoint:new_local(30,0)),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{A}{DLSM}"
},

["goalone"] = {
  switch = function ()
    if bufcnt(player.toTargetDist("Defender") < 60, "fast", 180) then
      return "goget"
    end
  end,
  Assister = task.staticGetBall(CHIP_POS),
  Defender = task.goCmuRush(ANTIPOS_1),
  Special  = task.goCmuRush(BALL_CHANGE_POS),
  Middle   = task.goCmuRush(CGeoPoint:new_local(20, 0)),
  Leader   = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{DALSM}"
},

["goget"] = {
    switch = function ()
    if bufcnt(player.toTargetDist("Defender") < 30, "normal", 180) then
      return "gopass"
    end
  end,
  Assister = task.staticGetBall(CHIP_POS),
  Defender = task.goCmuRush(ANTIPOS_2),
  Special  = task.goCmuRush(BALL_CHANGE_POS),
  Middle   = task.goCmuRush(ball.refAntiYPos(CGeoPoint:new_local(330, -180))),
  Leader   = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{DALSM}"
},

["gopass"] = {
    switch = function ()
    if bufcnt(player.kickBall("Assister") or
              player.toBallDist("Assister") > 20, "normal", 180) then
      return "goopen"
    elseif bufcnt(true, 120) then
      return "exit"
    end
  end,
  Assister = task.chipPass(CHIP_POS, 300),
  Defender = task.goCmuRush(ANTIPOS_3),
  Special  = task.goCmuRush(BALL_CHANGE_POS),
  Middle   = task.rightBack(),
  Leader   = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{DALSM}"
},

["goopen"] = {
    switch = function ()
    if bufcnt(player.toTargetDist("Defender") < 25, "fast", 180) then
      return "gokick"
    end
  end,
  Assister = task.goSupportPos("Defender"),
  Defender = task.goCmuRush(ANTIPOS_3),
  Special  = task.defendMiddle(),
  Middle   = task.rightBack(),
  Leader   = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{ADS}[LM]"
},

["gokick"] = {
    switch = function ()
    if player.kickBall("Defender") then
      return "finish"
    elseif bufcnt(true, 100) then
      return "exit"
    end
  end,
  Assister = task.goSupportPos("Defender"),
  Defender = task.waitTouch(ANTIPOS_3, 0),
  Special  = task.defendMiddle(),
  Middle   = task.rightBack(),
  Leader   = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{ADS}[SLM]"
},

name = "Ref_CornerKickV1",
applicable = {
  exp = "a",
  a = true
},
score = 0,
attribute = "attack",
timeout = 99999
}