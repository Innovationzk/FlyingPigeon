#include <fstream>
#include <cstring>
#include "../../common/osp.h"
#include "../../common/commondef.h"
#include "../include/demoinstanceserver.h"

#define OspPrintf(a, b, ...) printf(__VA_ARGS__)

DemoInstanceServer::DemoInstanceServer(void)
{
}


DemoInstanceServer::~DemoInstanceServer(void)
{
    if (file.is_open())
    {
        file.close();
    }
}

void DemoInstanceServer::InstanceEntry(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called.Instance number:%d\n", __FUNCTION__, m_instId);

    u16 wEventType = pMsg->event;


    switch(wEventType)
    {
	case 3636:
		printf("set timer\n");
		SetTimer(TIMEOUT_PERIOD, 2000);
		break;
	case TIMEOUT_PERIOD:
	{
		static int i = 0;
		++i;
		printf("on timeout %d\n", i);
		break;
	}
	case EV_CLT_SER_POST_MSG_REQ:
		printf("receive message%d\n",*((int*)(pMsg->content)));
		OspDelay(100);
		break;
	case EV_CLT_SER_REGIST_REQ:
		printf("%s\n",pMsg->content);
		break;
    default:
        {
			
        }
    }
}

void DemoInstanceServer::receiveFile(CMessage *const pMsg)
{
    file.write((const char *)(pMsg->content), pMsg->length);
}

void DemoInstanceServer::DaemonInstanceEntry(CMessage *const pMsg, CApp *pCApp)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n",__FUNCTION__);

    u16 wEventType = pMsg->event;

    switch(wEventType)
    {
	case TIMEOUT_PERIOD:
	{
		static int i = 0;
		++i;
		printf("on timeout %d\n", i);
		SetTimer(TIMEOUT_PERIOD, 500);
		break;
	}
	case EV_CLT_SER_REGIST_REQ:
		OspPrintf(TRUE, FALSE, "[%s]: new client regist:client node is%d\n", __FUNCTION__, pMsg->srcnode);
		break;
    case EV_CLT_SER_START_POST_FILE_REQ:
    {
        // TODO 让DaemonInstance分配空闲的Instance处理数据
        const char *fileName = getFileNameFromFullPath((char*)(pMsg->content));
		OspPrintf(TRUE, FALSE, "[%s]: fileName is:%s\n", __FUNCTIONW__, fileName);
        file.open(fileName, std::ios::out | std::ios::binary);
        if (!file.is_open())
        {
            OspPrintf(TRUE, FALSE, "[%s]: cannot create file,file name is:%s\n", __FUNCTION__, fileName);
        }
        OspPrintf(TRUE, FALSE, "[%s]: post file confirm\n", __FUNCTION__);
        OspPost(MAKEIID(APP_ID_CLIENT, 1), EV_SER_CLT_POST_FILE_ACK, NULL, 0, pMsg->srcnode);
    }
        break;
    case EV_CLT_SER_POST_FILE_REQ:
        receiveFile(pMsg);
        break;
    case EV_CLT_SER_FILE_MD5_NTF:
        file.close();
        break;
    default:
        {

        }
    }
}

const char* DemoInstanceServer::getFileNameFromFullPath(const char* chr)
{
    return strrchr(chr, '/')+1;
}
