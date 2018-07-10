gOppoConfig = {
  ----------------------play---------------------------------------------------------------
    CornerKick  = {40},   -- 挑对角{1,4,6*,16,17,23,30,40*}, 传中间{8,22}, 向后传{11,31}, 门将射门{18}
	FrontKick   = function()
		if math.abs(ball.refPosX()) < 150 then
			return {13}  --{3,4,13(Immortal)} {5}
		else
			return {2}  -- {5}
		end
	end,
	MiddleKick  = function()
		if math.abs(ball.refPosX()) < 130 then
			return {7} --{5(Immortal),7}
		else
			return {7}
		end
	end,
	BackKick    = {"Ref_ImmortalKickV1"}, --{10, "Ref_ImmortalKickV1"}
	CornerDef   = "Ref_CornerDefV4", -- 可能要用v4，防头球
	BackDef 	= function()
		if math.abs(ball.refPosY()) > 100 then
			return "Ref_BackDefV5"  --保守用v5
		else
			return "Ref_BackDefV8"--挑过中场犯规脚本
		end
	end,
	MiddleDef   = "Ref_MiddleDefV10",--横向marktouch用v5，四车markfront用v10,保守打法用V2
	FrontDef    = "Ref_FrontDefV8", --四车markfront用v9,若挑就用V7，若他们四车全上就用V11,保守打法用Ref_FrontDefV2
	PenaltyKick = "Ref_PenaltyKickV3", --Ref_PenaltyKick2017V5
	PenaltyDef  = "Ref_PenaltyDefV4", --Ref_PenaltyDefV3
	KickOff		= "Ref_KickOffV1",
	KickOffDef  = "Ref_KickOffDefV1",
	NorPlay     = "NormalPlayMark"
}
