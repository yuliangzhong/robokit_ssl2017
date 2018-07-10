-- stay at goal line, wait for enemy to lose control of the ball

local DIST_THRESH = 20
local ENEMY_ANGLE_THRESH = 0.2

local _timer
local buffer = 30


local enemyHasBall = function()
  local him = enemy.best()
  if enemy.bestToBallDist() < DIST_THRESH then
    if math.abs(enemy.bestToBallDir() - enemy.bestDir()) < ENEMY_ANGLE_THRESH then
      return true
    end
    if ball.velMod() < BALL_VEL_THRESH then
      return true
    end
  end
  if math.abs(player.velDir(him) - player.toBallDir(him)) < ENEMY_ANGLE_THRESH then
    return true
  end
end

local canCharge = function()
  local him = enemy.best()
  if enemyCanChip() then
    return false
  end
  if enemyHasBall() then
    return false
  elseif player.toBallDist(him) > player.toBallDist("Goalie") then
    return true
  -- elseif player.velDir("Goalie") - math.abs()
  end
  return false
end

local enemyCanChip = function()
  local him = enemy.best()
  if player.toOurGoalDist("Goalie") > 100 then
    if enemyHasBall() then
      return true
    end
  elseif player.velDir("Goalie") - (player.pos("Goalie") - OUR_GOAL_CENTER):dir()
  > math.pi*2/3 then
    return true
  elseif player.toPlayerDist("Goalie", him) < CHIP_DIST_THRESH then
    return true
  end
end


gPlayTable.CreatePlay{
  firstState = "start",

  ["start"] = {
    switch = function()
      if bufcnt(cond.isNormalStart(), 15) then
        _timer = vision:Cycle()
        return "normalGoalie"
      end
    end,
    Leader   = task.continue(),
    Special  = task.continue(),
    Assister = task.continue(),
    Middle   = task.continue(),
    Defender = task.continue(),
    Goalie   = task.goCmuRush(CGeoPoint:new_local(-450 + 8.5,0)),
    match    = "{LASMD}"
  },

  ["normalGoalie"] = {
    switch = function()
      if canCharge() then
        return "charge"
      end
      if vision:Cycle() > _timer + 600 + buffer then
        print("timeout")
        return "exit"
      end
    end,
    Leader   = task.continue(),
    Special  = task.continue(),
    Assister = task.continue(),
    Middle   = task.continue(),
    Defender = task.continue(),
    Goalie   = task.goalie(),
    match    = "{LASMD}"
  },

  ["charge"] = {
    switch = function ()
      if vision:Cycle() > _timer + 600 + buffer then
        print("time out")
        return "exit"
      end
      if enemyCanChip() then
        return "defendChip"
      end
      if enemyHasBall() and not canCharge() then
        return "normalGoalie"
      end
    end,
    Leader   = task.continue(),
    Special  = task.continue(),
    Assister = task.continue(),
    Middle   = task.continue(),
    Defender = task.continue(),
    Goalie   = task.shootoutGoalie(),
    match    = "{LASMD}"
  },

  ["defendChip"] = {
    switch = function()
      if vision:Cycle() > _timer + 600 + buffer then
        print("time out")
        return "exit"
      end
      if canCharge() then
        return "charge"
      end
    end,
    Leader   = task.continue(),
    Special  = task.continue(),
    Assister = task.continue(),
    Middle   = task.continue(),
    Defender = task.continue(),
    Goalie   = task.shootoutGoalie(),
    match    = "{LASMD}"
  },

  name = "Ref_PenaltyDefV5",
  applicable = {
    exp = "a",
    a = true
  },
  attribute = "attack",
  timeout = 99999
}
