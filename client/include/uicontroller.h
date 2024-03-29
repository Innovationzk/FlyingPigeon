#ifndef UI_H
#define UI_H
#include <QObject>
#include <QString>
#include "commondef.h"

class FriendListScreen;
class LoginScreen;

// UIController类用来控制画面的显示与迁移
class UIController:public QObject
{
    Q_OBJECT

public:
    explicit UIController(QObject *parent = 0);
    ~UIController();

    // 全局指针
    static UIController *sm_uiController;
    static UIController* getInstance();

    static QString getUserName();

    // client_service回调函数
    static void serviceCallBackFunc(u16 eventType, const void* content, u16 length);

signals:
    /*************client_service回调时发送的信号*************/
    void sigServerStatus(u16 eventType);                     // 服务器状态
    void sigConnectReult(clt_ui_connect_result_ntf result);  // 登录结果
    void sigRegistList(clt_ui_regist_list_ntf registList);   // 在线客户端列表
    void sigSendMsgNack();                                   // 服务端离线，消息发送失败
    void sigReceiveMsg(clt_ui_receive_msg_ntf msg);          // 收到消息
    void sigSendFileResult(u16 eventType);                   // 开始发送文件
    void sigNotifyFileProgress(u32 fileNo, u32 progress);    // 文件发送进度
    void sigNotifySendFileFalied(u32 fileNo);                // 文件发送失败
    void sigPostFileComplete(u32 fileNo);                    // 文件发送成功
    void sigResendFile(u32 fileNo);                          // 文件校验失败，需要重新发送
    void sigDisconnect();                                    // 连接断开

public slots:
    void onNotifyConnectSucceed(QString userName);

private:
    FriendListScreen *m_friendListScreen;
    LoginScreen *m_loginScreen;

    static QString sm_userName;
};

#endif // UI_H
