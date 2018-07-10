if gSwitchNum["state"] == "normal" then
	if not player.realNumExist(gSwitchNum["normal"]) and player.realNumExist(gSwitchNum["backup"]) then
		print("change to backup")
		gSwitchNum["state"] = "backup"
	end
elseif gSwitchNum["state"] == "backup" then
	if not player.realNumExist(gSwitchNum["backup"]) and player.realNumExist(gSwitchNum["normal"]) then
		print("change to normal")
		gSwitchNum["state"] = "normal"
	end
end

gNormalPlay = gOppoConfig.NorPlay
-- 先注掉，在需要时可以开启使用
if (not IS_TEST_MODE) and USE_SWITCH and gSwitchNum["state"] == "backup" then
	dofile("./lua_scripts/ssl/opponent/Backup.lua")
else
	dofile("./lua_scripts/ssl/opponent/"..OPPONENT_NAME..".lua")	
end

function RunRefScript(name)
	local filename = "./lua_scripts/ssl/play/Ref/"..name..".lua"
	dofile(filename)
end

function SelectRefPlay()
	local curRefMsg = vision:GetCurrentRefereeMsg()
	if curRefMsg == "" then
		return false
	end
	if curRefMsg == "ourIndirectKick" and gLastRefMsg ~= curRefMsg then
		gIsOurIndirectKickExit = false
	end
	if curRefMsg == "ourIndirectKick" and gIsOurIndirectKickExit then
		return false
	end
	if gLastRefMsg ~= curRefMsg or curRefMsg == "gameStop" then
		ball.updateRefMsg()
	end
	RunRefScript(curRefMsg)
	gLastRefMsg = curRefMsg
	return true
end

function SelectBayesPlay()
	--gCurrentPlay = "Nor_PassAndShoot"
	-- gCurrentPlay = "Nor_MorePass"

	--add by twj 14/5/12
	--------------------
	if gNormalPlay == "NormalPlayNew" or gNormalPlay == "NormalPlayDefend" or gNormalPlay == "NormalPlayOneState" then
		world:clearBallStateCouter()
	end
	---------------------
	gCurrentPlay = gNormalPlay
	ResetPlay(gCurrentPlay)
end

if SelectRefPlay() then
	-- or NeedExit(gCurrentPlay)不添加会出问题！！
	if gCurrentPlay ~= gLastPlay or NeedExit(gCurrentPlay) then
		ResetPlay(gCurrentPlay)
		if vision:GetCurrentRefereeMsg() == "ourIndirectKick" then
			gIsOurIndirectKickExit = true
		end
		print("New Play: "..gCurrentPlay)
	else
--~ 		print("TimerCounter: ", gTimeCounter)
	end
else
	if IS_TEST_MODE then
		if gLastPlay == "" or NeedExit(gCurrentPlay) then
			gCurrentPlay = gTestPlay
		end
		if gCurrentPlay ~= gLastPlay or
			NeedExit(gCurrentPlay) then
			ResetPlay(gCurrentPlay)
		end		
	else
		--add by twj 14/5/12
		------------------------------------------
		if gNormalPlay == "NormalPlayNew" or gNormalPlay == "NormalPlayDefend" or gNormalPlay == "NormalPlayOneState" 
		or gNormalPlay == "NormalPlayMark" then
			world:setBallHandler(gRoleNum["Leader"])
			gLastBallStatus=gCurrentBallStatus
			gCurrentBallStatus=world:getBallStatus(vision:Cycle(),gRoleNum["Leader"])
			gLastFieldArea=gCurrentFieldArea
			gCurrentFieldArea=cond.judgeFieldArea()
		end
		--------------------------------------------

		if gLastPlay == "" or NeedExit(gCurrentPlay) then
			print("Play: "..gCurrentPlay.." Exit!!")
			SelectBayesPlay()
		end
	end
end

gLastPlay = gCurrentPlay

RunPlay(gCurrentPlay)

local stateFile=io.open(".\\LOG\\ssl\\"..gStateFileNameString..".txt","a")
stateFile:write(vision:Cycle().." "..gCurrentState.." "..gCurrentPlay.." me:"..bestPlayer:getOurBestPlayer()
	.." he:"..bestPlayer:getTheirBestPlayer().."\n")
stateFile:close()

--print( world:getSuitSider())
--print("vel"..enemy.vel(bestPlayer:getTheirGoalie()):y()*2)
--print(bestPlayer:getTheirGoalie())
--print("hello",bestPlayer:getOurBestPlayer())
--print(gCurrentState,vision:Cycle())
--print("ball",ball.posX(),ball.valid())
--print("raw",vision:RawBall():X(),vision:RawBall():Valid())
--print(vision:getBallVelStable(),vision:ballVelValid())
debugEngine:gui_debug_msg(CGeoPoint:new_local(455, 60),gCurrentState)
--print(gCurrentPlay)
--world:drawReflect(gRoleNum["Tier"])
--defenceInfo:setNoChangeFlag()
--print(vision:Cycle()..vision:GetCurrentRefereeMsg(),vision:gameState():gameOn())