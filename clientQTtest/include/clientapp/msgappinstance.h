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

	// DaemonInstance处理逻辑
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
    // 客户端注册
    void Regist(CMessage* const pMsg);
    // 客户端登录
    void LogIn(CMessage* const pMsg);
    // 断开后重新连接服务器
    void ReconnectServer();
    // 发送消息
    void SendMsg(CMessage* const pMsg);
    // 连接Server
    void ConnectServer();

	// 普通Instanc处理逻辑
	void InstanceEntry(CMessage* const pMsg);

private:

};