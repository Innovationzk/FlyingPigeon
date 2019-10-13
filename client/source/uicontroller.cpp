#include <cstring>
#include <memory>
#include <QTimer>
#include <QDebug>
#include <QVector>
#include <QMessageBox>
#include "uicontroller.h"
#include "friendlistscreen.h"
#include "loginscreen.h"
#include "clientapp/clientapp.h"
#include "singlechatscreen.h"
#include "multichatscreen.h"
#include "multichatfriendlistitem.h"

/*
#ifndef _DEBUG
#define printf()
#endif // _DEBUG
*/

UIController* UIController::sm_uiController = NULL;
QString UIController::sm_userName = "undefined";

UIController::UIController(QObject *parent):QObject(parent)
{
    sm_uiController = this;

    // 创建画面
    m_friendListScreen = new FriendListScreen;
    m_loginScreen = new LoginScreen;

    // 显示登录画面
    //m_loginScreen->show();
    m_friendListScreen->show();

    connect(m_loginScreen, SIGNAL(sigConnectSucceed(QString)), this, SLOT(onNotifyConnectSucceed(QString)));
    connect(this, SIGNAL(sigRegistList(clt_ui_client_list_ntf)), m_friendListScreen, SLOT(onUpdateClientList(clt_ui_client_list_ntf)));
}

UIController::~UIController()
{
}

UIController* UIController::getInstance()
{
    return sm_uiController;
}

QString UIController::getUserName()
{
    return sm_userName;
}

// 该函数为底层回调函数
void UIController::serviceCallBackFunc(u16 eventType, const void* content, u16 length)
{
    qDebug("[%s]: called, eventType: %d\n", __FUNCTION__, eventType);

    switch (eventType)
    {
    case EV_CLT_UI_REGIST_ACK:
    case EV_CLT_UI_REGIST_NACK:
    case EV_CLT_UI_LOG_IN_ACK:
    case EV_CLT_UI_LOG_IN_NACK:
    {
        qDebug("[%s]: emit sigServerStatus\n", __FUNCTION__);
        emit sm_uiController->sigServerStatus(eventType);
        break;
    }
    case EV_CLT_UI_CONNECT_RESULT_NTF:
    {
        qDebug("[%s]: emit sigConnectReult\n", __FUNCTION__);
        emit sm_uiController->sigConnectReult(eventType, *((clt_ui_connect_result_ntf*)(content)));
        break;
    }
    case EV_CLT_UI_REGIST_LIST_NTF:
    {
        qDebug("[%s]: emit regist_list_ntf\n", __FUNCTION__);
        emit sm_uiController->sigRegistList(*((clt_ui_regist_list_ntf*)(content)));
        break;
    }
    case EV_CLT_UI_POST_FILE_ACK:
    case EV_CLT_UI_POST_FILE_NACK:
    {
        qDebug("[%s]: emit post_file result\n", __FUNCTION__);
        emit sm_uiController->sigSendFileResult(eventType);
        break;
    }
    case EV_CLT_UI_POST_FILE_PROGRESS_NTF:
    {
        ui_clt_post_file_progress_ntf* msg = (ui_clt_post_file_progress_ntf*)content;
        emit sm_uiController->sigNotifyFileProgress(msg->fileNo, msg->progress);
    }
    default:
        break;
    }
}

/***********************public slots****************************/
void UIController::onNotifyConnectSucceed(QString userName)
{
    sm_userName=userName;
    m_loginScreen->hide();
    m_loginScreen->close();
    m_friendListScreen->setUserName(userName);
    m_friendListScreen->show();
}

