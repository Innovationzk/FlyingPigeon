#ifndef UI_H
#define UI_H
#include <QObject>
#include <QString>
#include "commondef.h"

class FriendListScreen;
class LoginScreen;

// UIController���������ƻ������ʾ��Ǩ��
class UIController:public QObject
{
    Q_OBJECT

public:
    explicit UIController(QObject *parent = 0);
    ~UIController();

    // ȫ��ָ��
    static UIController *sm_uiController;
    static UIController* getInstance();

    static QString getUserName();

    // client_service�ص�����
    static void serviceCallBackFunc(u16 eventType, const void* content, u16 length);

signals:
    /*************client_service�ص�ʱ���͵��ź�*************/
    void sigServerStatus(u16 eventType);                     // ������״̬
    void sigConnectReult(u16 eventType, clt_ui_connect_result_ntf result);  // ��¼���
    void sigRegistList(clt_ui_regist_list_ntf registList);   // ���߿ͻ����б�
    void sigSendFileResult(u16 eventType);                   // ��ʼ�����ļ�
    void sigNotifyFileProgress(u32 fileNo, u32 progress);    // �ļ����ͽ���

public slots:
    void onNotifyConnectSucceed(QString userName);

private:
    FriendListScreen *m_friendListScreen;
    LoginScreen *m_loginScreen;

    static QString sm_userName;
};

#endif // UI_H
