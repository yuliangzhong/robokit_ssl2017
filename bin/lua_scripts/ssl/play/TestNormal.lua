local zeroPos  = CGeoPoint:new_local(80, 0)
local rightPos = CGeoPoint:new_local(200, 120)
local leftPos  = CGeoPoint:new_local(200, -120)

gPlayTable.CreatePlay{

firstState = "advance",

["advance"] = {
	switch = function ()
		if bufcnt(not player.isMarked("Special"), 45, 120) then
			return "pass"
		elseif bufcnt(player.toBallDist("Leader") < 15, 120) then
			return "back"
		elseif bufcnt(true, 200) then
			return "dribble"
		end
	end,
	Leader   = task.advance(),
	Special  = task.goPassPos("Leader"),
	Middle   = task.goPassPos("Special"),
	-- Kicker   = task.leftBack(),
	-- Tier     = task.rightBack(),
	-- Goalie   = task.goalie(),
	match    = "[LSM]"
},


["back"] = {
	switch = function ()
		if bufcnt(true, 60) then
			return "advance"
		end
	end,
	Leader   = task.goCmuRush(zeroPos),
	Special  = task.stop(),
	Middle   = task.stop(),
	-- Kicker   = task.goCmuRush(rightPos),
	-- Tier     = task.goCmuRush(leftPos),
	-- Goalie   = task.goalie(),
	match    = "(LSM)"
},

["pass"] = {
	switch = function ()
		if player.kickBall("Leader") or 
		   player.isBallPassed("Leader", "Special") then
			return "kick"
		elseif bufcnt(not cond.currentBayes("attack"), 100, 120) then
			return "advance"
		elseif bufcnt(player.toBallDist("Leader") < 15, 120) then
			return "back"
		end
	end,
	Leader   = task.pass("Special"),
	Special  = task.goPassPos("Leader"),
	Middle   = task.goPassPos("Special"),
	-- Kicker   = task.leftBack(),
	-- Tier     = task.rightBack(),
	-- Goalie   = task.goalie(),
	match    = "{LSM}"
},

["kick"] = {
	switch = function ()
		if bufcnt(player.kickBall("Special"), "fast", 160) then
			return "exit"
		end
	end,
	Leader   = task.goPassPos("Special"),
	Special  = task.shoot(),
	Middle   = task.goPassPos("Leader"),
	-- Kicker   = task.leftBack(),
	-- Tier     = task.rightBack(),
	-- Goalie   = task.goalie(),
	match    = "{LSM}"
},

name = "TestNormal",
applicable = {
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
