if vision:Cycle() - gOurIndirectTable.lastRefCycle > 6 then
	if cond.validNum() <= 3 then
		gCurrentPlay = "Ref4_FrontKickV1"
	elseif math.abs(ball.refPosY()) < 130 then
		gCurrentPlay = "Ref_FrontKickV40"
	else
		if type(gOppoConfig.FrontKick) == "function" then
			gCurrentPlay = cond.getOpponentScript("Ref_FrontKickV", gOppoConfig.FrontKick(), 11)
		else
			gCurrentPlay = cond.getOpponentScript("Ref_FrontKickV", gOppoConfig.FrontKick, 11)
		end
	end
end