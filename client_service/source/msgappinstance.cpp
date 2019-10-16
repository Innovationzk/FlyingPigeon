#include <msgappinstance.h>
#include <msgappdata.h>
#include <commondef.h>

/****************TODO:正式发布版需要修改******************
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
********************************************************/

extern int serverNode;
extern zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_CLIENT, MsgAppData, MAX_ALIAS_LENGTH_CLIENT> msgApp;
typedef void (*CallBack)(u16 eventType, const void* content, u16 length);
extern CallBack CallBackFunc;

MsgAppInstance::MsgAppInstance()
{
}

MsgAppInstance::~MsgAppInstance()
{
}

void MsgAppInstance::DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, eventType is:%d\n", __FUNCTION__, pMsg->event);

    switch (pMsg->event)
    {
    case EV_UI_CLT_REGIST_REQ:
    {
        // 向服务端发送注册请求
        Regist(pMsg);
        break;
    }
    case EV_SER_CLT_REGIST_ACK:
    {
        // 服务端确认注册，通知UI
        OspPrintf(TRUE, FALSE, "[%s]: registration success\n", __FUNCTION__);
        clt_ui_connect_result_ntf registResult;
        registResult.result = CONNECTION_SUCCEED;
        CallBackFunc(EV_CLT_UI_CONNECT_RESULT_NTF, &registResult, sizeof(registResult));
        //通知fileApp连接成功
        post(MAKEIID(FILE_APP_ID_CLIENT, CInstance::DAEMON), EV_CLT_CLT_CONNECT_SUCCEED_NTF, NULL, 0, 0);
        break;
    }
    case EV_SER_CLT_REGIST_NACK:
    {
        // 服务端拒绝注册，通知UI
        ser_clt_regist_nack *nackMsg = (ser_clt_regist_nack*)(pMsg->content);
        if (OCCUPIED_USERNAME == nackMsg->result)  // 用户名被占用
        {
            OspPrintf(TRUE, FALSE, "[%s]: registration failed, username is occupied\n", __FUNCTION__);
            CallBackFunc(EV_CLT_UI_CONNECT_RESULT_NTF, pMsg->content, pMsg->length);
        }
        else if (CLIENT_NUMBER_LIMIT == nackMsg->result)  // 在线客户端已满
        {
            OspPrintf(TRUE, FALSE, "[%s]: registration failed, the number of online clients has reached the maximum\n", __FUNCTION__);
            CallBackFunc(EV_CLT_UI_CONNECT_RESULT_NTF, pMsg->content, pMsg->length);
        }
        break;
    }
    case EV_UI_CLT_LOG_IN_REQ:
    {
        // 向服务端发送登录请求
        LogIn(pMsg);
        break;
    }
    case EV_SER_CLT_LOG_IN_ACK:
    {
        // 服务端确认登录，通知UI
        OspPrintf(TRUE, FALSE, "[%s]: log in succeed\n", __FUNCTION__);
        clt_ui_connect_result_ntf registResult;
        registResult.result = CONNECTION_SUCCEED;
        CallBackFunc(EV_CLT_UI_CONNECT_RESULT_NTF, &registResult, sizeof(registResult));
        //通知fileApp连接成功
        post(MAKEIID(FILE_APP_ID_CLIENT, CInstance::DAEMON), EV_CLT_CLT_CONNECT_SUCCEED_NTF, NULL, 0, 0);
        break;
    }
    case EV_SER_CLT_LOG_IN_NACK:
    {
        // 服务端拒绝登录，通知UI
        ser_clt_log_in_nack *nackMsg = (ser_clt_log_in_nack*)(pMsg->content);
        if (CLIENT_NUMBER_LIMIT == nackMsg->result)  // 在线客户端已满
        {
            OspPrintf(TRUE, FALSE, "[%s]: log in failed, the number of online clients has reached the maximum\n", __FUNCTION__);
            CallBackFunc(EV_CLT_UI_CONNECT_RESULT_NTF, pMsg->content, pMsg->length);
        }
        break;
    }
    case EV_SER_CLT_REGIST_LIST_NTF:
        // 服务端通知更新在线客户端列表
        OspPrintf(TRUE, FALSE, "[%s]: notify UI to update client list\n", __FUNCTION__);

#ifdef _DEBUG
        {
            ser_clt_regist_list_ntf* clientList = (ser_clt_regist_list_ntf*)(pMsg->content);
            OspPrintf(TRUE, FALSE, "[%s]: online client number:%d\n", __FUNCTION__, clientList->clientNum);
            s8* clientName = clientList->clientName;
            for (int i = 0; i < clientList->clientNum; i++)
            {
                OspPrintf(TRUE, FALSE, "[%s]: %d\t%s\n", __FUNCTION__, clientList->clientNo[i], clientName);
                while (*(++clientName) != '\0');
                ++clientName;
            }   
        }
#endif // _DEBUG

        CallBackFunc(EV_CLT_UI_REGIST_LIST_NTF, pMsg->content, pMsg->length);
        break;
    case EV_UI_CLT_POST_MSG_REQ:
    {
        // 向其他客户端发送消息
        SendMsg(pMsg);
        break;
    }
    case EV_SER_CLT_POST_MSG_NTF:
    {
        // 收到其他客户端的消息
        ser_clt_post_msg_ntf* msg = (ser_clt_post_msg_ntf*)(pMsg->content);
        OspPrintf(TRUE, FALSE, "[%s]: receive msg,srcClientNo:%d,msgContent:%s\n", __FUNCTION__, msg->srcClientNo, msg->msgContent);
        // 通知UI
        CallBackFunc(EV_CLT_UI_RECEIVE_MSG_NTF, pMsg->content, pMsg->length);
        break;
    }
    case EV_CLT_CLT_RECONNECT_NTF:
    {
        // 断开时超时重连
        ReconnectServer();
        break;
    }
    case OSP_DISCONNECT:
    {
        // 收到断开通知,通知UI
        CallBackFunc(EV_CLT_UI_DISC_NTF, NULL, 0);
        // 定时重连
        OspPrintf(TRUE, FALSE, "[%s]: server node discconnect,will reconnect after %dms\n", __FUNCTION__, RECONNECT_INTERVAL);
        serverNode = INVALID_NODE;
        SetTimer(EV_CLT_CLT_RECONNECT_NTF, RECONNECT_INTERVAL);
        break;
    }
    default:
        break;
    }
}

