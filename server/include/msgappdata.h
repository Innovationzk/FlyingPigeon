#pragma once
#include <map>
#include "../../common/osp.h"
#include "../../common/kdvtype.h"
#include "../../common/json/json.h"
#include "../../common/commondef.h"

class MsgAppData
{
public:
	struct ClientInfo
	{
		std::string name;
		u32 node;
	};

	static int clientNo; // 所有客户端唯一标识，编号从1开始，其数值代表下一个注册的客户端的编号

	MsgAppData();
	~MsgAppData();

	// 新客户端注册
	CONNECT_RESULT RegisterNewClient(const char* name, u32 node);
	// 客户端登录，更新在线客户端列表
	CONNECT_RESULT AddOnlineClient(const char* name, u32 node);
	// 客户端断开连接，更新在线客户端列表
	void RemoveOnlineClient(u32 clientNode);
	// 添加等待发送的离线消息
	void AddOfflineMessage(u32 srcClientNo, u32 dstClientNo, const char* msgContent);
	// 获取目标客户端的离线消息
	Json::Value getOfflineMessage(u32 clientNode);
	// 检查客户端是否在线
	bool isOnlineClient(u32 clientNo);
	// 根据客户端节点号获取其编号
	u32 getOnlineClientNo(u32 clientNode);
	// 根据客户端编号号获取其节点
	u32 getOnlineClientNode(u32 clientNo);

	std::map<u32, ClientInfo> getOnlineClientList();
	Json::Value getClientList();

private:
	std::map<u32, ClientInfo> m_onlineClientList;   // 在线客户端列表
	Json::Value m_clientList;       // 所有注册过的客户端列表
	Json::Value m_offlineMessage;   // 等待发送的离线消息列表
};