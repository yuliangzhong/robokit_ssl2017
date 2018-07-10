/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team��		SSL-ZJUNlict											*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	  gpuBestAlgThread.cpp										*/
/* Func:	  ���������λ����̣߳�֧��CPUģʽ��GPUģʽ				*/
/* Author:	  ��Ⱥ 2012-08-18											*/
/*            Ҷ���� 2014-03-01                                         */
/* Refer:	  ###														*/
/* E-mail:	  wangqun1234@zju.edu.cn									*/
/************************************************************************/

#include "gpuBestAlgThread.h"
#include "VisionModule.h"
#include "GDebugEngine.h"
#include "ShootRangeList.h"
#include "TimeCounter.h"
#include "tinyxml/ParamReader.h"
#include "param.h"

#define OURPLAYER_NUM	6
#define THEIRPLAYER_NUM 6
#define BALL_NUM		1

#define UN_GPU_PRECISE 10

extern CMutex* _best_visiondata_copy_mutex;
extern CMutex* _value_getter_mutex;

CGeoPoint C_ourPlayer_Geo[6];
CGeoPoint C_theirPlayer_Geo[6];
CGeoPoint C_ball_Geo;

namespace {
	CThreadCreator *_best_calculation_thread = 0;
	CUsecTimer timer;
	float(* C_PointPotential)[GRIDY] = NULL;  //��ʱ�õ�potential�ڴ�
	const bool debug = false; //ע����Ǻ���Ϥ��δ��룬���ܴ򿪿��أ��������������ϵͳ����
	const double PI = 3.1415926;
}

CGPUBestAlgThread::CGPUBestAlgThread() {
	sendPoint = CGeoPoint(0,0);
	_pVision = NULL;
	_PointPotential = new float[GRIDX][GRIDY];
	for (int i = 0;i < 10;i++) { //�˴���10��˵���Կ�10�����򣬺�maxPlayer�޹�
		pointKeeper[i] = CGeoPoint(9999,9999);
		_lastCycle[i] = 0;
	}
	
	halfLength = Param::Field::PITCH_LENGTH / 2;
	std::cout << halfLength << endl;
	halfWidth = Param::Field::PITCH_WIDTH / 2;
	std::cout << halfWidth << endl;
	halfGoalWidth = Param::Field::GOAL_WIDTH / 2;
	std::cout << halfGoalWidth << endl;
}

CGPUBestAlgThread::~CGPUBestAlgThread() {
	delete []_PointPotential;
}

void CGPUBestAlgThread::initialize(CVisionModule* pVision) {
	_pVision = pVision;
	// ���� GPU ������߳�
	_best_calculation_thread = new CThreadCreator(doBestCalculation, 0);
	return ;
}

void CGPUBestAlgThread::setSendPoint(const CGeoPoint passPoint) {
	sendPoint = passPoint;
}

void CGPUBestAlgThread::generatePointValue() {
	// ������GPU��������λ��
	/************************************************************************/
	/* �����㷨���ݴ��룺����λ����Ϣ                                       */
	/************************************************************************/
	// ����
	_best_visiondata_copy_mutex->lock();
	// ����
	for (int i = 0; i < OURPLAYER_NUM; i++) {
		if (_pVision->OurPlayer(i+1).Valid()) {
			C_ourPlayer_Geo[i].setX((float)_pVision->OurPlayer(i+1).Pos().x());
			C_ourPlayer_Geo[i].setY((float)_pVision->OurPlayer(i+1).Pos().y());
		}
		else {
			C_ourPlayer_Geo[i].setX(9999);
			C_ourPlayer_Geo[i].setY(9999);
		}

	}
	for (int i = 0; i < THEIRPLAYER_NUM; i++) {
		if (_pVision->TheirPlayer(i+1).Valid()) {
			C_theirPlayer_Geo[i].setX((float)_pVision->TheirPlayer(i+1).Pos().x());
			C_theirPlayer_Geo[i].setY((float)_pVision->TheirPlayer(i+1).Pos().y());
		} 
		else {
			C_theirPlayer_Geo[i].setX(9999);
			C_theirPlayer_Geo[i].setY(9999);
		}

	}
	C_ball_Geo.setX((float)sendPoint.x());
	C_ball_Geo.setY((float)sendPoint.y());
	// ����
	_best_visiondata_copy_mutex->unlock();
	// ��֧��CUDAʱ��CPU����
	if (NULL == C_PointPotential) {
		C_PointPotential = new float[GRIDX][GRIDY];
	}		
	//����,ע������ĳ������ݣ�����������������ʵ���ز�һ�������ر仯����Ҫ�ֶ��޸ģ�������wang qun
	//ע�⣬���ز�������ȡΪ UN_GPU_PRECISE �������������������е�ѭ�������������� yys
	for (int x = -halfLength ; x <= halfLength; x += UN_GPU_PRECISE) {
		for (int y = -halfWidth; y <= halfWidth; y += UN_GPU_PRECISE) {
			C_PointPotential[x + halfLength][y + halfWidth] = getPointPotential(x,y);
		}
	}
	//����ֵ���������̹߳�������
	_value_getter_mutex->lock();
	for (int x = -halfLength; x <= halfLength; x += UN_GPU_PRECISE) {
		for (int y = -halfWidth; y <= halfWidth; y += UN_GPU_PRECISE) {
			_PointPotential[x + halfLength][y + halfWidth] = C_PointPotential[x + halfLength][y + halfWidth];
		}
	}
	_value_getter_mutex->unlock();
}

