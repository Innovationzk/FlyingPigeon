#pragma once
#include <osp.h>
#include <kdvtype.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

class MsgAppInstance :public CInstance
{
public:
	MsgAppInstance();
	~MsgAppInstance();

	// DaemonInstance�����߼�
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
    // �ͻ���ע��
    void Regist(CMessage* const pMsg);
    // �ͻ��˵�¼
    void LogIn(CMessage* const pMsg);
    // �Ͽ����������ӷ�����
    void ReconnectServer();
    // ������Ϣ
    void SendMsg(CMessage* const pMsg);
    // ����Server
    void ConnectServer();

	// ��ͨInstanc�����߼�
	void InstanceEntry(CMessage* const pMsg);

private:

};