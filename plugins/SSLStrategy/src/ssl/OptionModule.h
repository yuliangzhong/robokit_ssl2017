#ifndef _OPTION_MODULE_H_
#define _OPTION_MODULE_H_
/**
* COptionModule.
* һЩ��ʼ������
*/
class COptionModule{
public:
	COptionModule();
	~COptionModule();
	int MySide() const { return _side; }
	int MyColor() const { return _color; }
private:
	int _side; // ���������ڵı�
	int _color; // �ҷ���ɫ,RefereeBox�õ�
};
#endif
