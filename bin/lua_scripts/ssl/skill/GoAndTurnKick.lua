function GoAndTurnKick(task)
	local mpos
	local mdir
	
	execute = function(runner)
		if type(task.dir) == "function" then
			mdir = task.dir(runner)
		else
			mdir = task.dir
		end
		return CGoAndTurnKick(runner, mdir, task.type, task.power)
	end

	matchPos = function()
		if type(task.pos) == "function" then
			mpos = task.pos()
		else
			mpos = task.pos
		end
		return mpos
	end

	return execute, matchPos
end

gSkillTable.CreateSkill{
	name = "GoAndTurnKick",
	execute = function (self)
		print("This is in skill"..self.name)
	end
}