#pragma once
#include <QObject>
#include "../common/commondef.h"

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
    static UIController *sm_ui;

    // client_service回调函数
    static void ServiceCallBackFunc(u16 eventType, const void* content, u16 length);

signals:
    // client_service回调时发送信号
    void SignalServiceCallBack(u16 eventType, const void* content, u16 length);

public slots:
    // client_service回调时槽函数
    void onSignalServiceCallBack(u16 eventType, const void* content, u16 length);
    // log in按钮压下
    void onBtnLoginClicked(const QString& userName);

private:
    FriendListScreen *m_friendListScreen;
    LoginScreen *m_loginScreen;
};

