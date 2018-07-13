-- by zhyaic 2014.07.22
-- 针对于immortal后场防开球车的特点所做修改
-- 如果想晃开immortal的防开球车，必须在晃人的方向上放一个我方车


-- 小场跑位点：
-- local FAKE_RECEIVE_POS = CGeoPoint:new_local(-100,200)
-- local LEADER_POS = ball.refAntiYPos(CGeoPoint:new_local(250,170))
-- local SPECIAL_POS = ball.refAntiYPos(FAKE_RECEIVE_POS)
-- local MIDDLE_POS_1 = ball.refSyntYPos(CGeoPoint:new_local(-310,150))
-- local MIDDLE_POS_2 = ball.refSyntYPos(CGeoPoint:new_local(-310,190))
-- local ASSISTER_POS = ball.refSyntYPos(CGeoPoint:new_local(250,200))
-- 大场：
-- 2017.7
local FAKE_RECEIVE_POS = CGeoPoint:new_local(-100,250)
local LEADER_POS = ball.refAntiYPos(CGeoPoint:new_local(250,250))
local SPECIAL_POS = ball.refAntiYPos(FAKE_RECEIVE_POS)
local MIDDLE_POS_1 = ball.refSyntYPos(CGeoPoint:new_local(-400,200))
local MIDDLE_POS_2 = ball.refSyntYPos(CGeoPoint:new_local(-400,250))
local ASSISTER_POS = ball.refSyntYPos(CGeoPoint:new_local(250,250))

gPlayTable.CreatePlay{

firstState = "startBall",

["startBall"] = {
  switch = function ()
  print("startBall")
    if bufcnt(player.toTargetDist("Leader") < 30 and
          player.toTargetDist("Special") < 30 and
          player.toTargetDist("Middle") < 30, "normal", 180) then
      return "readyRush"
    end
  end,
  Assister = task.staticGetBall(CGeoPoint:new_local(-100,200),_,flag.slowly),
  Leader   = task.goCmuRush(LEADER_POS),
  Special  = task.goCmuRush(SPECIAL_POS,player.toBallDir),
  Middle   = task.goCmuRush(MIDDLE_POS_1),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{A}{MLSD}"
},

["readyRush"] = {
  switch = function ()
  print("readyRush")
    if bufcnt(true, 60) then
      return "waitRobot"
    end
  end,
  Assister = task.staticGetBall(CGeoPoint:new_local(-100,200),_,flag.slowly),
  Leader   = task.goCmuRush(LEADER_POS),
  Special  = task.goCmuRush(SPECIAL_POS,player.toBallDir),
  Middle   = task.goCmuRush(MIDDLE_POS_2),
  Defender = task.singleBack(),
  Goalie   = task.goalie(),
  match    = "{A}{MLSD}"
},

["waitRobot"] = {
  switch = function ()
  print("waitRobot")
    if bufcnt(player.toTargetDist("Middle") < 40, "fast", 90) then
      return "rushBall"
    end
  end,
  Assister = task.goBackBall(FAKE_RECEIVE_POS,22), 
  Leader   = task.goCmuRush(LEADER_POS),
  Middle   = {GoCmuRush{acc = 300, pos = ball.backPos(CGeoPoint:new_local(325,0), 10, 0), dir = ball.backDir(p)}},
  Special  = task.rightBack(),
  Defender = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{AML}[SD]"
},

["rushBall"] = {
  switch = function ()
  print("rushBall")
    if bufcnt(player.kickBall("Middle"), "fast", 90) then
      return "exit"
    end
  end,
  Middle   = task.chaseNear(),
  Leader   = task.goCmuRush(LEADER_POS),
  Assister = task.goSimplePos(ASSISTER_POS,-3.14,flag.slowly),
  Special  = task.rightBack(),
  Defender = task.leftBack(),
  Goalie   = task.goalie(),
  match    = "{AML}[SD]"
},  

name = "Ref_BackKickV13",
applicable ={
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}
