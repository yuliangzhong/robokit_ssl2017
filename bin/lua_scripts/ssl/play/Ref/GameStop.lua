-- change to double size by yys
if ball.refPosX() < -360 and math.abs(ball.refPosY()) > 200 then
	gCurrentPlay = "Ref_Stop4CornerDef"
elseif ball.refPosX() > 360 and math.abs(ball.refPosY()) > 220 then
	gCurrentPlay = "Ref_Stop4CornerKick"
else
	gCurrentPlay = "Ref_StopV2"
end