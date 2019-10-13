#pragma once
#include "../../common/osp.h"
#include "../../common/kdvtype.h"
#include "../../common/commondef.h"

class MsgAppInstance :public CInstance
{
public:
    MsgAppInstance();
    ~MsgAppInstance();

    // DaemonInstance�����߼�
    void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
    // ���ͻ����б�֪ͨ���������߿ͻ���
    void NotifyOnlineClientList();
    // ������Ϣ���ͻ���
    void SendMsg(CMessage* const pMsg);

    // ��ͨInstanc�����߼�
    void InstanceEntry(CMessage* const pMsg);

private:

};