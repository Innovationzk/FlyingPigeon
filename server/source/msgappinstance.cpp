#include <string>
#include "../include/msgappinstance.h"
#include "../include/msgappdata.h"

/****************TODO:��ʽ��������Ҫ�޸�******************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

extern zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_SERVER, MsgAppData, MAX_ALIAS_LENGTH_SERVER> msgApp;

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
	case EV_CLT_SER_REGIST_REQ:
	{
		CONNECT_RESULT registerResult = msgApp.RegisterNewClient((char*)(pMsg->content), pMsg->srcnode);
		if (CONNECTION_SUCCEED == registerResult)  // �ͻ���ע��ɹ�
		{
			OspPrintf(TRUE, FALSE, "[%s]: client registration success, send onlineClientList\n", __FUNCTION__);
			OspNodeDiscCBReg(pMsg->srcnode, MSG_APP_ID_SERVER, CInstance::DAEMON);
            OspNodeDiscCBRegQ(pMsg->srcnode, FILE_APP_ID_SERVER, CInstance::DAEMON);
            post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_REGIST_ACK, NULL, 0, pMsg->srcnode);

            // ֪ͨ���пͻ��˸��������б�
            NotifyOnlineClientList();
		}
		else if (OCCUPIED_USERNAME == registerResult)  // �ͻ������Ʊ�ռ��
		{
			OspPrintf(TRUE, FALSE, "[%s]: client registration failed, username is occupied\n", __FUNCTION__);
			ser_clt_regist_nack nackMsg;
			nackMsg.result = OCCUPIED_USERNAME;
			post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_REGIST_NACK, &nackMsg, sizeof(nackMsg), pMsg->srcnode);
		}
		else if (CLIENT_NUMBER_LIMIT == registerResult)  // �Ѵﵽ�������������
		{
			OspPrintf(TRUE, FALSE, "[%s]: client registration failed, the number of online clients has reached the maximum\n", __FUNCTION__);
			ser_clt_regist_nack nackMsg;
			nackMsg.result = CLIENT_NUMBER_LIMIT;
			post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_REGIST_NACK, &nackMsg, sizeof(nackMsg), pMsg->srcnode);
		}
		break;
	}
	case EV_CLT_SER_LOG_IN_REQ:
	{
		CONNECT_RESULT connectResult = msgApp.AddOnlineClient((char*)(pMsg->content), pMsg->srcnode);
		if (CONNECTION_SUCCEED == connectResult)  // �ͻ������ӳɹ�
		{
			OspPrintf(TRUE, FALSE, "[%s]: client connect success, send onlineClientList\n", __FUNCTION__);
            OspNodeDiscCBReg(pMsg->srcnode, MSG_APP_ID_SERVER, CInstance::DAEMON);
            OspNodeDiscCBRegQ(pMsg->srcnode, FILE_APP_ID_SERVER, CInstance::DAEMON);
			post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_LOG_IN_ACK, NULL, 0, pMsg->srcnode);

			// ֪ͨ���пͻ��˸��������б�
			NotifyOnlineClientList();

			// ���ÿͻ�����������Ϣ����ȫ������
			Json::Value offlineMsg = msgApp.getOfflineMessage(pMsg->srcnode);
			if (!offlineMsg.empty())
			{
				ser_clt_post_msg_ntf msg;
				for (int i = 0; i < offlineMsg.size(); i++)
				{
					msg.srcClientNo = offlineMsg[i][0].asUInt();
					strcpy(msg.msgContent, offlineMsg[i][1].asString().c_str());
					post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_POST_MSG_NTF, &msg, sizeof(msg), pMsg->srcnode);
				}
			}
		}
		else if (CLIENT_NUMBER_LIMIT)  // �Ѵﵽ�������������
		{
			OspPrintf(TRUE, FALSE, "[%s]: client connect failed, the number of online clients has reached the maximum\n", __FUNCTION__);
			ser_clt_log_in_nack nackMsg;
			nackMsg.result = CLIENT_NUMBER_LIMIT;
			post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_LOG_IN_NACK, &nackMsg, sizeof(nackMsg), pMsg->srcnode);
		}
		else
		{
			// do nothing
		}
		break;
	}
	case EV_CLT_SER_POST_MSG_NTF:
	{
		SendMsg(pMsg);
		break;
	}
	case OSP_DISCONNECT:
	{
		// �����ԣ��Ͽ�ʱ��pMsg->content��һ��ָ�룬ָ��*pMsg����һ���ڴ��ַ���õ�ַ��ֵΪ�Ͽ��Ľڵ�ţ��д���һ����֤��
		u32 node = *(pMsg->content);  // �Ͽ��Ľڵ��
		OspPrintf(TRUE, FALSE, "[%s]: node disconnect:%d\n", __FUNCTION__, node);
		msgApp.RemoveOnlineClient(node);
		NotifyOnlineClientList();
		break;
	}
	default:
		break;
	}
}

void MsgAppInstance::NotifyOnlineClientList()
{
	OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
	std::map<u32, MsgAppData::ClientInfo> onlineClientList = msgApp.getOnlineClientList();
	ser_clt_regist_list_ntf clientListMsg;
	std::string allClientName;

	// ��ȡ���߿ͻ������ݲ�������clientListMsg��
	clientListMsg.clientNum = onlineClientList.size();
	int i = 0;
	for (std::map<u32, MsgAppData::ClientInfo>::iterator it = onlineClientList.begin(); it != onlineClientList.end(); it++)
	{
		allClientName += it->second.name += '\0';
		clientListMsg.clientNo[i++] = it->first;
	}
	memcpy(clientListMsg.clientName, allClientName.c_str(), allClientName.size());

	// ֪ͨÿһ�����߿ͻ��˿ͻ����б�
	OspPrintf(TRUE, FALSE, "[%s]: notify all client clientList\n", __FUNCTION__);
	for (std::map<u32, MsgAppData::ClientInfo>::iterator it = onlineClientList.begin(); it != onlineClientList.end(); it++)
	{
		post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_REGIST_LIST_NTF, &clientListMsg, sizeof(clientListMsg), it->second.node);
	}
}

void MsgAppInstance::SendMsg(CMessage* const pMsg)
{
	OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

	clt_ser_post_msg_ntf* srcMsg = (clt_ser_post_msg_ntf*)(pMsg->content);
	OspPrintf(TRUE, FALSE, "[%s]: target clients number is:%d,msg content is:%s\n", __FUNCTION__, srcMsg->clientNum, srcMsg->msgContent);

	u32 srcClientNo = msgApp.getOnlineClientNo(pMsg->srcnode);
	for (int i = 0; i < srcMsg->clientNum; i++)
	{
		u32 dstClientNo = srcMsg->clientNo[i];
		if (msgApp.isOnlineClient(dstClientNo))  // Ŀ��ͻ������ߣ�ֱ�ӷ���
		{
			u32 dstClientNode = msgApp.getOnlineClientNode(dstClientNo);
			ser_clt_post_msg_ntf dstMsg;
			dstMsg.srcClientNo = srcClientNo;
			strcpy(dstMsg.msgContent, srcMsg->msgContent);
			post(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), EV_SER_CLT_POST_MSG_NTF, &dstMsg, sizeof(dstMsg), dstClientNode);
		}
		else  // Ŀ��ͻ������ߣ����淢������������
		{
			msgApp.AddOfflineMessage(srcClientNo, dstClientNo, srcMsg->msgContent);
		}
	}
}

void MsgAppInstance::InstanceEntry(CMessage* const pMsg)
{
	OspPrintf(TRUE, FALSE, "[%s]: called, eventType is:%d\n", __FUNCTION__, pMsg->event);
}