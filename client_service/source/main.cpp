#if 0
#include <iostream>
#include "../include/clientapp.h"
#include "../include/fileappinstance.h"
#include "../include/fileappdata.h"
#include "../include/msgappinstance.h"
#include "../include/msgappdata.h"
#include "../../common/osp.h"
#include "../../common/commondef.h"

/********************������*********************/
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
    const char* help = "�������ţ�\nh���������\n1��������������״̬\n2��ע�Ტ���ӷ�����\n3��ֱ�����ӷ�����\n4���Ͽ�����\n5��������Ϣ\nf�������ļ�\np����ͣ����\nc����������\n";
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
			if (OspIsValidTcpNode(serverNode)) printf("����������\n");
			else printf("����������\n");
			break;
		case '2':
		{
            cout << "�������û���\n";
            string name;
            cin >> name;
            Post(EV_UI_CLT_REGIST_REQ, name.c_str(), name.size() + 1);
			break;
		}
        case '3':
        {
            cout << "�������û���\n";
            string name;
            cin >> name;
            Post(EV_UI_CLT_LOG_IN_REQ, name.c_str(), name.size() + 1);
            break;
        }
        case '4':
        {
            if (TRUE == OspDisconnectTcpNode(serverNode)) OspPrintf(TRUE, FALSE, "[%s]: �ɹ��Ͽ�����\n", __FUNCTION__);
            break;
        }
        case '5':
        {
            clt_ser_post_msg_ntf msg;
            cout << "������Ŀ��ͻ�������\n";
            cin >> msg.clientNum;
            cout << "����������Ŀ��ͻ��˱��\n";
            for (int i = 0; i < msg.clientNum; i++) cin >> msg.clientNo[i];
            cout << "��������Ϣ����\n";
            cin >> msg.msgContent;
            Post(EV_UI_CLT_POST_MSG_REQ, &msg, sizeof(msg));
            break;
        }
        case 'f':
        {
            ui_clt_post_file_req file;
            cout << "�������ļ�����\n";
            cin >> file.fileNum;
            char* p = file.fileName;
            for (int i = 0; i < file.fileNum; i++)
            {
                cout << "�������ļ�����·��\n";
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
            cout << "��������Ҫ��ͣ���ļ���ţ�������0��ʾȫ����ͣ��\n";
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
            cout << "��������Ҫ�������ļ���ţ�������0��ʾȫ��������\n";
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
            cout<<"������Ҫȡ�����ļ���ţ�������0��ʾȫ��������\n";
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