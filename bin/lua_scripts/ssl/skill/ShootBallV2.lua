function ShootBallV2(task)
	local mdir
	
	execute = function(runner)
		if type(task.dir) == "function" then
			mdir = task.dir(runner)
		else
			mdir = task.dir
		end

		return CShootBallV2(runner, mdir)
	end

	matchPos = function()
		return pos.playerBest()
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "ShootBallV2",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}