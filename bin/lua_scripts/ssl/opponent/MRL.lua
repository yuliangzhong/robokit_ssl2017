-- 可指定具体定位球，也可以以table的形式在几个定位球中随机选择

gOppoConfig = {
	----------------------play---------------------------------------------------------------
	CornerKick  = {23},  --首选{11,12,14,16,17}  --次选{3,4,9,13} --{6,15}
	FrontKick   = {6}, --首选{3,5,10} --次选{1,2,6,9,11} --{4}
	MiddleKick  = {5,6}, --首选{5,6} --万一放一个车在对面死防{4}
	BackKick    = "Ref_BackKickV6", --首选"Ref_BackKickV4" --次选"Ref_ImmortalKickV1"
	CornerDef   = "Ref_CornerDefV4",
	BackDef 	 =  function()
		if math.abs(ball.refPosY()) >100 then
			return "Ref_BackDefV7"
		else 
			return "Ref_BackDefV8"
		end
	end,
	MiddleDef   = function()
		if ball.refPosX()>160 then
			return "Ref_FrontDefV7"
		else 
			return "Ref_MiddleDefV7" 
		end
	end,
	FrontDef    = "Ref_FrontDefV7",
	PenaltyKick = "Ref_PenaltyKickV3",
	PenaltyDef  = "Ref_PenaltyDefV3",
	KickOff		  = "Ref_KickOffV1",
	KickOffDef  = "Ref_KickOffDefV1",
	NorPlay     = "NormalPlayDefend"
}