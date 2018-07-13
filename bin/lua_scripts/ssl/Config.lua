IS_TEST_MODE = false
IS_SIMULATION = CGetIsSimulation()
USE_SWITCH = false
USE_AUTO_REFEREE = false
OPPONENT_NAME = "TestRef"

gStateFileNameString = string.format(os.date("%Y%m%d%H%M"))
gTestPlay = "TestReceivePass"--"TestSkillPlay"
gNormalPlay = "Nor_MorePass"
gSwitchNum = {
	["normal"]  = 6,
	["backup"]  = 7,
	["state"]   = "normal"
}

gRoleFixNum = {
	["Kicker"]   = {6},
	["Goalie"]   = {1},
	["Tier"]	 = {3},
	["Receiver"] = {4}
}

-- 用来进行定位球的保持
-- 在考虑智能性时用table来进行配置，用于OurIndirectKick
gOurIndirectTable = {
	-- 在OurIndirectKick控制脚本中可以进行改变的值
	-- 上一次定位球的Cycle
	lastRefCycle = 0
}

gSkill = {
	"SmartGoto",
	"SimpleGoto",
	"RunMultiPos",
	"RunMultiPosV2",
	"Stop",
	"PenaltyKick",
	"ShootBall",
	"ShootBallV2",
	"GetBall",
	"StaticGetBall",
	"GoAndTurnKick",
	"TouchKick",
	"SlowGetBall",
	"ForceStartRush",
	"ChaseKick",
	"ChaseKickV2",
	"DriftKick",
	"InterKick",
	"InterKickV2",
	"AdvanceBall",
	"ReceivePass",
	"Goalie2013",
	"Goalie2015",
	"Goalie2015V2",
	"TestCircleBall",
	"Learn/RoundBallTest",
	"Learn/RoundToPenalty",
	"RecordTrainData",
	"GoAndTurnKickV3",
	"OpenSpeed",
	"OpenSpeedLocalVersion",
	"PenaltyGoalie",
	"PenaltyGoalieV2",
	"ShootoutGoalie",
	"ShootoutGoalieV2",
	"Marking",
	"WaitKick",
	"GoAndTurn",
	"GotoMatchPos",
	"PenaltyKick2013",
	"GoCmuRush",
	"TouchBetweenPos",
	"CrazyPush",
	"GoAroundRobot",
	"Blocking",
	"SpeedInRobot",
	"MarkingXFirst",
	"ProtectBall",
	"GoAvoidShootLine",
	"DribbleTurn",
	"MarkingField",
	"MarkingTouch",
	"DribbleTurnKick",
	"GoAndTurnKickV4"
}