void MsgAppInstance::Regist(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
    msgApp.setUserName((char*)(pMsg->content));

    ConnectServer();
    if (INVALID_NODE == serverNode)  // 服务器离线
    {
        OspPrintf(TRUE, FALSE, "[%s]: notify UI server offine,send regist_nack\n", __FUNCTION__);
        CallBackFunc(EV_CLT_UI_REGIST_NACK, NULL, 0);
    }
    else  // 服务器在线
    {
        OspPrintf(TRUE, FALSE, "[%s]: notify UI server online,send regist_ack\n", __FUNCTION__);
        CallBackFunc(EV_CLT_UI_REGIST_ACK, NULL, 0);
        // 发送注册请求
        OspPrintf(TRUE, FALSE, "[%s]: send regist REQ to server\n", __FUNCTION__);
        std::string name = msgApp.getUserName();
        post(MAKEIID(MSG_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_REGIST_REQ, name.c_str(), name.size() + 1, serverNode);
    }
}

void MsgAppInstance::LogIn(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,userName is:%s\n", __FUNCTION__, (char*)(pMsg->content));
    msgApp.setUserName((char*)(pMsg->content));

    ConnectServer();
    if (INVALID_NODE == serverNode)  // 服务器离线
    {
        CallBackFunc(EV_CLT_UI_LOG_IN_NACK, NULL, 0);
    }
    else  // 服务器在线
    {
        CallBackFunc(EV_CLT_UI_LOG_IN_ACK, NULL, 0);
        // 发送注册请求
        OspPrintf(TRUE, FALSE, "[%s]: send log in REQ to server\n", __FUNCTION__);
        std::string name = msgApp.getUserName();
        post(MAKEIID(MSG_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_LOG_IN_REQ, name.c_str(), name.size() + 1, serverNode);
    }
}

void MsgAppInstance::ReconnectServer()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
    ConnectServer();
    if (INVALID_NODE != serverNode)  // 连接成功
    {
        // 发送登录请求
        OspPrintf(TRUE, FALSE, "[%s]: send log in REQ to server\n", __FUNCTION__);
        std::string name = msgApp.getUserName();
        post(MAKEIID(MSG_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_LOG_IN_REQ, name.c_str(), name.size() + 1, serverNode);
    }
}

void MsgAppInstance::SendMsg(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,msg content is:%s\n", __FUNCTION__, ((clt_ser_post_msg_ntf*)(pMsg->content))->msgContent);

    if (TRUE == OspIsValidTcpNode(serverNode))  // 服务器在线，发送消息
    {
        OspPrintf(TRUE, FALSE, "[%s]: send msg to server\n", __FUNCTION__);
        clt_ser_post_msg_ntf* srcMsg = (clt_ser_post_msg_ntf*)(pMsg->content);
        OspPrintf(TRUE, FALSE, "[%s]: target clients number is:%d,msg content is:%s\n", __FUNCTION__, srcMsg->clientNum, srcMsg->msgContent);
        CallBackFunc(EV_CLT_UI_POST_MSG_ACK, NULL, 0);
        post(MAKEIID(MSG_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_POST_MSG_NTF, pMsg->content, pMsg->length,serverNode);
    }
    else  // 服务器离线，拒绝发送
    {
        OspPrintf(TRUE, FALSE, "[%s]: server offline,refuse to send\n", __FUNCTION__);
        CallBackFunc(EV_CLT_UI_POST_MSG_NACK, NULL, 0);
    }
}

void MsgAppInstance::ConnectServer()
{
    OspPrintf(TRUE, FALSE, "[%s]: called,serverNode is:%d\n", __FUNCTION__, serverNode);

    if (TRUE == OspIsValidTcpNode(serverNode))
    {
        OspPrintf(TRUE, FALSE, "[%s]: server online\n", __FUNCTION__);
        return;
    }

    serverNode = OspConnectTcpNode(IPV4_ADDRESS_SERVER, LISTEN_PORT_SERVER, 10, 3, 1000);  // 连接服务器
    if (INVALID_NODE != serverNode)  // 连接成功
    {
        OspPrintf(TRUE, FALSE, "[%s]: Successfully connect to the server,serverNode is:%d\n", __FUNCTION__, serverNode);
        OspSetHBParam(serverNode, 10, 3);
        KillTimer(EV_CLT_CLT_RECONNECT_NTF);

        // 断开时给fileApp和msgApp发送通知
        OspNodeDiscCBReg(serverNode, MSG_APP_ID_CLIENT, CInstance::DAEMON);
        OspNodeDiscCBRegQ(serverNode, FILE_APP_ID_CLIENT, CInstance::DAEMON);
    }
    else    // 连接失败，设置定时器重新连接
    {
        OspPrintf(TRUE, FALSE, "[%s]: Connect server failed,set timer to reconnect\n", __FUNCTION__);
        SetTimer(EV_CLT_CLT_RECONNECT_NTF, RECONNECT_INTERVAL);
    }
}

void MsgAppInstance::InstanceEntry(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, eventType is:%d\n", __FUNCTION__, pMsg->event);
}
