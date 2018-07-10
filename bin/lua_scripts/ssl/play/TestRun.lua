local TargetPos1  = CGeoPoint:new_local(200,250)
local TargetPos2  = CGeoPoint:new_local(200,-250)
gPlayTable.CreatePlay{

firstState = "run1",

-- ["runX"] = {
-- 	switch = function ()
-- 		--print("runX")
-- 		if(player.velMod("Kicker")>maxvel) then
-- 			maxvel=player.velMod("Kicker")
-- 		end
-- 		if bufcnt(player.toPointDist("Kicker",TargetPosX)<20 , 200) then
-- 		    print(maxvel)
-- 			return "runY"
-- 		end
-- 	end,
-- 	Kicker  = task.goCmuRush(TargetPosX,0),
-- 	match = ""
-- },

-- ["runY"] = {
-- 	switch = function ()
-- 		--print("runY")
-- 		if(player.velMod("Kicker")>maxvel) then
-- 			maxvel=player.velMod("Kicker")
-- 		end
-- 		--print(player.velMod("Kicker"))
-- 		if bufcnt(player.toPointDist("Kicker",TargetPosY)<20 , 200) then
-- 			print(maxvel)
-- 			return "runX"
-- 		end
-- 	end,
-- 	Kicker  = task.goCmuRush(TargetPosY,0),
-- 	match = ""
-- },

["run1"] = {
	switch = function ()
		if player.toTargetDist("Kicker") < 20 then
			return "run2"
		end

	end,
	Kicker  = task.goCmuRush(TargetPos1, 0),
	match = ""
},
["run2"] = {
	switch = function ()
		if player.toTargetDist("Kicker") < 20 then
			return "run1";
		end
	end,
	Kicker  = task.goCmuRush(TargetPos2, 0),
	match = ""
},

name = "TestRun",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}

