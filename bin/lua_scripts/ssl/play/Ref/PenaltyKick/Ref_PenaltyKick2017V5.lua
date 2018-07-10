local enemyFlag = 31
-- enemyFlag 1： 表示出击型,家中搞对练的那个
--			 21： 保守型，没有吸着球走的程序，用挑一下追的方法（2017/7/27 地板弹性不是很强）
--			 22:  保守型，改动的第二个版本，使用之前的点球计算方法,好像有点小问题
--			 31:  出击型，吸住球踢门 
local startCycle
local SIDE_POS, preparePoint, INTER_POS = pos.refStopAroundBall()
local  ACC = 200
local STOP_FLAG = bit:_or(flag.slowly, flag.dodge_ball)
local STOP_DSS = bit:_or(STOP_FLAG, flag.allow_dss)

local goal      = CGeoPoint:new_local(0,100)
local ourGoal   = CGeoPoint:new_local(450,0)
local penaltyV4Param = {
	flatKickPower = 5555+16,			--没用的参数
	chipKickPower = 5555+30,			--小挑的力度
	kickDirection = math.pi/18,			--小挑的方向
	chipOverPower = 5555+100			--挑过守门员的力度	
}
local theirGoalieNum
local targetPos = CGeoPoint:new_local(-150+math.cos(penaltyV4Param["kickDirection"])*100,100*math.sin(penaltyV4Param["kickDirection"]))
local gCount = 0

local function canChipOver()
	if theirGoalieNum==0 then
		return 1
	end
	print("Goalie Num : "..theirGoalieNum)
	local goalieX=enemy.posX(theirGoalieNum)
	--local goalieVelY=enemy.vel(theirGoalieNum):y()*2
	local lastGoalieX = vision:TheirPlayer(vision:LastCycle(),theirGoalieNum):X()
	if goalieX < 50 + (lastGoalieX - goalieX) *30   then
		--print ("I CAN CHIPOVER: "..vision:Cycle() - startCycle.."     GOALIE Y: "..goalieY)
		return true
	else
		return false
	end
end

gPlayTable.CreatePlay{

firstState = "prepare",

	["prepare"] = {
		switch = function ()
			if bufcnt(player.toPointDist("Kicker",preparePoint)<50,"fast") then
				return "goto"
			end
		end,
		Kicker = task.goCmuRush(preparePoint, dir.playerToBall, ACC, STOP_DSS),
		match = ""
	},

	["goto"] = {
		switch = function ()
			if cond.isNormalStart() then
				startCycle = vision:Cycle()
				theirGoalieNum = bestPlayer:getTheirGoalie()
				if enemyFlag == 1 then
					return "KickOut"
				elseif enemyFlag == 21 then
					return "PassToSelf"
				elseif enemyFlag == 22 then
					return "PassToSelfV2"
				elseif enemyFlag == 31 then
					return "chipOverPrepare"
				end
			end
		end,
		Kicker   = task.goBackBall(penaltyV4Param.kickDirection,20),
		match    = ""
	},


	["KickOut"] = {
		switch = function ()
			if bufcnt(player.kickBall("Kicker") or player.toBallDist("Kicker") > 30,"fast") then
				return "tempRun1"
			end
		end,
		Kicker = task.chipPass(targetPos,penaltyV4Param.chipKickPower,nil,false),
		match = ""
	},

	["tempRun1"] = {
		switch =function ()
			if bufcnt(ball.posX()>-80,"fast") then
				return "shoot"
			end
		end,
		Kicker = task.goBackBall(CGeoPoint:new_local(450,0),15),
		match = ""
	},

	["shoot"] = {
		switch = function()
		end,
		Kicker = task.shoot(),
		match = ""
	},

	["PassToSelf"] = {
		switch = function ()
			if bufcnt(player.kickBall("Kicker"),"fast") then
				return "receiveShoot"
			end
		end,
		Kicker = task.chipPass(targetPos,penaltyV4Param["chipKickPower"],nil,false),
		match = ""
	},
	["receiveShoot"] = {
		switch = function ()
		end,
		Kicker = task.receiveShoot(),
		match = ""
	},

	["PassToSelfV2"] = {
		switch = function ()
			if bufcnt(player.kickBall("Kicker"),"fast") then
				return "slowGoto"
			end
		end,
		Kicker = task.chipPass(targetPos,penaltyV4Param["chipKickPower"]/2,nil,false),
		match = ""
	},

	["chipOverPrepare"] = {
		switch = function ()
			if bufcnt(canChipOver(),"fast",999) then
				print("CCCCCCCCCCCCCCCChip Over")
				return "chipOver"
			end
			if vision:Cycle() - startCycle > 180 then
				return "KickOut"
			end
		end,
		match = ""
	},

	["chipOver"] = {
		switch = function ()
			if bufcnt(player.kickBall("Kicker") or player.toBallDist("Kicker") > 30,"fast") then
				return "shoot"
			end
		end,
		Kicker = task.chipPass(ourGoal,penaltyV4Param["chipOverPower"],nil,false),
		match = ""
	},

	name = "Ref_PenaltyKick2017V5",

	applicable ={
		exp = "a",
		a = true
				},

	attribute = "attack",
	timeout = 99999
}
