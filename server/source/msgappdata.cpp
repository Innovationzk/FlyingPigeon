#include <fstream>
#include <string>
#include <vector>
#include "../include/msgappdata.h"

/****************TODO:正式发布版需要修改******************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

int MsgAppData::clientNo = 1;

MsgAppData::MsgAppData()
{
	// 读取本地保存的客户端列表
	std::fstream clientListFile("localClientList.json", std::ios::in | std::ios::binary);
	if (clientListFile.is_open())
	{
		Json::Reader reader;
		reader.parse(clientListFile, m_clientList);
		clientNo = m_clientList.size()+1;
		clientListFile.close();
	}

	// 读取离线消息列表
	std::fstream offlineMessageFile("offlineMessage.json", std::ios::in | std::ios::binary);
	if (offlineMessageFile.is_open())
	{
		Json::Reader reader;
		reader.parse(offlineMessageFile, m_offlineMessage);
		clientListFile.close();
	}
}

MsgAppData::~MsgAppData()
{
}

CONNECT_RESULT MsgAppData::RegisterNewClient(const char* name, u32 node)
{
	// 如果达到最大App数或用户名被占用
	if (m_clientList.isMember(name))
	{
		return OCCUPIED_USERNAME;
	}
	else if (m_onlineClientList.size() >= MAX_ONLINE_CLIENT_NUM)
	{
		return CLIENT_NUMBER_LIMIT;
	}

	// 给客户端分配编号并保存其信息
	ClientInfo clientInfo;
	clientInfo.name = name;
	clientInfo.node = node;

	OspPrintf(TRUE, FALSE, "[%s]: new client %s regist,clientNo is:%d\n", __FUNCTION__, name, clientNo);
	m_onlineClientList[clientNo] = clientInfo;
	m_clientList[clientInfo.name] = clientNo;

	// 将新的客户端列表保存至本地文件
	std::fstream clientListFile("localClientList.json", std::ios::out | std::ios::binary);
	if (clientListFile.is_open())
	{
		Json::FastWriter fw;
		std::string content = fw.write(m_clientList);
		clientListFile.write(content.c_str(), content.size());
		clientListFile.close();
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[%s]: Failed to create file:localClientList.json\n", __FUNCTION__);
	}

	clientNo++;

	return CONNECTION_SUCCEED;
}

CONNECT_RESULT MsgAppData::AddOnlineClient(const char* name, u32 node)
{
	// 如果达到最大App数
	if (m_onlineClientList.size() >= MAX_ONLINE_CLIENT_NUM)
	{
		return CLIENT_NUMBER_LIMIT;
	}

	ClientInfo clientInfo;
	clientInfo.name = name;
	clientInfo.node = node;

	if (m_clientList.isMember(clientInfo.name))  // 客户端已经注册过，读取本地文件信息
	{
		OspPrintf(TRUE, FALSE, "[%s]: client %s reconnect,clientNo is:%d\n", __FUNCTION__, name, m_clientList[name].asInt());

		m_onlineClientList[m_clientList[clientInfo.name].asInt()] = clientInfo;
	}
	else  // 客户端第一次注册，保存信息至本地
	{
		OspPrintf(TRUE, FALSE, "[%s]: new client %s regist,clientNo is:%d\n", __FUNCTION__, name, clientNo);
		m_onlineClientList[clientNo] = clientInfo;
		m_clientList[clientInfo.name] = clientNo;

		std::fstream clientListFile("localClientList.json", std::ios::out | std::ios::binary);
		if (clientListFile.is_open())
		{
			Json::FastWriter fw;
			std::string content = fw.write(m_clientList);
			clientListFile.write(content.c_str(), content.size());
			clientListFile.close();
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[%s]: Failed to create file:localClientList.json\n", __FUNCTION__);
		}

		clientNo++;
	}
	return CONNECTION_SUCCEED;
}

void MsgAppData::RemoveOnlineClient(u32 clientNode)
{
	OspPrintf(TRUE, FALSE, "[%s]: called,clientNode \"%d\" disconnect\n", __FUNCTION__, clientNode);
	// TODO
	for (std::map<u32, ClientInfo>::iterator it = m_onlineClientList.begin(); it != m_onlineClientList.end(); it++)
	{
		if (clientNode == it->second.node)
		{
			OspPrintf(TRUE, FALSE, "[%s]: remove client \"%s\" from onlineClientList\n", __FUNCTION__, it->second.name.c_str());
			m_onlineClientList.erase(it);
			return;
		}
	}
	OspPrintf(TRUE, FALSE, "[%s]: error,no target found in onlineClientList\n", __FUNCTION__);
}

void MsgAppData::AddOfflineMessage(u32 srcClientNo, u32 dstClientNo, const char* msgContent)
{
	OspPrintf(TRUE, FALSE, "[%s]: called,add offline message,src: %d,dst: %d,msgContent: %s\n", __FUNCTION__, srcClientNo, dstClientNo, msgContent);

	// 将待发送的消息保存
	Json::Value msg;
	msg.append(srcClientNo);
	msg.append(msgContent);
	m_offlineMessage[std::to_string((long long)dstClientNo)].append(msg);

	// 更新本地文件
	std::fstream offlineMessageFile("offlineMessage.json", std::ios::out | std::ios::binary);
	if (offlineMessageFile.is_open())
	{
		Json::FastWriter fw;
		std::string content = fw.write(m_offlineMessage);
		offlineMessageFile.write(content.c_str(), content.size());
		offlineMessageFile.close();
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[%s]: error:failed to create file:offlineMessage.json\n", __FUNCTION__);
	}
}

Json::Value MsgAppData::getOfflineMessage(u32 clientNode)
{
	OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

	u32 clientNo = getOnlineClientNo(clientNode);

	Json::Value ret;
	if (m_offlineMessage.isMember(std::to_string((long long)clientNo)))  // 如果有未发送的离线消息，将其返回并从记录中移除
	{
		ret = m_offlineMessage[std::to_string((long long)clientNo)];
		m_offlineMessage.removeMember(std::to_string((long long)clientNo));
		// 更新本地文件
		std::fstream offlineMessageFile("offlineMessage.json", std::ios::out | std::ios::binary);
		if (offlineMessageFile.is_open())
		{
			Json::FastWriter fw;
			std::string content = fw.write(m_offlineMessage);
			offlineMessageFile.write(content.c_str(), content.size());
			offlineMessageFile.close();
		}
		else
		{
			OspPrintf(TRUE, FALSE, "[%s]: error:failed to create file:offlineMessage.json\n", __FUNCTION__);
		}
		OspPrintf(TRUE, FALSE, "[%s]: offline messages:%s\n", __FUNCTION__, Json::StyledWriter().write(ret).c_str());
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[%s]: client %s does not have offline messages\n", __FUNCTION__, m_onlineClientList[clientNo].name.c_str());
	}
	return ret;
}

bool MsgAppData::isOnlineClient(u32 clientNo)
{
	// 只有clientNo在在线客户端列表中并且连接正常，表示该客户端在线
	if (m_onlineClientList.find(clientNo) != m_onlineClientList.end() && OspIsValidTcpNode(m_onlineClientList[clientNo].node))
	{
		return true;
	}
	return false;
}

u32 MsgAppData::getOnlineClientNo(u32 clientNode)
{
	for (std::map<u32, ClientInfo>::iterator it = m_onlineClientList.begin(); it != m_onlineClientList.end(); it++)
	{
		if (it->second.node == clientNode)
		{
			return it->first;
		}
	}
	OspPrintf(TRUE, FALSE, "[%s]: clientNode isn't a onlineClientNode\n", __FUNCTION__);
	return 0;
}

u32 MsgAppData::getOnlineClientNode(u32 clientNo)
{
	if (m_onlineClientList.find(clientNo) == m_onlineClientList.end())
	{
		OspPrintf(TRUE, FALSE, "[%s]: clientNo isn't a onlineClientNo\n", __FUNCTION__);
		return 0;
	}
	return m_onlineClientList[clientNo].node;
}

std::map<u32, MsgAppData::ClientInfo> MsgAppData::getOnlineClientList()
{
	return m_onlineClientList;
}

Json::Value MsgAppData::getClientList()
{
	return m_clientList;
}