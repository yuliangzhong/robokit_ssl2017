local theirGoal=CGeoPoint:new_local(450,0)

gPlayTable.CreatePlay{

firstState = "Pause",

["Pause"] = {
	switch = function ()
		if bufcnt(ball.valid() ,300) then  --and player.toBallDist("Kicker")<300 
			return "staticGetBall"
		end
	end,
	Kicker = task.stop(),
	match = ""
},

["staticGetBall"]={
	switch = function ()
		if bufcnt(player.toBallDist("Kicker")<20 ,150) then  --and player.toBallDist("Kicker")<300 
			return "goAndTurnKick"
		end
	end,
	Kicker = task.staticGetBall(CGeoPoint:new_local(0,-200)),
	match = ""

},

["goAndTurnKick"] = {
	switch = function ()
		if bufcnt(player.kickBall("Kicker"),"fast") then
			return "Wait"
		end
	end,
	Kicker =  task.goAndTurnKickQuick(theirGoal,1000,"flat"),
	--Kicker =  task.goAndTurnKick(theirGoal,1000),

	match = ""	
},

["Wait"] = {
	switch = function ()
		if bufcnt(true,100) then
			return "Pause"
		end
	end,
	Kicker =  task.stop(),
	match = ""	
},

name = "TestGoAndTurnKick",

applicable ={
	exp = "a",
	a = true
},

attribute = "attack",

timeout = 99999
}