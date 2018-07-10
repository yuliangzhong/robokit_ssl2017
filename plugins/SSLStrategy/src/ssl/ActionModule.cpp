/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team:			SSL-ZJUNlict										*/
/* HomePage: http://www.nlict.zju.edu.cn/ssl/WelcomePage.html			*/
/************************************************************************/
/* File:	ActionModule.h												*/
/* Brief:	C++ Implementation: Action	execution						*/
/* Func:	Provide an action command send interface					*/
/* Author:	cliffyin, 2012, 08											*/
/* Refer:	NONE														*/
/* E-mail:	cliffyin007@gmail.com										*/
/************************************************************************/	

#include "SSLStrategy.h"

#include "ActionModule.h"
#include <tinyxml/ParamReader.h>
#include <KickStatus.h>
#include <DribbleStatus.h>
#include <TaskMediator.h>
#include <PlayerCommandV2.h>
#include <PlayInterface.h>
#include <CommandFactory.h>
#include <PathPlanner.h>
#include <BallStatus.h>

CActionModule::CActionModule(const COptionModule* pOption,CVisionModule* pVision,const CDecisionModule* pDecision)
: _pOption(pOption),_pVision(pVision),_pDecision(pDecision)
{

}

CActionModule::~CActionModule(void)
{

}

// ���ڵ������еĳ��Ŵ���5�����
bool CActionModule::sendAction(unsigned char robotIndex[])
{
	rbk::protocol::SRC_Cmd cmds;

	/************************************************************************/
	/* ��һ��������С����ִ�и�����������ɶ���ָ��                       */
	/************************************************************************/
	for (int vecNum = 1; vecNum <= Param::Field::MAX_PLAYER; ++ vecNum) {
		rbk::protocol::Message_SSL_Command* ssl_cmd = nullptr;
		// ��ȡ��ǰС������
		CPlayerTask* pTask = TaskMediator::Instance()->getPlayerTask(vecNum);
		// û����������
		if (NULL == pTask) {
			continue;
		}

		// ִ��skill�����������ִ�У��õ����յ�ָ�<vx vy w> + <kick dribble>
		// ִ�еĽ��������ӿڣ�����-DCom��ʵ��-CommandSender�� + ָ���¼���˶�-Vision���߿�-PlayInterface)
		bool dribble = false;
		CPlayerCommand* pCmd = NULL;
		pCmd = pTask->execute(_pVision); 

		if (!pCmd) {
			std::cout<<"PlayerCommand execute is Null "<<vecNum<<std::endl;
		}
		// �ܣ���Ч���˶�ָ��
		if (pCmd) {
			dribble = pCmd->dribble() > 0;
			// �·��˶� <vx vy w>
			if(ssl_cmd == nullptr){
				ssl_cmd = cmds.add_command();
				ssl_cmd->set_robot_id( PlayInterface::Instance()->getRealIndexByNum(pCmd->number()));
			}
			// for rbk
			//pCmd->execute(IS_SIMULATION, robotIndex[vecNum-1]);
			((CPlayerSpeedV2*)pCmd)->setSpeedtoSSLCmd(ssl_cmd);
			// ��¼ָ��
			_pVision->SetPlayerCommand(pCmd->number(), pCmd);
		}

		// �ߣ���Ч���߿�ָ��
		double kickPower = 0.0;
		double chipkickDist = 0.0;
		double passdist = 0.0;
		if (KickStatus::Instance()->needKick(vecNum) && pCmd) {
			// ��������ز���
			kickPower = KickStatus::Instance()->getKickPower(vecNum);
			chipkickDist = KickStatus::Instance()->getChipKickDist(vecNum);
			passdist = KickStatus::Instance()->getPassDist(vecNum);
			// �漰��ƽ/����ֵ�������ֻ��ϵ��ز�����ʵ�ʷֵ����ע CommandSender
			CPlayerKickV2 kickCmd(vecNum, kickPower, chipkickDist, passdist, dribble);
			// �������� <kick dribble>
			//kickCmd.execute(IS_SIMULATION);
			if(ssl_cmd == nullptr){
				ssl_cmd = cmds.add_command();
				ssl_cmd->set_robot_id( PlayInterface::Instance()->getRealIndexByNum(pCmd->number()));
			}
			kickCmd.setKicktoSSLCmd(ssl_cmd);
		}

		// ��¼����
		BallStatus::Instance()->setCommand(vecNum, kickPower, chipkickDist, dribble, _pVision->Cycle());
	}
	
	/************************************************************************/
	/* �ڶ�����ָ����մ���                                                 */
	/************************************************************************/
	// �����һ���ڵ�����ָ��
	KickStatus::Instance()->clearAll();
	// �����һ���ڵĿ���ָ��
	DribbleStatus::Instance()->clearDribbleCommand();
	// �����һ���ڵ��ϰ�����
	CPathPlanner::resetObstacleMask();

	GRBKHandle::Instance()->publishTopic(cmds);
	return true;
}

bool CActionModule::sendNoAction(unsigned char robotIndex[])
{
	rbk::protocol::SRC_Cmd cmds;
	for (int vecNum = 1; vecNum <= Param::Field::MAX_PLAYER; ++ vecNum) {
		// ����ֹͣ����
		CPlayerCommand *pCmd = CmdFactory::Instance()->newCommand(CPlayerSpeedV2(vecNum,0,0,0,0));
		// ִ�����·�
		//pCmd->execute(IS_SIMULATION);
		rbk::protocol::Message_SSL_Command* ssl_cmd = cmds.add_command();
		ssl_cmd->set_robot_id(PlayInterface::Instance()->getRealIndexByNum(pCmd->number()));
		((CPlayerSpeedV2*)pCmd)->setSpeedtoSSLCmd(ssl_cmd);
		// ��¼ָ��
		_pVision->SetPlayerCommand(pCmd->number(), pCmd);
	}
	GRBKHandle::Instance()->publishTopic(cmds);
	return true;
}

void CActionModule::stopAll()
{
	rbk::protocol::SRC_Cmd cmds;
	for (int vecNum = 1; vecNum <= Param::Field::MAX_PLAYER; ++ vecNum) {
		// ����ֹͣ����
		CPlayerCommand *pCmd = CmdFactory::Instance()->newCommand(CPlayerSpeedV2(vecNum,0,0,0,0));
		// ִ�����·�
		//pCmd->execute(IS_SIMULATION);
		rbk::protocol::Message_SSL_Command* ssl_cmd = cmds.add_command();
		ssl_cmd->set_robot_id(PlayInterface::Instance()->getRealIndexByNum(pCmd->number()));
		((CPlayerSpeedV2*)pCmd)->setSpeedtoSSLCmd(ssl_cmd);
		// ָ���¼
		_pVision->SetPlayerCommand(pCmd->number(), pCmd);
	}
	GRBKHandle::Instance()->publishTopic(cmds);
	return ;
}