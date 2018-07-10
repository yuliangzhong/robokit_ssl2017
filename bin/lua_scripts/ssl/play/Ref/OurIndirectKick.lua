-- 在进入每一个定位球时，需要在第一次进时进行保持
--need to modify
-- if math.abs(ball.refPosY()) < 200 then
-- 	dofile("./lua_scripts/ssl/play/Ref/FrontKick/FrontKick.lua")
-- else
	if ball.refPosX() > 360 then
		dofile("./lua_scripts/ssl/play/Ref/CornerKick/CornerKick.lua")
	elseif ball.refPosX() > 0 and ball.refPosX() < 362 then
		dofile("./lua_scripts/ssl/play/Ref/FrontKick/FrontKick.lua")
	elseif ball.refPosX() > -250 and ball.refPosX() < 2 then
		dofile("./lua_scripts/ssl/play/Ref/MiddleKick/MiddleKick.lua")
	else
		dofile("./lua_scripts/ssl/play/Ref/BackKick/BackKick.lua")
	end
-- end

gOurIndirectTable.lastRefCycle = vision:Cycle()