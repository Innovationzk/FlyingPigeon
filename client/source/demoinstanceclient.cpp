#include "../include/demoinstanceclient.h"
#include "../include/demoappdata.h"
#include "../../common/commondef.h"

#define OspPrintf(a, b, ...) printf(__VA_ARGS__)

extern s32 serverNode;

typedef zTemplate<DemoInstanceClient, MAX_FILE_INSTANCE_NUMBER_CLIENT, DemoAppData, MAX_ALIAS_LEN_CLIENT> DemoAppClient;
extern DemoAppClient demoAppClient;

DemoInstanceClient::DemoInstanceClient(void)
    : m_dwServerNode(0)
{
}


DemoInstanceClient::~DemoInstanceClient(void)
{
    file.close();
}

void DemoInstanceClient::InstanceEntry(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n",__FUNCTION__);

    u16 wEventType = pMsg->event;

    switch(wEventType)
    {
    case EV_UI_CLT_POST_FILE_CMD:
        file.open((const char*)(pMsg->content), std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            OspPrintf(TRUE, FALSE, "[%s]: cannot open file:%s", __FUNCTION__, pMsg->content);
            return;
        }
        OspPrintf(TRUE, FALSE, "[%s]: Start post file,file name is:%s", __FUNCTION__, pMsg->content);
        m_fileSize=GetFileSize((char*)(pMsg->content));
        OspPost(MAKEIID(APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_POST_FILE_REQ, pMsg->content, pMsg->length, serverNode);
        break;
    case EV_SER_CLT_POST_FILE_ACK:
        if (!file.is_open())
        {
            OspPrintf(TRUE, FALSE, "[%s]: file didn't opend\n", __FUNCTION__);
            return;
        }
        m_progress=0;
        OspPrintf(TRUE, FALSE, "[%s]: receive message", __FUNCTION__);
        //OspPost(MAKEIID(APP_ID_CLIENT, 1), EV_CLT_CLT_POST_FILE_CMD);
        break;
    case EV_SER_CLT_FILE_MD5_NTF:
        OspPrintf(TRUE, FALSE, "[%s]: receive message", __FUNCTION__);
        break;
    case EV_CLT_CLT_LOG_CMD:
        OspPrintf(TRUE, FALSE, "%s", pMsg->content);
        break;
    default:
        {

        }
    }
}

void DemoInstanceClient::DaemonInstanceEntry(CMessage *const pMsg, CApp *pCApp)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n",__FUNCTION__);
    OspPrintf(TRUE, FALSE, "[%s]: pCApp is -> %p\n", __FUNCTION__, pCApp);
    typedef zTemplate<DemoInstanceClient, MAX_FILE_INSTANCE_NUMBER_CLIENT, DemoAppData, MAX_ALIAS_LEN_CLIENT> DemoAppClient;
    DemoAppClient *demoAppClient = dynamic_cast<DemoAppClient*>(pCApp);
    OspPrintf(TRUE, FALSE, "[%s]: demoAppClient is -> %p\n", __FUNCTION__, demoAppClient);

    u16 wEventType = pMsg->event;

    switch(wEventType)
    {
    case EV_CLT_SER_REGIST_REQ:
        {
            OspPost(MAKEIID(APP_ID_SERVER, 1), EV_CLT_SER_REGIST_REQ, pMsg->content, pMsg->length, serverNode);
            AddInstTimerInfo(EV_CLT_SER_REGIST_REQ , "123456\0");
            OspPrintf(TRUE, FALSE, "[%s]:post message,\n", __FUNCTION__);
        }
        break;
    case EV_CLT_CLT_LOG_CMD:
        {
            int i = SetTimer(EV_CLT_SER_REGIST_REQ, 1000);
            OspPrintf(TRUE, FALSE, "[%s]:i=%d\n", __FUNCTION__, i);
        }
    default:
        {

        }
    }
}

void DemoInstanceClient::postFile()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n",__FUNCTION__);

    file.read(buffer, FILE_BUF_SIZE);
    if (file.eof())
    {
        OspPrintf(TRUE, FALSE, "[%s]: post the last message\n", __FUNCTION__);
        OspPost(MAKEIID(APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_POST_FILE_REQ, buffer, file.gcount(), serverNode);
        OspPrintf(TRUE, FALSE, "[%s]: update progress 100%%\n", __FUNCTION__);
        demoAppClient.getCallBackSendProgress()(100);
        OspPost(MAKEIID(APP_ID_SERVER, CInstance::DAEMON), EV_SER_CLT_FILE_MD5_NTF, NULL, 0, serverNode);
        OspPrintf(TRUE, FALSE, "[%s]: post file complete\n", __FUNCTION__);
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: post message\n", __FUNCTION__);
        OspPost(MAKEIID(APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_POST_FILE_REQ, buffer, FILE_BUF_SIZE, serverNode);
        m_progress += FILE_BUF_SIZE;
        OspPrintf(TRUE, FALSE, "[%s]: update progress %f%%\n", __FUNCTION__, m_progress);
        demoAppClient.getCallBackSendProgress()(m_progress*100.0/m_fileSize);
        //OspPost(MAKEIID(APP_ID_CLIENT, 1), EV_CLT_CLT_POST_FILE_CMD);
        OspDelay(10);
    }
}

void DemoInstanceClient::setServerNode(s32 serverNode)
{
    m_dwServerNode = serverNode;
}

s32 DemoInstanceClient::getServerNode(s32 serverNode)
{
    return m_dwServerNode;
}
