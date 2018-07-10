function ShootoutGoalieV2(task)
  local mflag = task.flag or 0

  execute = function(runner)
    return CShootoutGoalieV2(runner, mflag)
  end

  -- local times = 0
  matchPos = function()
    -- times = times + 1
    -- print("mpos in ShootoutGoalie is called "..times.." times so far")
    return pos.goaliePos()
  end

  return execute,matchPos
end

gSkillTable.CreateSkill{
  name = "ShootoutGoalieV2",
  execute = function (self)
    print("This is in skill"..self.name)
  end
}
