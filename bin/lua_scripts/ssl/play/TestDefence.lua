--Test the action of Defence
--by zyl 20180711
local pos = ball.refAntiYPos(CGeoPoint:new_local(300,160))

gPlayTable.CreatePlay{

firstState = "start",
["start"] = {
	switch = function ()
	end,
	Goalie = task.goalie(),
	Tier = task.leftBack(),
	Receiver = task.rightBack(),
	Kicker = task.goAndTurnKick(player.pos("Goalie"),2000),--第二个参数是射门力度！
	match  = ""
},


name = "TestDefence",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout   = 99999
}
