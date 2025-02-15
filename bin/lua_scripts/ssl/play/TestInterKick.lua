local getNewTargetPos = function()
	math.randomseed(tostring(os.time()):reverse():sub(1, 6))    --优化随机数生成 
	local TargetPos_x = (math.random()-0.5)*param.pitchLength*0.4
	print("TargetPos_x = "..TargetPos_x)
	math.randomseed(tostring(os.time()):reverse():sub(1, 6))
	local TargetPos_y = (math.random()-0.5)*param.pitchWidth*0.75
	print("TargetPos_y = "..TargetPos_y)
	local TempTargetPos  = CGeoPoint:new_local(TargetPos_x,TargetPos_y)
	return TempTargetPos
end

local TargetPos  = CGeoPoint:new_local(-150,160)
--local TargetPos  = getNewTargetPos()

gPlayTable.CreatePlay{

firstState = "prepare",
["prepare"] = {
	switch = function ()
		if bufcnt(player.toPointDist("Kicker",TargetPos)<20 and player.toPointDist("Kicker",ball.pos())>300, "slow") then
		print("Jump wait")
			return "wait"
		end
	end,
	Kicker  = task.goCmuRush(TargetPos,_,300),
	match = ""
},

["wait"] = {
	switch = function ()
		--print("wait")
		if bufcnt(player.toPointDist("Kicker",ball.pos())<200 and ball.velMod()>50 and ball.valid(), "fast")then
		    print("Jump inter")
			return "inter"
		end
	end,
	Kicker  = task.goCmuRush(TargetPos,_,300),
	match = ""
},

["inter"] = {
    switch = function ()
    	--print("chase")
		if bufcnt((player.kickBall("Kicker") or math.abs(player.posX("Kicker"))>param.pitchLength/2-20
			or math.abs(player.posY("Kicker"))>param.pitchWidth/2-10 or ball.posX()>365 or ball.posY()<-290 or ball.posY()>290) , 2) then
		--print("GoingThrough") --这个函数似乎有问题
		--print(player.kickBall("Kicker"))
		print("Jump prepare")
			return "prepare"
		end
		if bufcnt(player.posX("Kicker")==0 and player.posY("Kicker")==0, 500) then
			return "prepare"
		end
	end,
	Kicker = task.InterNew(),
	match = ""
},

name = "TestInterKick",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}



