local CHIP_POS   = CGeoPoint:new_local(255,60)

gPlayTable.CreatePlay{

firstState = "goto",

["goto"] = {
	switch = function ()
		print("goto")
		if bufcnt(player.toTargetDist("Kicker")<30,  "slow")then
			return "dribble"
		end
	end,		
	Kicker  = task.goSpeciPos(CGeoPoint:new_local(200,-140)),
	Tier   =  task.staticGetBall(CGeoPoint:new_local(200,0)),	
	match = ""
},

["dribble"] = {
	switch = function ()
		print("dribble")
		if bufcnt(player.toBallDist("Tier")<15, "fast")then			
			return "chipBall"
		end
	end,		
	Kicker  = task.goSpeciPos(CGeoPoint:new_local(200,-140)),
	Tier   = task.slowGetBall(CHIP_POS),	
	match = ""
},	

["chipBall"] = {
    switch = function ()
    	print("chipBall")
		if bufcnt(player.kickBall("Tier"), 1) then						
			return "fixGoto"																			
		end	
	end,		
	Kicker = task.goSpeciPos(CGeoPoint:new_local(200,-140)),
	Tier = task.chipPass(CHIP_POS,120),	
	match = ""
},

["fixGoto"] = { 
 switch = function ()
 		print("fixGoto")
		if bufcnt(ball.posX()>200 and player.kickBall("Kicker"), 1) then					
				return "goto"																			
		end					
	end,	
	Kicker = task.waitTouch(CGeoPoint:new_local(200,-140),0),
	Tier = task.stop(),	
	match = ""
},	

name = "TestWaitTouch",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}