CGeoPoint CGPUBestAlgThread::getBestPoint(const CGeoPoint leftUp,const CGeoPoint rightDown, const int pointKeeperNum, const ReCalModeT& reCalMode,bool forceReset) {
	//��ֹ�������
	if (leftUp.x() <= rightDown.x() || leftUp.y() >= rightDown.y()) {
		cout<< "ErrorPoint In GPUBestPointCalc!!!!!   " << leftUp << "  " << rightDown << endl;
		return CGeoPoint(0,0);
	}
	if (Utils::OutOfField(leftUp,1) || Utils::OutOfField(rightDown,1)) {
		cout<< "ErrorPoint In GPUBestPointCalc!!!!!   " << leftUp << "  " << rightDown << endl;
		return CGeoPoint(0,0);
	}
	CGeoPoint bestPoint = leftUp;
	//��ʱ�䲻���ú���������ʷ��¼�����
	if ( _pVision->Cycle() - _lastCycle[pointKeeperNum] > Param::Vision::FRAME_RATE * 0.1 ) pointKeeper[pointKeeperNum] = CGeoPoint(9999,9999);
	//�Ƿ�����ϴ����������ж�
	if (_pVision->Cycle() == _lastCycle[pointKeeperNum]
		&& pointKeeper[pointKeeperNum].x() < leftUp.x() && pointKeeper[pointKeeperNum].x() > rightDown.x()
		&& pointKeeper[pointKeeperNum].y() < rightDown.y() && pointKeeper[pointKeeperNum].y() > leftUp.y())
	{
		return pointKeeper[pointKeeperNum];
	}
	if (false == forceReset && pointKeeper[pointKeeperNum].dist(CGeoPoint(9999,9999)) > 10 && true == isLastOneValid(pointKeeper[pointKeeperNum])
		&& pointKeeper[pointKeeperNum].x() < leftUp.x() && pointKeeper[pointKeeperNum].x() > rightDown.x()
		&& pointKeeper[pointKeeperNum].y() < rightDown.y() && pointKeeper[pointKeeperNum].y() > leftUp.y())
	{ 
		_lastCycle[pointKeeperNum] = _pVision->Cycle();
		return pointKeeper[pointKeeperNum];
	} 
	else {
		float minValue = 1000;
		_value_getter_mutex->lock();		
		for (int x = (int)rightDown.x() + 1;x < (int)leftUp.x() - 1;x++) {
			if ( 0 != x % UN_GPU_PRECISE) {  // �����ز�����Ϊ UN_GPU_PRECISE �ı��������continue������  yys
				continue;
			}
			for (int y = (int)leftUp.y() + 1;y < (int)rightDown.y() - 1;y++) {
				if ( 0 != y % UN_GPU_PRECISE) {
					continue;
				}
				if (_PointPotential[x + halfLength][y + halfWidth] < minValue) {
					minValue = _PointPotential[x + halfLength][y + halfWidth];
					bestPoint = CGeoPoint(x,y);
				}
			}
		}
		_value_getter_mutex->unlock();
	}
	pointKeeper[pointKeeperNum] = bestPoint;
	_lastCycle[pointKeeperNum] = _pVision->Cycle();
	//std::cout << bestPoint << std::endl;
	return bestPoint;
}

bool CGPUBestAlgThread::isLastOneValid(const CGeoPoint& p) {
	CShootRangeList shootRangeList(_pVision, 1, p);
	const CValueRangeList& shootRange = shootRangeList.getShootRange();
	if (shootRange.size() > 0) {
		const CValueRange* bestRange = shootRange.getMaxRangeWidth();
		if (bestRange->getWidth() > Param::Field::BALL_SIZE*2.0) {
			return true;
		}
	}
	return false;
}