gRefPlayTable = {
	"Ref/TheirIndirectKick",
	"Ref/PenaltyKick/Ref_PenaltyKickV1",
	"Ref/PenaltyKick/Ref_PenaltyKickV2",
	"Ref/PenaltyKick/Ref_PenaltyKickV3",
	"Ref/PenaltyKick/Ref_PenaltyKickV4",
	"Ref/PenaltyKick/Ref_PenaltyKick2017V4",
	"Ref/PenaltyKick/Ref_PenaltyKick2017V5",
	"Ref/KickOff/Ref_KickOffV1",
	"Ref/KickOff/Ref_KickOffV2",
	"Ref/KickOff/Ref_KickOffV3",
	"Ref/KickOff/Ref_KickOffV4",
	"Ref/GameStop/Ref_StopV2",
	"Ref/GameStop/Ref_Stop4CornerDef",
	"Ref/GameStop/Ref_Stop4CornerKick",
	"Ref/Ref_HaltV1",
	"Ref/CornerKick/Ref_CornerKickV1",
	"Ref/CornerKick/Ref_CornerKickV2",
	"Ref/CornerKick/Ref_CornerKickV3",
	"Ref/CornerKick/Ref_CornerKickV4",
	"Ref/CornerKick/Ref_CornerKickV5",
	"Ref/CornerKick/Ref_CornerKickV6",
	"Ref/CornerKick/Ref_CornerKickV7",
	"Ref/CornerKick/Ref_CornerKickV8",
	"Ref/CornerKick/Ref_CornerKickV9",
	"Ref/CornerKick/Ref_CornerKickV10",
	"Ref/CornerKick/Ref_CornerKickV11",
	"Ref/CornerKick/Ref_CornerKickV12",
	"Ref/CornerKick/Ref_CornerKickV13",
	"Ref/CornerKick/Ref_CornerKickV14",
	"Ref/CornerKick/Ref_CornerKickV16",
	"Ref/CornerKick/Ref_CornerKickV17",
	"Ref/CornerKick/Ref_CornerKickV18",
	"Ref/CornerKick/Ref_CornerKickV19",
	"Ref/CornerKick/Ref_CornerKickV20",
	"Ref/CornerKick/Ref_CornerKickV21",
	"Ref/CornerKick/Ref_CornerKickV22",
	"Ref/CornerKick/Ref_CornerKickV23",
	"Ref/CornerKick/Ref_CornerKickV30",
	"Ref/CornerKick/Ref_CornerKickV31",
	"Ref/CornerKick/Ref_CornerKickV40",
	"Ref/CornerKick/Ref4_CornerKickV1",
	"Ref/FrontKick/Ref_FrontKickV1",
	"Ref/FrontKick/Ref_FrontKickV2",
	"Ref/FrontKick/Ref_FrontKickV3",
	"Ref/FrontKick/Ref_FrontKickV4",
	"Ref/FrontKick/Ref_FrontKickV5",
	"Ref/FrontKick/Ref_FrontKickV6",
	"Ref/FrontKick/Ref_FrontKickV7",
	"Ref/FrontKick/Ref_FrontKickV8",
	"Ref/FrontKick/Ref_FrontKickV9",
	"Ref/FrontKick/Ref_FrontKickV10",
	"Ref/FrontKick/Ref_FrontKickV11",
	"Ref/FrontKick/Ref_FrontKickV12",
	"Ref/FrontKick/Ref_FrontKickV13",
	"Ref/FrontKick/Ref_FrontKickV40",
	"Ref/FrontKick/Ref4_FrontKickV1",
	"Ref/MiddleKick/Ref_MiddleKickV1",
	"Ref/MiddleKick/Ref_MiddleKickV2",
	"Ref/MiddleKick/Ref_MiddleKickV3",
	"Ref/MiddleKick/Ref_MiddleKickV4",
	"Ref/MiddleKick/Ref_MiddleKickV5",
	"Ref/MiddleKick/Ref_MiddleKickV6",
	"Ref/MiddleKick/Ref_MiddleKickV7",
	"Ref/BackKick/Ref_ImmortalKickV1",
	"Ref/BackKick/Ref_BackKickV1",
	"Ref/BackKick/Ref_BackKickV2",
	"Ref/BackKick/Ref_BackKickV3",
	"Ref/BackKick/Ref_BackKickV4",
	"Ref/BackKick/Ref_BackKickV5",
	"Ref/BackKick/Ref_BackKickV6",
	"Ref/BackKick/Ref_BackKickV7",
	"Ref/BackKick/Ref_BackKickV8",
	"Ref/BackKick/Ref_BackKickV9",
	"Ref/BackKick/Ref_BackKickV10",
	"Ref/BackKick/Ref_BackKickV11",
	"Ref/BackKick/Ref_BackKickV12",
	"Ref/BackKick/Ref_BackKickV13",
	"Ref/BackKick/Ref_BackKickV14",
	"Ref/KickOffDef/Ref_KickOffDefV1",
	"Ref/PenaltyDef/Ref_PenaltyDefV1",
	"Ref/PenaltyDef/Ref_PenaltyDefV2",
	"Ref/PenaltyDef/Ref_PenaltyDefV3",
	"Ref/PenaltyDef/Ref_PenaltyDefV4",
	"Ref/PenaltyDef/Ref_PenaltyDefV5",
	"Ref/CornerDef/Ref_CornerDefV1",
	"Ref/CornerDef/Ref_CornerDefV1_2",
	"Ref/CornerDef/Ref_CornerDefV2",
	"Ref/CornerDef/Ref_CornerDefV3",
	"Ref/CornerDef/Ref_CornerDefV3_2",
	"Ref/CornerDef/Ref_CornerDefV4",
	"Ref/CornerDef/Ref_CornerDefV4_1",
	"Ref/CornerDef/Ref_CornerDefV4_2",
	"Ref/CornerDef/Ref_CornerDefV4_3",
	"Ref/CornerDef/Ref_CornerDefV5",
	"Ref/CornerDef/Ref_CornerDefV5_1",
	"Ref/CornerDef/Ref_CornerDefV6",
	"Ref/MiddleDef/Ref_MiddleDefV1",
	"Ref/MiddleDef/Ref_MiddleDefV1_2",
	"Ref/MiddleDef/Ref_MiddleDefV2",
	"Ref/MiddleDef/Ref_MiddleDefV2_2",
	"Ref/MiddleDef/Ref_MiddleDefV3",
	"Ref/MiddleDef/Ref_MiddleDefV4",
	"Ref/MiddleDef/Ref_MiddleDefV5",
	"Ref/MiddleDef/Ref_MiddleDefV6",
	"Ref/MiddleDef/Ref_MiddleDefV7",
	"Ref/MiddleDef/Ref_MiddleDefV8",
	"Ref/MiddleDef/Ref_MiddleDefV9",
	"Ref/MiddleDef/Ref_MiddleDefV10",
	"Ref/FrontDef/Ref_FrontDefV1",
	"Ref/FrontDef/Ref_FrontDefV2",
	"Ref/FrontDef/Ref_FrontDefV3",
	"Ref/FrontDef/Ref_FrontDefV4",
	"Ref/FrontDef/Ref_FrontDefV5",
	"Ref/FrontDef/Ref_FrontDefV6",
	"Ref/FrontDef/Ref_FrontDefV7",
	"Ref/FrontDef/Ref_FrontDefV8",
	"Ref/FrontDef/Ref_FrontDefV9",
	"Ref/FrontDef/Ref_FrontDefV10",
	"Ref/BackDef/Ref_BackDefV1",
	"Ref/BackDef/Ref_BackDefV1_2",
	"Ref/BackDef/Ref_BackDefV2",
	"Ref/BackDef/Ref_BackDefV2_2",
	"Ref/BackDef/Ref_BackDefV3",
	"Ref/BackDef/Ref_BackDefV4",
	"Ref/BackDef/Ref_BackDefV5",
	"Ref/BackDef/Ref_BackDefV6",
	"Ref/BackDef/Ref_BackDefV7",
	"Ref/BackDef/Ref_BackDefV8",
	"Ref/BackDef/Ref_BackDefV9"
}

