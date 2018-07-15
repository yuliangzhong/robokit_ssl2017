--local targetP = CGeoPoint:new_local(p:x(), ball.antiY()*p:y())
--local TargetPos1  = ball.pos() + Utils.Polar2Vector(18, Utils.Normalize((ball.pos() - targetP):dir()))
--local TargetPos1  = ball.pos() + Utils.Polar2Vector(50, math.pi*2/4)
--local TargetPos1  = CGeoPoint:new_local(250,0)
--local TargetPos2  = CGeoPoint:new_local(-250,0)
local getReceiver = function ()
	return player.pos("Kicker") + Utils.Polar2Vector(28, math.pi*3/4)

local getTier = function ()
	return player.pos("Kicker") + Utils.Polar2Vector(42,math.pi*2/4)

local getGoalie() = function ()
	return player.pos("Kicker") + Utils.Polar2Vector(28,math.pi*1/4)



gPlayTable.CreatePlay{

firstState = "getready1",

["getready1"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Kicker") < 20, 180, 300) then
			return "getready1"
		end
	end,

	Kicker  = task.goCmuRush(CGeoPoint:new_local(250,0)),
	Receiver = task.goCmuRush(getReceiver),
	Tier = task.goCmuRush(getTier),
	Goalie = task.goCmuRush(getGoalie),
	match = ""
},

-- ["getready2"] = {
-- 	switch = function ()
-- 		if bufcnt(player.toTargetDist("Kicker") < 20, 180, 300) then
-- 			return "getready1"
-- 		end
-- 	end,
-- 	Kicker  = task.goCmuRush(CGeoPoint:new_local(-250,0)),
-- 	Receiver = task.goCmuRush(getkickerpos() + Utils.Polar2Vector(28, math.pi*3/4)),
-- 	Tier = task.goCmuRush(getkickerpos()+Utils.Polar2Vector(42,math.pi*2/4)),
-- 	Goalie = task.goCmuRush(getkickerpos()+Utils.Polar2Vector(28,math.pi*1/4)),
-- 	match = "[ALM]"
-- },

name = "TestRoundDefend",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}