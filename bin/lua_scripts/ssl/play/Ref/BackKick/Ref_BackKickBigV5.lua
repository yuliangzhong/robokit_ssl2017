-- 开球车传球后上前射门，大场地版(不会挑过中线)
-- by yys 2014-03-17

MakeBallPos       = ball.refSyntYPos(CGeoPoint:new_local(0, 0))
AtrPos1           = ball.refAntiYPos(CGeoPoint(200, 230))
AtrPos2           = ball.refAntiYPos(CGeoPoint:new_local(260, 230))
ShootBallPos      = ball.refSyntYPos(CGeoPoint:new_local(-120, 230))
ShootBallPosFront = ball.refSyntYPos(CGeoPoint:new_local(-100, 230))

local ChipDir = function (p)  --第一次挑传传球方向
  return function (  )
    return (ball.refSyntYPos(p)() - ball.pos()):dir()
  end
end

gPlayTable.CreatePlay{

  firstState = "Start",

  ["Start"] = {
    switch = function()
      if  bufcnt(player.toTargetDist("Leader") < 30 and player.InfoControlled("Assister")
        and player.toTargetDist("Special") < 30, 40, 120) then
        return "FirstPass"
      end
    end,
    Leader   = task.goCmuRush(MakeBallPos, player.toPointDir(ShootBallPosFront)),
    Assister = task.slowGetBall(CGeoPoint:new_local(80, 0)),
    Special  = task.goCmuRush(AtrPos1),
    Middle   = task.goCmuRush(AtrPos2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "[ALMDS]"
  },

  ["FirstPass"] = {
    switch = function()
      if  bufcnt(player.kickBall("Assister") or player.toBallDist("Assister") > 20, "normal") then
        return "FixGo"
      elseif  bufcnt(true, 120) then
        return "exit"
      end
    end,
    Leader   = task.goCmuRush(MakeBallPos, player.toPointDir(ShootBallPosFront)),
    Assister = task.chipPass(ChipDir(CGeoPoint:new_local(0, 0)), 180),
    Special  = task.goCmuRush(AtrPos1),
    Middle   = task.goCmuRush(AtrPos2),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALMDS}"
  },

  ["FixGo"] = {
    switch = function()
      if  bufcnt(true, 30) then
        return "SecondPass"
      end
    end,
    Leader   = task.goCmuRush(MakeBallPos, player.toPointDir(ShootBallPosFront)),
    Assister = task.goCmuRush(ShootBallPos),
    Special  = task.leftBack(),
    Middle   = task.defendMiddle(),
    Defender = task.singleBack(),
    Goalie   = task.goalie(),
    match    = "{ALMDS}"
  },

  ["SecondPass"] = {
    switch = function()
      if   bufcnt(player.kickBall("Leader") or player.isBallPassed("Leader", "Assister"), "normal", 120) then
        return "Kick"
      elseif  bufcnt(true, 180) then
        return "exit"
      end
    end,
    Leader   = task.touchPass(ShootBallPosFront),
    Assister = task.goCmuRush(ShootBallPos),
    Special  = task.leftBack(),
    Middle   = task.defendMiddle(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{ALMDS}"
  },

  ["Kick"] = {
    switch = function()
      if  bufcnt(player.kickBall("Assister"), "fast", 180) then
        return "exit"
      end
    end,
    Leader   = task.leftBack(),
    Assister = task.shoot(),
    Special  = task.advance(),
    Middle   = task.defendMiddle(),
    Defender = task.rightBack(),
    Goalie   = task.goalie(),
    match    = "{ALMDS}"
  },

  name      = "Ref_BackKickBigV5",
  applicable  ={
    exp = "a",
    a   = true
  },
  attribute   = "attack",
  timeout   = 99999
}
