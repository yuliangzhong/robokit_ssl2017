local CHIP_POS   = CGeoPoint:new_local(255,60)

gPlayTable.CreatePlay{

firstState = "goto",

["goto"] = {
	switch = function ()
		--print("goto")
		if bufcnt(player.toTargetDist("Kicker")<30,  "slow")then
			return "pass"
		end
	end,		
	Kicker  = task.goSpeciPos(CGeoPoint:new_local(100,-150)),
	Tier   =  task.staticGetBall(CGeoPoint:new_local(200,100)),	
	match = ""
},

["pass"] = {
	switch = function ()
		--print("pass")
		if bufcnt(player.kickBall("Tier"), "fast")then			
			return "kick"
		end
	end,		
	Kicker  = task.goSpeciPos(CGeoPoint:new_local(100,-150)),
	Tier   = task.goAndTurnKick("Kicker"),	
	match = ""
},	

["kick"] = {
    switch = function ()
    	--print("kick")
		if bufcnt(player.kickBall("Kicker"), 1) then						
			return "goto"																			
		end	
	end,		
	Tier = task.stop(),
	Kicker = task.testChip(),	
	match = ""
},

name = "TestChipPass",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}