gBayesPlayTable = {
	"Nor/Nor_PassAndShoot",
	"Nor/Nor_MorePass",
	"Nor/NormalPlayNew",
	"Nor/NormalPlayDefend",
	"Nor/NormalPlayOneState",
	"Nor/NormalPlayMark"
}

gTestPlayTable = {
	"TestRun",
	"TestNormal",
	"TestInterKick",
	"TestReceivePass",
	"TestKickWithoutBall",
	"TestChase",
	"TestChaseNew",
	"TestTwoTouch",
	"Cha_Goto",
	"TestSkillPlay",
	"TestPathPlanning",
	"TestOnePassShoot",
	"TestCompensate",
	"RunHeartShape",
	"RecordChipData",
	"TestGoAndTurnKick",
	"TestPassRecv",
	"VisionRecord",
	"RecordCircleBall",
	"TestImmortalKick",
	"TestAssPass",
	"TestRoundPass",
	"TestAvoidShootLine",
	"TestWaitTouch",
	"TestChipPass",
	"TestCirleAndKick",
	"TestTwoCtrMethod",
	"TestPassAndRush",
	"TestChipPassShoot",
	"CompensateRecord",
	"TestShortPassAndShoot",
	"TestHelpDefenceV1",
	"TestHelpDefenceV2",
	"IranOpenChallenge",
	"IranOpenChallenge_yellow",
	"TestForAngle",
	"TestDribbleTurnKick",
	"TestTurnPassAndShoot",
	"OpenSpeedTest",
	"TestOnePassShootV1",
	"TestDefence",
	"TestGoalie"
}
