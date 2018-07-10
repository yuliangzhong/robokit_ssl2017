#ifndef __OPP_PLAYER_H__
#define __OPP_PLAYER_H__
//���ֶ�Ա
#include <iostream>
#include <string>
#include "OppRoleMatcher.h"
using namespace std;

class CVisionModule;
class CAttributeSet;
class COppRole;

class COppPlayer
{
public:
	COppPlayer();
	~COppPlayer();
	//initialize,����attrSetΪģ��
	void setProperty(const int num,const CAttributeSet& attrSet);//��ÿһ��playerӦ�øú�������������ʼ�����߶�̬�������Լ�

	//calc function
	void evaluate(const CVisionModule* pVision);		//���Լ���
	void matchRole();			//ƥ���ɫ,Ҫ�ȵ���calcThreaten����

	//return parameters
	int getNum(){return _num;}
	int getThreatenPri();
	double getThreatenValue();
	string getRoleName();
	double getAttributeValue(string name);
	//skill call
	bool isTheRole(const string name);

	friend COppRole* COppRoleMatcher::matchRole(COppPlayer&);
	//friend class COppRoleMatcher;
private:
	int _num;							//����
	CAttributeSet* _attrSetP;	//���Լ�,���ڼ�����в�ȡ�ƥ���ɫ
	COppRole* _oppRole;		//��ɫ������ѡ��Ӧ�Է���
};




#endif