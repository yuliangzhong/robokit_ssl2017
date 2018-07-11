--Test the action of Goalie
--by zyl 20180711
local pos = ball.refAntiYPos(CGeoPoint:new_local(300,160))

gPlayTable.CreatePlay{

firstState = "start",
["start"] = {
	switch = function ()
	end,
	Goalie = task.goalie(),
	Kicker = task.goAndTurnKick(player.pos("Goalie"),2000),--第二个参数是射门力度！
	match  = ""
},


name = "TestGoalie",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout   = 99999
}
