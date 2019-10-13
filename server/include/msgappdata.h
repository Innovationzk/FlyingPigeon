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

	static int clientNo; // ���пͻ���Ψһ��ʶ����Ŵ�1��ʼ������ֵ������һ��ע��Ŀͻ��˵ı��

	MsgAppData();
	~MsgAppData();

	// �¿ͻ���ע��
	CONNECT_RESULT RegisterNewClient(const char* name, u32 node);
	// �ͻ��˵�¼���������߿ͻ����б�
	CONNECT_RESULT AddOnlineClient(const char* name, u32 node);
	// �ͻ��˶Ͽ����ӣ��������߿ͻ����б�
	void RemoveOnlineClient(u32 clientNode);
	// ��ӵȴ����͵�������Ϣ
	void AddOfflineMessage(u32 srcClientNo, u32 dstClientNo, const char* msgContent);
	// ��ȡĿ��ͻ��˵�������Ϣ
	Json::Value getOfflineMessage(u32 clientNode);
	// ���ͻ����Ƿ�����
	bool isOnlineClient(u32 clientNo);
	// ���ݿͻ��˽ڵ�Ż�ȡ����
	u32 getOnlineClientNo(u32 clientNode);
	// ���ݿͻ��˱�źŻ�ȡ��ڵ�
	u32 getOnlineClientNode(u32 clientNo);

	std::map<u32, ClientInfo> getOnlineClientList();
	Json::Value getClientList();

private:
	std::map<u32, ClientInfo> m_onlineClientList;   // ���߿ͻ����б�
	Json::Value m_clientList;       // ����ע����Ŀͻ����б�
	Json::Value m_offlineMessage;   // �ȴ����͵�������Ϣ�б�
};