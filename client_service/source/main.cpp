#if 0
#include <iostream>
#include "../include/clientapp.h"
#include "../include/fileappinstance.h"
#include "../include/fileappdata.h"
#include "../include/msgappinstance.h"
#include "../include/msgappdata.h"
#include "../../common/osp.h"
#include "../../common/commondef.h"

/********************测试用*********************/
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)

extern zTemplate<FileAppInstance, FILE_APP_INSTANCE_NUMBER_CLIENT, FileAppData, MAX_ALIAS_LENGTH_CLIENT> fileApp;
extern zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_CLIENT, MsgAppData, MAX_ALIAS_LENGTH_CLIENT> msgApp;

extern int serverNode;
using namespace CLIENTAPP;
using namespace std;

int main()
{
	Init();

	char chr;
    const char* help = "请输入编号：\nh：输出帮助\n1：检查服务器连接状态\n2：注册并连接服务器\n3：直接连接服务器\n4：断开连接\n5：发送消息\nf：发送文件\np：暂停发送\nc：继续发送\n";
	std::cout << help;
	while (TRUE)
	{
		OspDelay(100);
		chr = getchar();
		switch (chr){
        case 'h':
            std::cout << help;
            break;
		case '1':
			if (OspIsValidTcpNode(serverNode)) printf("服务器在线\n");
			else printf("服务器离线\n");
			break;
		case '2':
		{
            cout << "请输入用户名\n";
            string name;
            cin >> name;
            Post(EV_UI_CLT_REGIST_REQ, name.c_str(), name.size() + 1);
			break;
		}
        case '3':
        {
            cout << "请输入用户名\n";
            string name;
            cin >> name;
            Post(EV_UI_CLT_LOG_IN_REQ, name.c_str(), name.size() + 1);
            break;
        }
        case '4':
        {
            if (TRUE == OspDisconnectTcpNode(serverNode)) OspPrintf(TRUE, FALSE, "[%s]: 成功断开连接\n", __FUNCTION__);
            break;
        }
        case '5':
        {
            clt_ser_post_msg_ntf msg;
            cout << "请输入目标客户端数量\n";
            cin >> msg.clientNum;
            cout << "请依次输入目标客户端编号\n";
            for (int i = 0; i < msg.clientNum; i++) cin >> msg.clientNo[i];
            cout << "请输入消息内容\n";
            cin >> msg.msgContent;
            Post(EV_UI_CLT_POST_MSG_REQ, &msg, sizeof(msg));
            break;
        }
        case 'f':
        {
            ui_clt_post_file_req file;
            cout << "请输入文件数量\n";
            cin >> file.fileNum;
            char* p = file.fileName;
            for (int i = 0; i < file.fileNum; i++)
            {
                cout << "请输入文件完整路径\n";
                string str;
                cin >> str;
                strcpy(p, str.c_str());
                while (*++p != '\0');
                ++p;
            }
            Post(EV_UI_CLT_POST_FILE_REQ, &file, sizeof(file));
            break;
        }
        case 'p':
        {
            cout << "请输入需要暂停的文件编号：（输入0表示全部暂停）\n";
            int i;
            cin >> i;
            if (0 == i)
            {
                Post(EV_UI_CLT_PAUSE_ALL_FILE_CMD, NULL, 0);
                break;
            }
            ui_clt_pause_file_cmd msg;
            msg.fileNo = i;
            Post(EV_UI_CLT_PAUSE_FILE_CMD, &msg, sizeof(msg));
            break;
        }
        case 'c':
        {
            cout << "请输入需要继续的文件编号：（输入0表示全部继续）\n";
            int i;
            cin >> i;
            if (0 == i)
            {
                Post(EV_UI_CLT_CONTINUE_ALL_FILE_CMD, NULL, 0);
                break;
            }
            ui_clt_continue_file_cmd msg;
            msg.fileNo = i;
            Post(EV_UI_CLT_CONTINUE_FILE_CMD, &msg, sizeof(msg));
            break;
        }
        case 'l':
        {
            cout<<"请输入要取消的文件编号：（输入0表示全部继续）\n";
            int i;
            cin >> i;
            if (0 == i)
            {
                Post(EV_UI_CLT_CANCEL_ALL_FILE_CMD, NULL, 0);
                break;
            }
            ui_clt_continue_file_cmd msg;
            msg.fileNo = i;
            Post(EV_UI_CLT_CANCEL_FILE_CMD, &msg, sizeof(msg));
            break;
        }
		}
	}
	return 0;
}
#endif