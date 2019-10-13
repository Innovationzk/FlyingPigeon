#pragma once
#include <QObject>
#include "../common/commondef.h"

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
    static UIController *sm_ui;

    // client_service�ص�����
    static void ServiceCallBackFunc(u16 eventType, const void* content, u16 length);

signals:
    // client_service�ص�ʱ�����ź�
    void SignalServiceCallBack(u16 eventType, const void* content, u16 length);

public slots:
    // client_service�ص�ʱ�ۺ���
    void onSignalServiceCallBack(u16 eventType, const void* content, u16 length);
    // log in��ťѹ��
    void onBtnLoginClicked(const QString& userName);

private:
    FriendListScreen *m_friendListScreen;
    LoginScreen *m_loginScreen;
};