double CGPUBestAlgThread::getPosPotential(const CGeoPoint p) {
	return getPointPotential(p.x(),p.y());
}

CThreadCreator::CallBackReturnType THREAD_CALLBACK CGPUBestAlgThread::doBestCalculation(CThreadCreator::CallBackParamType lpParam) {
	while (true) {
		GPUBestAlgThread::Instance()->generatePointValue();
		//Sleep(200);
	}
}

/************************************************************************/
/* ���ۺ�������������                                                   */
/************************************************************************/
void CGPUBestAlgThread::C_evaluateReceive_Geo(const CGeoPoint p, float& reverseReceiveP) {
	float dist_ball2p = p.dist(C_ball_Geo);
	float dir_ball2p = (p - C_ball_Geo).dir();
	float minAngleDiff = 1000;
	//1
	float dist_opp2p = (p - C_theirPlayer_Geo[0]).mod();
	CVector ball2opp = C_theirPlayer_Geo[0] - C_ball_Geo;
	float dist_ball2opp = ball2opp.mod();
	float dir_ball2opp = ball2opp.dir();
	float absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;// ���Դ��ĽǶȿյ����루��Ҫ���۲�����
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//2
	dist_opp2p = (p - C_theirPlayer_Geo[1]).mod();
	ball2opp = C_theirPlayer_Geo[1] - C_ball_Geo;
	dist_ball2opp = ball2opp.mod();
	dir_ball2opp = ball2opp.dir();			
	absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//3
	dist_opp2p = (p - C_theirPlayer_Geo[2]).mod();
	ball2opp = C_theirPlayer_Geo[2] - C_ball_Geo;
	dist_ball2opp = ball2opp.mod();
	dir_ball2opp = ball2opp.dir();			
	absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//4
	dist_opp2p = (p - C_theirPlayer_Geo[3]).mod();
	ball2opp = C_theirPlayer_Geo[3] - C_ball_Geo;
	dist_ball2opp = ball2opp.mod();
	dir_ball2opp = ball2opp.dir();			
	absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//5
	dist_opp2p = (p - C_theirPlayer_Geo[4]).mod();
	ball2opp = C_theirPlayer_Geo[4] - C_ball_Geo;
	dist_ball2opp = ball2opp.mod();
	dir_ball2opp = ball2opp.dir();			
	absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//6
	dist_opp2p = (p - C_theirPlayer_Geo[5]).mod();
	ball2opp = C_theirPlayer_Geo[5] - C_ball_Geo;
	dist_ball2opp = ball2opp.mod();
	dir_ball2opp = ball2opp.dir();			
	absDirDiff = fabs(Utils::Normalize(dir_ball2opp - dir_ball2p));
	if (absDirDiff < minAngleDiff && dist_ball2opp < dist_ball2p) {
		minAngleDiff = absDirDiff;
	}
	reverseReceiveP += 50.0 / (dist_opp2p + 0.1);
	//��Ҫ�赲�Լ�������
	if (debug) {
		std::cout << "receive is " << reverseReceiveP << std::endl;
	}
	CGeoLine temp_line(C_ball_Geo, CGeoPoint(halfLength, 0));
	float blockMyself = p.dist(temp_line.projection(p));
	if (blockMyself <= 50) {
		reverseReceiveP += C_POINT_PASS_OFF;
	} 
	else reverseReceiveP += 20.0 / (blockMyself + 0.1);
	if (debug) {
		std::cout << "block self angle is " << 20.0 / (blockMyself + 0.1) << std::endl;
	}
	reverseReceiveP += 3.0/(minAngleDiff + 0.01); // ������赲��,����0.001Ϊ��֤������Ϊ0
	if (debug) {
		std::cout << "block angle is " << 3.0 / (minAngleDiff + 0.01) << std::endl;
	}
}
/************************************************************************/
/*���ŽǶȼ���                                                          */
/************************************************************************/
inline float C_GetDirRange(const C_blockDirection& D){return (D.leftDist <= D.rightDist ? D.leftDist : D.rightDist)*fabs(D.rightDir - D.leftDir);} //�˴���ʱ����fabs()��������û�����ȥ��
inline float C_GetDirMiddle(const C_blockDirection& D){return (D.leftDir + D.rightDir)/2.0;}
//���ŽǶȼ��㺯��
C_shootParam CGPUBestAlgThread::C_shootRange_Geo(const CGeoPoint p){
	C_shootParam sParam;
	CGeoPoint blockOpp[6];
	int blockOppNum = 0; //��C_theirPlayer�Ĺ�ϵ��i-1
	//p��������������ز���
	const CGeoPoint goalPoint(halfLength, 0);
	const CGeoPoint goalLeftPoint(halfLength, -halfGoalWidth);
	const CGeoPoint goalRightPoint(halfLength, halfGoalWidth);
	const CGeoLine goalLeftLine(p, goalLeftPoint);
	const CGeoLine goalRightLine(p, goalRightPoint);
	const float leftAngle = (goalLeftPoint - p).dir();
	const float rightAngle = (goalRightPoint - p).dir();
	//�������赲��Χ��С��λ��д������blockOpp[]
	//1
	CGeoPoint opp = C_theirPlayer_Geo[0];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//2
	opp = C_theirPlayer_Geo[1];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//3
	opp = C_theirPlayer_Geo[2];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//4
	opp = C_theirPlayer_Geo[3];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//5
	opp = C_theirPlayer_Geo[4];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//6
	opp = C_theirPlayer_Geo[5];
	if(p.x() - 10 < opp.x() && Utils::dirDiff((opp - p), (goalPoint - p)) < PI*2/3.0 && Utils::dirDiff((p - goalPoint), (opp - goalPoint)) < PI*2/3.0) {
		blockOpp[blockOppNum] = opp;
		blockOppNum++;
	}
	//��Ч��Ե��������
	float activeEdge[14];
	float activeDist[14];
	memset(activeEdge,0,14*sizeof(float));
	memset(activeDist,0,14*sizeof(float));
	//for(int i=0;i<14;i++){
	//	activeedge[i]=0;
	//	activedist[i]=0;
	//}


	int activeEdgeCount = 0;
	//����Ч��Ե�߼��뵽��Ч��Ե��������
	//�����������
	int leftCount = blockOppNum;
	for (;leftCount > 0;leftCount--) { //ע��i��ӦblockOpp[i-1]
		if (Utils::pointToLineDist(blockOpp[leftCount - 1], goalLeftLine) < 9) {
			break;
		}
	}
	if (0 == leftCount) {
		activeEdge[activeEdgeCount] = leftAngle;
		activeDist[activeEdgeCount] = p.dist(goalLeftPoint);
		activeEdgeCount++;
	}
	//�������ұ���
	int rightCount = blockOppNum;
	for (;rightCount > 0;rightCount--) {
		if (Utils::pointToLineDist(blockOpp[rightCount - 1], goalRightLine) < 9) {
			break;
		}
	}
	if (0 == rightCount) {
		activeEdge[activeEdgeCount] = rightAngle;
		activeDist[activeEdgeCount] = p.dist(goalRightPoint);
		activeEdgeCount++;
	}
	int useBlockOppNum = blockOppNum;
	for (;useBlockOppNum > 0;useBlockOppNum--) { //��useBlockOppNum��������Ե���ж�
		CVector p2opp = blockOpp[useBlockOppNum-1] - p;
		float maxSide = p2opp.mod();
		float derta = asinf(9.0/maxSide); 
		//���Ե
		float oppLeftBlockAngle = p2opp.dir() - derta;
		CGeoLine oppLeftBlockLine(p, oppLeftBlockAngle);
		int oppLeftCount = blockOppNum;
		for (;oppLeftCount > 0;oppLeftCount--) {
			if (oppLeftCount == useBlockOppNum) {
				continue;
			}
			if (Utils::pointToLineDist(blockOpp[oppLeftCount - 1], oppLeftBlockLine) < 9) {
				break;
			}
		}
		if (0 == oppLeftCount && true == Utils::CBetween(oppLeftBlockAngle,leftAngle,rightAngle)) {
			activeEdge[activeEdgeCount] = oppLeftBlockAngle;
			activeDist[activeEdgeCount] = p2opp.mod();
			activeEdgeCount++;
		}
		//�ұ�Ե
		float oppRightBlockAngle = p2opp.dir() + derta; 
		CGeoLine oppRightBlockLine(p,oppRightBlockAngle);
		int oppRightCount = blockOppNum;
		for (;oppRightCount > 0;oppRightCount--) {
			if (oppRightCount == useBlockOppNum) {
				continue;
			}
			if (Utils::pointToLineDist(blockOpp[oppRightCount-1], oppRightBlockLine) < 9) {
				break;
			}
		}
		if (0 == oppRightCount && true == Utils::CBetween(oppRightBlockAngle,leftAngle,rightAngle)) {
			activeEdge[activeEdgeCount] = oppRightBlockAngle;
			activeDist[activeEdgeCount] = p2opp.mod();
			activeEdgeCount++;
		}
	}
	//���û�����ŽǶ�,ֱ�ӷ���
	if (0 == activeEdgeCount) {
		sParam.biggestAngle = 0;
		sParam.bestDir = 0;
		return sParam;
	}
	//��Ч��Ե����ð������
	float temp;
	for(int i = activeEdgeCount - 1;i > 0;i--) {
		for(int j = 0;j < i;j++) {
			if(activeEdge[j] > activeEdge[j+1]) {
				temp = activeEdge[j];
				activeEdge[j] = activeEdge[j+1];
				activeEdge[j+1] = temp;
				temp = activeDist[j];
				activeDist[j] = activeDist[j+1];
				activeDist[j+1] = temp;
			}
		}
	}
	//�����赲����ṹ��
	C_blockDirection blockDir[7];
	memset(blockDir,0,sizeof(C_blockDirection)*7);
	int blockDirCount = activeEdgeCount/2;
	for (int i = 0;i < blockDirCount;i++) {
		blockDir[i].leftDir = activeEdge[i*2];
		blockDir[i].leftDist = activeDist[i*2];
		blockDir[i].rightDir = activeEdge[i*2+1];
		blockDir[i].rightDist = activeDist[i*2+1];
	}
	//ð���������ŽǶ�,�������ŵ���ǰ
	C_blockDirection temp2;
	for(int i = blockDirCount - 1;i > 0;i--) {
		if( C_GetDirRange(blockDir[i]) > C_GetDirRange(blockDir[i-1])) {
			temp2 = blockDir[i];
			blockDir[i] = blockDir[i-1];
			blockDir[i-1] = temp2;
		}
	}
	//ȡ�������ŽǶȼ�������
	sParam.biggestAngle = C_GetDirRange(blockDir[0]);
	sParam.bestDir = C_GetDirMiddle(blockDir[0]);
	if (debug) {
		std::cout << "the biggestAngle is" << sParam.biggestAngle << std::endl;
		CVector m(1000, blockDir[0].leftDir);
		GDebugEngine::Instance()->gui_debug_line(CGeoPoint(p.x(),p.y()),CGeoPoint(p.x(),p.y()) + CVector(m.x(),m.y()),COLOR_RED);
		m = CVector(1000, blockDir[0].rightDir);
		GDebugEngine::Instance()->gui_debug_line(CGeoPoint(p.x(),p.y()),CGeoPoint(p.x(),p.y()) + CVector(m.x(),m.y()),COLOR_RED);
	}
	return sParam;                                 
}
/************************************************************************/
/* ���ۺ��������Ų���                                                   */
/************************************************************************/
void CGPUBestAlgThread::C_evaluateShoot_Geo(const CGeoPoint p, float& reverseShootP) {
	C_shootParam paramP = C_shootRange_Geo(p);
	//���ſյ�
	reverseShootP += 200.0/(paramP.biggestAngle + 0.1);// ϵ��0.1������,����0.001Ϊ��֤������Ϊ0
	if (debug) {
		std::cout<< "kong dang is " << reverseShootP << std::endl;
	}
	//���Ŷ�Ա�������ŵĵ����Ƕ�
	float shootDirDiff = fabs(Utils::Normalize(paramP.bestDir - (C_ball_Geo - p).dir()));
	reverseShootP += 1.5*shootDirDiff*shootDirDiff;
	if (debug) {
		std::cout << "tiaozhengjiao is " << 1.5 * shootDirDiff * shootDirDiff << std::endl << std::endl << std::endl;
	}
}

