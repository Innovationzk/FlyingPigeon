#include "../include/fileappdata.h"
#include "../include/fileappinstance.h"
#include "../include/msgappdata.h"
#include "../include/msgappinstance.h"
#include <iostream>
/****************TODO:正式发布版需要修改*****************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

zTemplate<FileAppInstance, FILE_APP_INSTANCE_NUMBER_SERVER, FileAppData, MAX_ALIAS_LENGTH_SERVER> fileApp;
zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_SERVER, MsgAppData, MAX_ALIAS_LENGTH_SERVER> msgApp;

int main()
{
#ifdef _DEBUG
    if (FALSE == IsOspInitd())
    {
        OspInit(TRUE, TELNET_PORT_SERVER);
    }
#endif

	if (INVALID_SOCKET == OspCreateTcpNode((u32)inet_addr( "0.0.0.0" ), LISTEN_PORT_SERVER, TRUE))
    {
        OspPrintf(TRUE, FALSE, "[%s]: error: cannot create TcpNode\n", __FUNCTION__);
    }
	fileApp.CreateApp(FILE_APP_NAME_SERVER, FILE_APP_ID_SERVER, FILE_APP_PRIORITY_SERVER, FILE_APP_QUEUE_SIZE_SERVER);
	msgApp.CreateApp(MSG_APP_NAME_SERVER, MSG_APP_ID_SERVER, MSG_APP_PRIORITY_SERVER, MSG_APP_QUEUE_SIZE_SERVER);

    OspSetPrompt("server->>");
    

	char chr;
	//std::cout << "请输入编号：\n1：输出在线客户端列表\n2：输出所有客户端列表\n";
	while (TRUE)
	{
		OspDelay(100);
		chr = getchar();
		switch (chr) {
		case '1':
		{
			std::map<u32, MsgAppData::ClientInfo> onlineClientList = msgApp.getOnlineClientList();
			for (std::map<u32, MsgAppData::ClientInfo>::iterator it = onlineClientList.begin(); it != onlineClientList.end(); it++)
			{
				std::cout << it->first << '\t' << it->second.name << '\t' << it->second.node << '\n';
			}
			break;
		}
		case '2':
		{
			Json::StyledWriter sw;
			std::cout << sw.write(msgApp.getClientList());
			break;
		}
        case '3':
            {
                OspPrintf(TRUE, FALSE, "[%s]: local ip:%d\n", __FUNCTION__, OspNodeIpGet(0));
                break;
            }
		default:
			break;
		}
	}
    OspQuit();

	return 0;
}