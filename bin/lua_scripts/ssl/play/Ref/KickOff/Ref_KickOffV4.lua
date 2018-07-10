-- 向前小挑一脚，后面的车插上射门
-- by zhyaic 2013.6.15

local CHIP_POS = function()
	return player.toPointDir(CGeoPoint:new_local(300,250),"Kicker")
end

gPlayTable.CreatePlay{
firstState = "startball",

["startball"] = {
	switch = function ()
		if bufcnt(cond.isNormalStart(), "normal") then
			return "gotemp"
		end
	end,
	Kicker   = task.staticGetBall(CHIP_POS),
	Assister = task.goCmuRush(CGeoPoint:new_local(-9, 80)),
	Special  = task.goCmuRush(CGeoPoint:new_local(-9, -80)),
	Defender = task.singleBack(),
	Middle   = {GoCmuRush{pos = CGeoPoint:new_local(-100,250), dir = dir.shoot(), acc = 500, flag = flag.not_avoid_our_vehicle}},
	Goalie   = task.goalie(),
	match    = "{AMSD}"
},

["gotemp"] = {
	switch = function ()
		if bufcnt(true, 70) then
			return "gopos"
		end
	end,
	Kicker   = task.slowGetBall(CHIP_POS),
	Assister = task.goCmuRush(CGeoPoint:new_local(-9, 80)),
	Special  = task.goCmuRush(CGeoPoint:new_local(-9, -80)),
	Defender = task.singleBack(),
	Middle   = {GoCmuRush{pos = CGeoPoint:new_local(-100,250), dir = dir.shoot(), acc = 500, flag = flag.not_avoid_our_vehicle}},
	Goalie   = task.goalie(),
	match    = "{AMSD}"
},

["gopos"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Assister") < 60, "fast") then
			return "chipball"
		end
	end,
	Kicker   = task.slowGetBall(CHIP_POS),	
	Assister = task.goCmuRush(CGeoPoint:new_local(-9,75)),
	Special  = task.goCmuRush(CGeoPoint:new_local(-9,-175)),
	Middle   = {GoCmuRush{pos = CGeoPoint:new_local(300,250), dir = dir.shoot(), acc = 500, flag = flag.not_avoid_our_vehicle}},
	Defender = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{AMSD}"
},

["chipball"] = {
	switch = function ()
		if bufcnt(player.kickBall("Kicker") or 
			player.toBallDist("Kicker") > 20, "fast") then
			return "fixgoto"
		elseif  bufcnt(true, 300) then
			return "exit"
		end
	end,
	Kicker   = task.chipPass(CHIP_POS,90),
	Assister = task.goCmuRush(CGeoPoint:new_local(-9,75)),
	Middle   = {GoCmuRush{pos = CGeoPoint:new_local(300,250), dir = dir.shoot(), acc = 500, flag = flag.not_avoid_our_vehicle}},
	Special  = task.rightBack(),
	Defender = task.leftBack(),
	Goalie   = task.goalie(),
	match    = "{AM}(SD)"
},

["fixgoto"] = {
	switch = function ()
		if  bufcnt(true, 10) then
			return "shoot"
		end
	end,
	Kicker   = task.defendMiddle(),
	Assister = task.goCmuRush(CGeoPoint:new_local(-9,75)),
	Middle   = {GoCmuRush{pos = CGeoPoint:new_local(200,180), dir = dir.shoot(), acc = 500, flag = flag.not_avoid_our_vehicle}},
	-- Middle 	 = task.goCmuRush(CGeoPoint:new_local(300,250)),
	Defender = task.leftBack(),
	Special  = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{SMAD}"
},

["shoot"] = {
	switch = function ()
		if bufcnt(player.kickBall("Middle"), "fast", 120) then
			return "exit"
		end
	end,
	Kicker   = task.defendMiddle(),
	Middle   = task.shoot(),
	Assister = task.goCmuRush(CGeoPoint:new_local(-9,175)),
	Defender = task.leftBack(),
	Special  = task.rightBack(),
	Goalie   = task.goalie(),
	match    = "{SMAD}"
},

name = "Ref_KickOffV4",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
