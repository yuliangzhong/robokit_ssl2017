local pos = ball.refAntiYPos(CGeoPoint:new_local(300,160))

gPlayTable.CreatePlay{

firstState = "start",
["start"] = {
	switch = function ()
	end,
	Goalie = task.goalie(),
	Kicker = task.goSimplePos(pos),--第二个参数是射门力度！
	match  = ""
},

-- ["start"] = {
-- 	switch  = function()
-- 		if bufcnt(player.toBallDist("Kicker")<20,120,_) then 
-- 			return "testState1"
-- 		end
-- 	end,
-- 	Kicker = task.staticGetBall(CGeoPoint:new_local(-450,0)),
-- 	match = ""
-- },

-- ["testState1"] = {
-- 	switch  = function()
-- 		if bufcnt(player.kickBall("Kicker"),10,_) then 
-- 			return "testState2"
-- 		end
-- 	end,
-- 	Kicker = task.goAndTurnKick(CGeoPoint:new_local(450,0)),
-- 	match = ""
-- },

-- ["testState2"] = {
-- 	switch = function()
-- 		print("test state 2")
-- 		-- if bufcnt(player.kickBall("Kicker"),10,_) then 
-- 		-- 	return "testState2"
-- 		-- end
-- 	end,
-- 	Kicker = task.stop(),
-- 	match = ""
-- },

name = "TestSkillPlay",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout   = 99999
}
