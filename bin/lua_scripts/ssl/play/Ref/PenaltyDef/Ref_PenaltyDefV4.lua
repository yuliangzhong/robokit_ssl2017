local buffer = 30
local _enemyDeepestX = 150
local _timer 
local ADVANCE_POS1 = CGeoPoint:new_local(-350,0)
local DEFEND_CHIP_POS = CGeoPoint:new_local(-350,0)
local USE_NORMAL_GOALIE = true
local BALL_VEL_TRESH = 400
local PASSIVE_BALL_VEL_THRESH = 100
local ENEMY_SAFE_DIST = 200
local CHIP_DIST_THRESH
local OUR_GOAL_CENTER = CGeoPoint:new_local(-450,0)
local DIST_THRESH = 20
local ENEMY_ANGLE_THRESH = 0.2
local LITTLE_CHIP_OR_DANGER_DANGER_X = -100
local NEAR_GOALLINE_THRESH = 120


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

local ballNearGoalLine = function()
  local side = player.posX("Goalie") / math.abs(player.posX("Goalie"))
  print((ball.posX() - param.pitchLength/2*side) * side)
  if math.abs(ball.posX() - param.pitchLength/2*side) < NEAR_GOALLINE_THRESH then
    return true
  else 
    return false
  end
end

local ballBehindGoalie = function()
  local delta = ball.posX() - player.posX("Goalie")
  -- left delta < 0, right delta > 0
  local side = player.posX("Goalie") / math.abs(player.posX("Goalie"))
  return delta * side > 0
end

local enemyCanChip = function()
  local him = enemy.best()
  if player.toOurGoalDist("Goalie") > 100 then
    if enemyHasBall() then
      return true
    end
  elseif player.velDir("Goalie") - (player.pos("Goalie") - OUR_GOAL_CENTER):dir() > math.pi*2/3 then
    return true
  elseif player.toPlayerDist("Goalie", him) < CHIP_DIST_THRESH then
    return true
  end
end

-- for ER Force
local enemyCanLittleChip = function()
  local him = enemy.best()
  local himx = enemy.posX(him)
  -- print(type(him))
  -- print(himx)
  local side = player.posX("Goalie") / math.abs(player.posX("Goalie"))
  if not enemyHasBall then return false end
  if himx < LITTLE_CHIP_OR_DANGER_DANGER_X * side then
    -- print(player.posX(him))
    if player.posX("Goalie") > himx/2 - param.pitchLength/4 then
      return true
    elseif player.velMod("Goalie") > 50 then
      return true
    end
  else
    return false
  end
end

local isPassive = function()
  local him = enemy.best()
  if ball.velMod() > BALL_VEL_TRESH then -- shoot
    return false
  elseif ball.posX() < 0 and ball.velMod() > PASSIVE_BALL_VEL_THRESH then
    return false
  elseif player.toPlayerDist(him,"Goalie") < ENEMY_DIST_THRESH then
    return false
  else
    return true
  end
end

local updateEnemyDeepestX = function()
  _enemyDeepestX = math.min(_enemyDeepestX, enemy.posX(enemy.best()))
end

gPlayTable.CreatePlay{

firstState = "start",

["start"] = {
  switch = function ()
    if bufcnt(cond.isNormalStart(), 15) then
      -- if bufcnt(isPassive(),15) then
      --   return "advance1"
      -- else return "shootoutGoalie"
      -- end
      _timer = vision:Cycle()
      return "advance1"
    end
    updateEnemyDeepestX()
  end,
  Goalie   = task.goCmuRush(CGeoPoint:new_local(-450 + 8.5,0)),
  match    = ""
},

["advance1"] = {
  switch = function()
    if bufcnt(player.toTargetDist("Goalie") < 10, 15) then
      if isPassive() then
        return "defendChip"
      end
    else  
      if USE_NORMAL_GOALIE then
        return "normalGoalie"
      else
        return "shootoutGoalie"
      end
    end
    updateEnemyDeepestX()
  end,
  Goalie   = task.goCmuRush(ADVANCE_POS1),
  match    = ""
},

["shootoutGoalie"] = {
  switch = function ()
    -- print("aaaa")
    if vision:Cycle() > 600 + buffer + _timer then
      print("time out ... exit")
      -- return "exit"
    end
    updateEnemyDeepestX()
  end,
  Goalie   = task.shootoutGoalie(),
  match    = ""
},

["defendChip"] = {
  switch = function()
    if enemyCanChip() then
      return "normalGoalie"
    end
    if not isPassive() then
      if USE_NORMAL_GOALIE then
        return "normalGoalie"
      else
        return "shootoutGoalie"
      end
    end
    -- if player.kickball(enemy.best()) then
    --   return 'normalGoalie'
    -- end
    if vision:Cycle() > 600 + buffer + _timer then
      print(vision:Cycle())
      print("time out ... exit")
      -- return "exit"
    end
    updateEnemyDeepestX()
  end,
  Goalie   = task.goCmuRush(DEFEND_CHIP_POS,0),
  match    = "{LASMD}"
},

["normalGoalie"] = {
  switch = function()
    if ballBehindGoalie() then return  end
    if ballNearGoalLine() then return end
    if vision:Cycle() > _timer + 600 + buffer then
      print("time out")
    end
    -- if isPassive() then
    --   return "defendChip"
    -- end
    local him = enemy.best()
    local himx = enemy.posX(him)
    if himx > LITTLE_CHIP_OR_DANGER_DANGER_X then return 
    else 
      if enemyCanLittleChip() then
        return "justAdvance"
      end
    end
    updateEnemyDeepestX()
  end,
  Goalie   = task.goalie(),
  match    = ""
},

["justAdvance"] = {
  switch = function()
    -- if ballNearGoalLine then return "normalGoalie" end
    if not enemyCanLittleChip() or ballNearGoalLine() then
      if USE_NORMAL_GOALIE then
        return "normalGoalie"
      else 
        return "shootoutGoalie"
      end
    end
  end,
  Goalie = task.advance(),
  match = ""
},

name = "Ref_PenaltyDefV4",
applicable = {
  exp = "a",
  a = true
},
attribute = "attack",
timeout = 99999
}