/************************************************************************/
/* ���ۺ�������������                                                   */
/************************************************************************/
void CGPUBestAlgThread::C_evaluateOthers_Geo(const CGeoPoint p, float& reverseOtherP) {
	if (p.dist(C_ball_Geo) < 150) { // ���뿪���һ������֮��
		reverseOtherP += C_POINT_PASS_OFF;
	}
}
/************************************************************************/
/************************************************************************/
float CGPUBestAlgThread::getPointPotential(int x,int y) {
	//Ҫ���۵ĵ�
	CGeoPoint evaluatePoint_Geo(x, y);
	//��ʼ����������ֵ	
	float reverseReceiveP_Geo = 0;
	float reverseShootP_Geo = 0;
	float reverseOtherP_Geo = 0;
	float reversePotential_Geo = 0;
	//���ۺ���
	C_evaluateReceive_Geo(evaluatePoint_Geo, reverseReceiveP_Geo);
	C_evaluateShoot_Geo(evaluatePoint_Geo, reverseShootP_Geo);
	C_evaluateOthers_Geo(evaluatePoint_Geo, reverseOtherP_Geo);
	reversePotential_Geo = reverseReceiveP_Geo + reverseShootP_Geo + reverseOtherP_Geo;
	return reversePotential_Geo;
}