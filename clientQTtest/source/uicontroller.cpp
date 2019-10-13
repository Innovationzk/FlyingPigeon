#include "uicontroller.h"
#include "friendlistscreen.h"
#include "loginscreen.h"

#include <cstring>
#include <memory>
#include <QTimer>
#include <QDebug>
#include <QDir>

UIController* UIController::sm_ui = NULL;

UIController::UIController(QObject *parent):QObject(parent)
{
    sm_ui = this;

    qDebug()<<QDir::currentPath();

    // ´´½¨»­Ãæ
    m_friendListScreen = new FriendListScreen;
    m_loginScreen = new LoginScreen;

    //m_loginScreen->show();
    m_friendListScreen->show();
    connect(m_loginScreen, SIGNAL(btnQuitClicked()), this, SIGNAL(quitApp()));
    connect(m_loginScreen, SIGNAL(btnLoginClicked(QString)), this, SLOT(onBtnLoginClicked(QString)));

    connect(this, SIGNAL(notifyClientList(clt_ui_client_list_ntf*)), m_friendListScreen, SLOT(onUpdateClientList(clt_ui_client_list_ntf*)));
}

UIController::~UIController()
{
}

void UIController::ServiceCallBackFunc(u16 eventType, const void* content, u16 length)
{
    qDebug() << __FUNCTION__ << " eventType:" << eventType;
    emit sm_ui->SignalServiceCallBack(eventType, content, length);
}

/**************************public slots********************************/
void UIController::onSignalServiceCallBack(u16 eventType, const void* content, u16 length)
{
    // TODO
}


void UIController::onBtnLoginClicked(const QString& userName)
{
//    m_loginScreen->close();
//    m_mainWindow->show();
}
