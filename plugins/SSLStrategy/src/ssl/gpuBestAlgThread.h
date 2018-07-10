/************************************************************************/
/* Copyright (c) CSC-RL, Zhejiang University							*/
/* Team��		SSL-ZJUNlict											*/
/* HomePage:	http://www.nlict.zju.edu.cn/ssl/WelcomePage.html		*/
/************************************************************************/
/* File:	  gpuBestAlgThread.h										*/
/* Func:	  ���������λ����̣߳�֧��CPUģʽ��GPUģʽ				*/
/* Author:	  ��Ⱥ 2012-08-18											*/
/*            Ҷ���� 2014-03-01                                         */
/* Refer:	  ###														*/
/* E-mail:	  wangqun1234@zju.edu.cn									*/
/* Version:	  0.0.1														*/
/************************************************************************/

#ifndef _GPU_BEST_ALG_THREAD_H
#define _GPU_BEST_ALG_THREAD_H

#include <fstream>
#include <MultiThread.h>
#include <singleton.h>
#include "VisionModule.h"
#include "geometry.h"

#define GRIDX 910
#define GRIDY 610
#define  C_POINT_PASS_OFF 200
#define  C_MAX_PLAYER		6

enum ReCalModeT {
	CIRCLE_MODE = 0,
	RECT_MODE,
};
/**
@brief    ���������λ����̣߳�֧��CPUģʽ��GPUģʽ
@details  �����Ƕ��������̵߳��㷨�̣߳��ṩȫ��������������λ��ļ��㡣ȫ�ֱ���USE_GPU
Ϊ1ʱʹ��GPU������λ���㣬Ϊ0ʱʹ��CPU������λ���㡣Ϊ����ÿ���������λ������ܹ������ȶ���
���һ���̶��Ļ��߱䶯��С��������Ҫ��ȡ��ʱע��һ���̶��������š�
@note	ע�⣬��ÿһ֡���У���ʹ�ö���ӿ�getBestPoint����ǰ��Ҫ����һ��setSendPoint�������趨
����ĵ㡣һ��˴�Ϊ��ǰ���ڵĵ�*/

//C_shootRange����ֵ�ṹ
typedef struct{
	float biggestAngle;
	float bestDir;
} C_shootParam;
//�赲����ṹ�壬���ڱ�������
typedef struct{
	float leftDir;
	float rightDir;
	float leftDist;
	float rightDist;
} C_blockDirection;

class CGPUBestAlgThread {
public:
	CGPUBestAlgThread();
	~CGPUBestAlgThread();

public:
	/**
	@brief	ע���Ӿ���Ϣ���ҿ����㷨�߳�*/
	void initialize(CVisionModule* _pVision);
	/**
	@brief	��ģ��Ķ���ӿ�
	@param	leftUp ȡ����������ϵ�(x�������ҷ����ֱ������ϵ��)
	@param	rightDown ȡ����������µ�(x�������ҷ����ֱ������ϵ��)
	@param	pointKeeperNum ȡ�㱣��ϵͳ�ı�ţ���ǰϵͳ��ʶ��������0~9
	@param	reCalMode ȡ������Ϊ���λ���Բ�Ρ���ǰϵͳֻ֧�־��Σ����˴�����RECT_MODE
	@param	forceReset ÿ�����¼��������λ�㣬Ĭ��Ϊfalse������true������ʹ��ȡ�㱣��ϵͳ*/
	CGeoPoint getBestPoint(const CGeoPoint leftUp = CGeoPoint(300,-200),const CGeoPoint rightDown = CGeoPoint(0,200), const int pointKeeperNum = 0, const ReCalModeT& reCalMode = RECT_MODE, bool forceReset = false);//pointKeeper֧��ͬʱ�������ѵ�ʱ����ѵ㱣�ֹ���
	
	/**
	@brief ȫ�����е������ֵ������*/	
	void generatePointValue();
	// ��� xx ��
	// ...

	/**
	@brief	�ж�ĳһ����ǰһ֡�������λ���Ƿ�����Ч*/
	bool isLastOneValid(const CGeoPoint& p);
	/**
	@brief	�趨�����
	@param	passPoint ��ǰ�����*/
	void setSendPoint(const CGeoPoint passPoint);
	/**
	@brief	ȡ�������ֵ�Ķ���ӿ�
	@param	p Ҫ��ȡ����ֵ�ĵ�*/
	double getPosPotential(const CGeoPoint p);

	// ����ΪһЩ���ۺ��� yys
	void C_evaluateReceive_Geo(const CGeoPoint p, float& reverseReceiveP);

	C_shootParam C_shootRange_Geo(const CGeoPoint p);

	void C_evaluateShoot_Geo(const CGeoPoint p, float& reverseShootP);

	void C_evaluateOthers_Geo(const CGeoPoint p, float& reverseOtherP);

	float getPointPotential(int x,int y);

public:
	/**
	@brief	�����߳����õĺ���*/
	static CThreadCreator::CallBackReturnType THREAD_CALLBACK doBestCalculation(CThreadCreator::CallBackParamType lpParam);

private:
	float (*_PointPotential)[GRIDY];	///<����ֵ����
	CVisionModule* _pVision;			///<ͼ��ָ��
	CGeoPoint pointKeeper[10];			///<ȡ�㱣������
	int _lastCycle[10];					///<ȡ�㱣��ϵͳ�õ���ǰһ֡֡��

	CGeoPoint sendPoint;				///<����㣬һ��Ϊ�����ڵĵ�

	int halfLength; // ���ذ볤
	int halfWidth;  // ���ذ��
	int halfGoalWidth;
};

typedef NormalSingleton< CGPUBestAlgThread > GPUBestAlgThread;
#endif