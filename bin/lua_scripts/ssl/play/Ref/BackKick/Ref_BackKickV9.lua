
local chipPos = CGeoPoint:new_local(150,130)

local MiddlePos    = ball.refAntiYPos(CGeoPoint:new_local(-100,-50))    --挡人位置一
local DisguisePos1 = ball.refAntiYPos(CGeoPoint:new_local(200,-120))
local DisguisePos2 = ball.refAntiYPos(CGeoPoint:new_local(180,-50))
local DisguisePos3 = ball.refAntiYPos(CGeoPoint:new_local(120,-100))

local PreparePos = ball.refAntiYPos(CGeoPoint:new_local(-200,120))
local GetPos = ball.refAntiYPos(CGeoPoint:new_local(0,100))
-- 大场：
-- local chipPos = CGeoPoint:new_local(250,230)

-- local MiddlePos    = ball.refAntiYPos(CGeoPoint:new_local(-100,-150))    --挡人位置一
-- local DisguisePos1 = ball.refAntiYPos(CGeoPoint:new_local(300,-220))
-- local DisguisePos2 = ball.refAntiYPos(CGeoPoint:new_local(280,-150))
-- local DisguisePos3 = ball.refAntiYPos(CGeoPoint:new_local(220,-200))

-- local PreparePos = ball.refAntiYPos(CGeoPoint:new_local(-300,220))
-- local GetPos = ball.refAntiYPos(CGeoPoint:new_local(0,200))

-- local ballYFactor = ball.antiY()

local function DisguiseChasePos()

	local x = ball.posX()+80;
	local y = ball.posY()+ball.antiY()*20;
	return CGeoPoint:new_local(x,y)
end

local function BlockPos()
	local x = ball.posX()+60;
	local y = ball.posY()+ball.antiY()*60;
	return CGeoPoint:new_local(x,y)
end

local function followPos()
	local x=ball.posX();
	local y=ball.posY()-ballYFactor*50;
	if ball.posX()>-50 then
		return DisguisePos3()
	end
	return CGeoPoint:new_local(x,y)
end

gPlayTable.CreatePlay{
	firstState = "start",

	["start"] = {
		switch = function ()
			if bufcnt(player.toTargetDist("Special") < 50 and
					      player.toTargetDist("Assister") < 50, 10, 120) then
				return "getBall"
			end
		end,
		Kicker   = task.staticGetBall(chipPos),
		Special  = task.goCmuRush(PreparePos),
		Assister = task.goCmuRush(DisguiseChasePos),
		Middle   = task.goCmuRush(MiddlePos),
		Defender = task.goCmuRush(BlockPos),
		Goalie   = task.goalie(),
		match    = "[SADM]"
	},

	["getBall"] = {
		switch = function ()
			if bufcnt(player.toTargetDist("Kicker") < 50 and player.toTargetDist("Special") < 30, 60) then
				return "pass"
			end
		end,
		Kicker   = task.slowGetBall(chipPos),
		Assister = {GoCmuRush{pos = DisguisePos1, dir = dir.shoot(), acc = 250}},
		Middle   = task.goCmuRush(MiddlePos),
		Special  = task.goCmuRush(PreparePos),
		Defender = task.goCmuRush(BlockPos),
		Goalie   = task.goalie(),
		match    = "[SADM]"
	},

	["pass"] = {
		switch = function ()
			if bufcnt(player.kickBall("Kicker"), "fast", 60) then
				return "chase"
			end
		end,
		-- Kicker   = task.chipPass(chipPos, 160),
		Kicker   = task.chipPass(chipPos, 260),
		Assister = task.goCmuRush(DisguisePos1),
		Middle   = task.goCmuRush(DisguisePos2),
		Special  = {GoCmuRush{pos = GetPos, dir = dir.shoot(), acc = 250}},
		Defender = task.goCmuRush(followPos),
		Goalie   = task.goalie(),
		match    = "[SADM]"
	},

	["chase"] = {
		switch = function ()
			if bufcnt(player.kickBall("Special"), 1, 180) then
				return "exit"
			end
		end,
		Kicker   = task.singleBack(),
		Assister = task.goCmuRush(DisguisePos1),
		Middle   = task.goCmuRush(DisguisePos2),
		Special  = task.shoot(),
		Defender = task.goCmuRush(followPos),
		Goalie   = task.goalie(),
		match    = "{SD}[AM]"
	},

	name 	   	= "Ref_BackKickV9",
	applicable 	= {
		exp = "a",
		a   = true
	},
	attribute 	= "attack",
	timeout 	= 99999
}
