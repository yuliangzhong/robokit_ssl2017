--it can test the speed of the ball by the way
local targetdierction
local realdirection = 0
local ballspeed
local balldirection
local init = true
local maxSpeed=-999
local pTarget =CGeoPoint:new_local(450, 0)
local cont=false
local record_flag=true
local pKick   = ball.refAntiYPos(CGeoPoint:new_local(200,100))
local pass_pos = pos.passForTouch(pKick)
local delta_y=0

gPlayTable.CreatePlay{

firstState = "wait",

["goto"] = {
	switch = function ()
		if bufcnt(player.toTargetDist("Kicker") < 20 , 100, 300) then
			cont=false
			record_flag=true
			delta_y=0
		    maxSpeed=-999
		    return "pass"
		end
	end,

	Kicker = task.goCmuRush(pKick),
	Tier   = task.staticGetBall("Kicker"),
	--Goalie = task.goSimplePos(pTarget,pi),
	match  = ""
},

["pass"] = {
	switch = function ()
		if bufcnt(player.kickBall("Tier"), 1, 300)then			
			return "kick"
		end
	end,		
	Kicker = task.goCmuRush(pKick),
	Tier   = task.goAndTurnKick(pass_pos,200),
	--Goalie = task.goSimplePos(pTarget,pi),
	match  = ""
},	

["kick"] = {
	switch   = function()
    	if ball.pos():dist(player.pos("Kicker")) < 95 then
      	mBallSpeed = ball.velMod()
      	mBallDirection = ball.velDir()
      	mTargetDirection = math.abs(Utils.Normalize(Utils.Normalize(ball.velDir() + math.pi) - player.dir("Kicker"))*180/math.pi)
    	end
    
    	if player.kickBall("Kicker") then
      		return "record"
    	end

    	if bufcnt(not player.kickBall("Kicker"),100) then
    		print("invalid!")
    		return "record"
    	end
    	
  	end,

	Kicker = task.touch(pTarget),
	Tier   = task.stop(),	

	match  = ""
},

["record"] = {

switch   = function()
	
	if ball.velMod()>maxSpeed then
		maxSpeed=ball.velMod()
	end

	if player.toBallDist("Kicker")>130 and player.toBallDist("Kicker")<150 then
        compensation = player.toBallDir("Kicker") - player.toTheirGoalDir("Kicker")  -- 补偿角度 = 实际击出的角度 - 需要击出的角度
        compensation = compensation/3.1415926*180
    end


    if ball.posX()>445 and record_flag==true then
    	delta_y=ball.posY()
    	recor_flag=false
    end


    --if bufcnt(ball.toPlayerDist("Kicker") > 150, 20, 120) then
    if bufcnt(ball.posX()>445, 5, 120) then

    	mDirection = player.toTheirGoalDir("Kicker")
    	mRealDirection = ball.velDir()

    	print("--------- Compensation Result ---------")
    	if(maxSpeed>800) then
		print("WARNING : Over Speed !!!")
    	end
        print("Max Speed (cm/s): "..maxSpeed.."\nPass Speed(cm/s): "..mBallSpeed)
        print("Pass Angle(deg): "..mTargetDirection.."\nDelta Y(cm):"..delta_y)
       
        if(ball.posY()<-25) then
        	print("Assesment: Lean Left.")
        end

        if(ball.posY()>25) then
        	print("Assesment: Lean Right.")
        end

        if(ball.posY()<=25 and ball.posY()>=-25) then
        	print("Assesment: Correct!")
        end

        local recordfile = io.open("CompensateRecordFile.txt","a")
        recordfile:write(maxSpeed,"\t",mBallSpeed,"\t", mTargetDirection, "\t",delta_y,"\n")
		recordfile:close()
    	
    	return "wait"
  	end

  	end,

	Kicker = task.stop(),
	Tier   = task.stop(),	
	match  = ""
},

["wait"] = {
  switch = function ()
  		--print("Ready press 'k'")
		--ch='p'
		--ch=io.read()
		
		--if(ch=='k') then
		--cont=true
		--end

		if true then--cont then
		    --print("next!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
		    return "goto"
		end
	end,

  Kicker = task.stop(),
  Tier   = task.stop(),
  match    = ""
},

name = "TestOnePassShoot",
applicable ={
	exp = "a",
	a = true
},
attribute = "attack",
timeout = 99999
}

