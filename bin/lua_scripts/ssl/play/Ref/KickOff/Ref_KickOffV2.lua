local FAKE_POS = CGeoPoint:new_local(9, 240)

gPlayTable.CreatePlay{
firstState = "start",

["start"] = {
	switch = function ()
		if cond.isNormalStart() then
			return "temp"
		end
	end,
	Leader   = task.fakeStand(FAKE_POS),
	Assister = task.goSpeciPos(CGeoPoint:new_local(-15, -240)),
	Special  = task.goSpeciPos(CGeoPoint:new_local(-15, 240)),
	Middle   = task.goSpeciPos(CGeoPoint:new_local(-170, 240)),
	Defender = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{LASMD}"
},

["temp"] = {
	switch = function ()
		if player.toTargetDist("Middle") < 150 then
			return "pass"
		end
	end,
	Leader   = task.fakeStand(FAKE_POS),
	Assister = task.goSpeciPos(CGeoPoint:new_local(-50, -57)),
	Special  = task.goSpeciPos(CGeoPoint:new_local(-50, 57)),
	Middle   = task.goSpeciPos(CGeoPoint:new_local(0, 240), dir.evaluateTouch(CGeoPoint:new_local(300, 0)), flag.not_avoid_our_vehicle),
	Defender = task.singleBack(),
	Goalie   = task.goalie(),
	match    = "{LASMD}"
},

["pass"] = {
	switch = function ()
		if bufcnt(player.kickBall("Leader"), 1, 120) then
			return "kick"
		end
	end,
	Leader   = task.fakeShoot(FAKE_POS),
	Middle   = task.goSpeciPos(CGeoPoint:new_local(0, 240), dir.evaluateTouch(CGeoPoint:new_local(300, 0)), flag.not_avoid_our_vehicle),
	Special  = task.goSpeciPos(CGeoPoint:new_local(-50, 57)),
	Defender = task.rightBack(),
	Assister = task.leftBack(),
	Goalie   = task.goalie(),
	match    = "{LASDM}"
},

-- TODO
["kick"] = {
    switch = function ()
		if bufcnt(player.kickBall("Middle"), 1, 90) then
			return "exit"
		end
	end,
	Leader   = task.goPassPos("Middle"),
	Special  = task.defendMiddle(),
	Middle   = task.touch(),
	Defender = task.rightBack(),
	Assister = task.leftBack(),
	Goalie   = task.goalie(),
	match    = "{LASDM}"
},

name = "Ref_KickOffV2",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}
