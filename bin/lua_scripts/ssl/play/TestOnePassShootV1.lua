local pKick   = ball.refAntiYPos(CGeoPoint:new_local(200,182))
local pass_pos = pos.passForTouch(pKick)

gPlayTable.CreatePlay{

firstState = "wait",

["wait"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Kicker") < 20 , 100, 300) then
			cont=false
			record_flag=true
			delta_y=0
		    maxSpeed=-999
		    return "pass"
		end
	end,

	Kicker = task.goCmuRush(pKick),
	Tier   = task.staticGetBall("Kicker"),
	--Goalie = task.goSimplePos(pTarget,pi),
	match  = ""
},
["pass"] = {
	switch = function ()
		if bufcnt(player.kickBall("Tier"), 1, 300)then			
			return "stay"
		end
	end,		
	Kicker = task.goCmuRush(pKick),
	Tier   = task.goAndTurnKick(pass_pos,500),
	match  = ""
},
["stay"] = {

	switch = function ()
		if bufcnt(ball.toPlayerDist("Kicker") < 20,10,300) then
			return "kicker"
		end
		
	end,
	Kicker = task.touch(),
	Tier   = task.stop(),
	match  = ""
},

["kicker"] = {
	switch = function()
		if bufcnt(player.kickBall("Kicker"),1,9999) then
			return "sleep"
		end
	end,
	Kicher = task.touch(),
	match = ""
},
["sleep"] = {
	switch = function()
	end,
	Kicker = task.stop(),
	Tier   = task.stop(),
	match = ""
},

name = "TestOnePassShootV1",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
