#pragma once
#include "../../common/osp.h"
#include "../../common/kdvtype.h"
#include "../../common/commondef.h"

class MsgAppInstance :public CInstance
{
public:
    MsgAppInstance();
    ~MsgAppInstance();

    // DaemonInstance处理逻辑
    void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
    // 将客户端列表通知给所有在线客户端
    void NotifyOnlineClientList();
    // 发送消息给客户端
    void SendMsg(CMessage* const pMsg);

    // 普通Instanc处理逻辑
    void InstanceEntry(CMessage* const pMsg);

private:

};