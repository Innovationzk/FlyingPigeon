#include <clientapp.h>
#include <fileappdata.h>
#include <fileappinstance.h>
#include <msgappdata.h>
#include <msgappinstance.h>
#include <commondef.h>

/****************TODO:正式发布版需要修改******************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

zTemplate<FileAppInstance, FILE_APP_INSTANCE_NUMBER_CLIENT, FileAppData, MAX_ALIAS_LENGTH_CLIENT> fileApp;  // 定义fileApp
zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_CLIENT, MsgAppData, MAX_ALIAS_LENGTH_CLIENT> msgApp;      // 定义msgApp

int serverNode = INVALID_NODE;

void tempFun(u16 eventType, const void* content, u16 length)
{
    return;
}
typedef void (*CallBack)(u16 eventType, const void* content, u16 length);
CallBack CallBackFunc = tempFun;

void CLIENTAPP::Init()
{
	if (FALSE == IsOspInitd())
	{
#ifdef _DEBUG
		OspInit(TRUE, TELNET_PORT_CLIENT);
#endif
		OspSetPrompt("client");     // 设置终端提示符

		if (OSP_OK != fileApp.CreateApp(FILE_APP_NAME_CLIENT, FILE_APP_ID_CLIENT, FILE_APP_PRIORITY_CLIENT, FILE_APP_QUEUE_SIZE_CLIENT))
		{
			OspPrintf(TRUE, FALSE, "[%s]: Failed to create fileApp\n", __FUNCTION__);
		}
        if (OSP_OK != msgApp.CreateApp(MSG_APP_NAME_CLIENT, MSG_APP_ID_CLIENT, MSG_APP_PRIORITY_CLIENT, MSG_APP_QUEUE_SIZE_CLIENT))
        {
            OspPrintf(TRUE, FALSE, "[%s]: Failed to create fileApp\n", __FUNCTION__);
        }
	}
	else
	{
		OspPrintf(TRUE, FALSE, "[%s]: Osp already initialized\n", __FUNCTION__);
	}
}

DLL_EXPORT void CLIENTAPP::Post(u16 eventType, const void* content, u16 length)
{
	if (eventType < EV_UI_CLT_REGIST_REQ)    // 发消息给FileApp
	{
		OspPost(MAKEIID(FILE_APP_ID_CLIENT, CInstance::DAEMON), eventType, content, length);
	}
	else  // 发消息给MsgApp
	{
		OspPost(MAKEIID(MSG_APP_ID_CLIENT, CInstance::DAEMON), eventType, content, length);
	}
}

DLL_EXPORT void CLIENTAPP::SetCallBackFunc(void(*callBack)(u16 eventType, const void* content, u16 length))
{
	CallBackFunc = callBack;
}
