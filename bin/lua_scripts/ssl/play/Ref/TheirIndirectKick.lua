-- 在进入每一个定位球时，需要在第一次进时进行保持
--need to modify
gNorPass2NorDefBallVel = 80
if ball.refPosX() < -360 then
	dofile("./lua_scripts/ssl/play/Ref/CornerDef/CornerDef.lua")
elseif ball.refPosX() > 250 then
	dofile("./lua_scripts/ssl/play/Ref/FrontDef/FrontDef.lua")
elseif ball.refPosX() > -5 then
	dofile("./lua_scripts/ssl/play/Ref/MiddleDef/MiddleDef.lua")
else
	dofile("./lua_scripts/ssl/play/Ref/BackDef/BackDef.lua")
end
