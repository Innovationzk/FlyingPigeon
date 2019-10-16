#include <QLabel>
#include <QFont>
#include <QFile>
#include <QString>
#include <QFrame>
#include <QDebug>
#include <QTextStream>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QMovie>
#include <QApplication>
#include "loginscreen.h"
#include "clientapp/clientapp.h"
#include "uicontroller.h"
#include "commondef.h"

#define LOGIN_SCREEN_WIDTH 500
#define LOGIN_SCREEN_HEIGHT 350

LoginScreen::LoginScreen(QWidget *parent) : QWidget(parent)
{
    // 设置窗口属性
    this->setWindowFlags(Qt::FramelessWindowHint);                // 去掉标题栏
    this->setAttribute(Qt::WA_TranslucentBackground);             // 设置窗口透明
    this->setFixedSize(LOGIN_SCREEN_WIDTH, LOGIN_SCREEN_HEIGHT);  // 设置窗口大小

    // 设置背景，比窗口小20dot，用于设置阴影
    QFrame *frameBackground = new QFrame(this);
    frameBackground->setFixedSize(this->width()-40,this->height()-40);
    frameBackground->move(20,20);
    frameBackground->setStyleSheet("QFrame{background-color: rgb(255, 255, 255);border-radius: 24px;padding: 2px}"
                         "QLineEdit{border-radius: 15px;background: rgb(235,235,235);selection-background-color: green;font-size: 12px}"
                         "QPushButton{background-color: rgb(0,180,240); border:2px; border-radius:15px; padding: 2px}");

    // 设置阴影
    m_frameShadowEffect = new QGraphicsDropShadowEffect(this);
    m_frameShadowEffect->setOffset(8,8);
    m_frameShadowEffect->setColor(QColor(0,0,0,100));
    m_frameShadowEffect->setBlurRadius(10);
    frameBackground->setGraphicsEffect(m_frameShadowEffect);

    // “用户登录”按钮
    labelLogin = new QLabel(frameBackground);
    QFont font(QString::fromLocal8Bit("微软雅黑"),28,66);
    labelLogin->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(55, 55, 55));
    labelLogin->setPalette(palette);
    labelLogin->setText(QString::fromLocal8Bit("用户登录"));
    labelLogin->move(LOGIN_SCREEN_WIDTH/2-110,40);

    // 输入框
    m_lineEditUserName= new QLineEdit(frameBackground);
    m_lineEditUserName->resize(frameBackground->width()-100,31);
    m_lineEditUserName->setFocus(Qt::MouseFocusReason);
    m_lineEditUserName->move((frameBackground->width()-m_lineEditUserName->width())/2,130);
    QFile fileUserName("userName.txt");
    fileUserName.open(QIODevice::ReadOnly);
    if (fileUserName.isOpen())  // 如果已经登录过，直接显示之前登录的用户名
    {
        QString strUserName = fileUserName.readAll();
        if (!strUserName.isEmpty())
        {
            m_lineEditUserName->setText(strUserName);
        }
        fileUserName.close();
    }
    else  // 第一次登录，提示"请输入用户名："
    {
        m_lineEditUserName->setPlaceholderText(QString::fromLocal8Bit("请输入用户名："));
    }

    // 提示标签
    m_labelHint = new QLabel(frameBackground);
    font.setPixelSize(12);
    font.setWeight(50);
    m_labelHint->setFont(font);
    palette.setColor(QPalette::WindowText, QColor(255, 55, 55));
    m_labelHint->setPalette(palette);
    m_labelHint->move(m_lineEditUserName->x(),170);
    m_labelHint->hide();

    // 登录按钮
    m_btnLogin = new QPushButton(frameBackground);
    m_btnLogin->resize(90,32);
    m_btnLogin->move(m_lineEditUserName->x()+m_lineEditUserName->width()-m_btnLogin->width(),225);
    font.setPixelSize(14);
    font.setWeight(QFont::Normal);
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    m_btnLogin->setFont(font);
    m_btnLogin->setPalette(palette);
    m_btnLogin->setText(QString::fromLocal8Bit("立即登录"));

    // 退出按钮
    m_btnQuit = new QPushButton(frameBackground);
    m_btnQuit->resize(90,32);
    m_btnQuit->move(m_lineEditUserName->x(),225);
    m_btnQuit->setFont(font);
    m_btnQuit->setPalette(palette);
    m_btnQuit->setText(QString::fromLocal8Bit("退出"));

    // 加载动画
    m_labelLoading = new QLabel(frameBackground);
    m_movieLoading = new QMovie("resources/loading.gif");
    m_movieLoading->setScaledSize(QSize(40,40));
    m_movieLoading->start();
    m_labelLoading->setMovie(m_movieLoading);
    m_labelLoading->move(m_btnLogin->x()+20,m_btnLogin->y()-8);
    m_labelLoading->hide();

    // 绑定槽函数
    connect(m_btnLogin, SIGNAL(clicked(bool)), this, SLOT(onBtnLoginClicked()));
    connect(m_btnQuit, SIGNAL(clicked(bool)), this, SLOT(onBtnQuitClicked()));

    UIController* uiController=UIController::getInstance();
    qRegisterMetaType<u16>("u16");
    qRegisterMetaType<clt_ui_connect_result_ntf>("clt_ui_connect_result_ntf");
    connect(uiController, SIGNAL(sigServerStatus(u16)), this, SLOT(onNotifyServerStatus(u16)));
    connect(uiController, SIGNAL(sigConnectReult(clt_ui_connect_result_ntf)), this, SLOT(onNotifyConnectResult(clt_ui_connect_result_ntf)));
}

LoginScreen::~LoginScreen()
{
    if (NULL != m_movieLoading)
    {
        m_movieLoading->stop();
        delete m_movieLoading;
        m_movieLoading=NULL;
    }

    if (NULL != m_frameShadowEffect)
    {
        delete m_frameShadowEffect;
        m_frameShadowEffect = NULL;
    }
}

void LoginScreen::onBtnQuitClicked()
{
    printf("[%s]: called, quit application", __FUNCTION__);
    QApplication::quit();
}

void LoginScreen::onNotifyServerStatus(u16 eventType)
{
    printf("[%s]: called, eventType: %d\n", __FUNCTION__, eventType);

    switch (eventType)
    {
    case EV_CLT_UI_REGIST_ACK:
    case EV_CLT_UI_LOG_IN_ACK:
    {
        printf("[%s]: server online\n", __FUNCTION__, eventType);
        break;
    }
    case EV_CLT_UI_REGIST_NACK:
    case EV_CLT_UI_LOG_IN_NACK:
    {
        printf("[%s]: server offine\n", __FUNCTION__, eventType);
        m_labelHint->setText(QString::fromLocal8Bit("服务器离线，请检查网络连接"));
        m_labelHint->show();
        m_labelLoading->hide();
        m_btnLogin->show();
        break;
    }
    default:
        break;
    }
}

void LoginScreen::onNotifyConnectResult(clt_ui_connect_result_ntf result)
{
    qDebug("[%s]: called\n", __FUNCTION__);

    if (CONNECTION_SUCCEED == result.result)  // 连接成功
    {
        // 保存用户名至文件
        QFile fileUserName("userName.txt");
        fileUserName.open(QIODevice::WriteOnly);
        if (fileUserName.isOpen())
        {
            fileUserName.write(m_userName.toUtf8());
            fileUserName.close();
        }
        else
        {
            qDebug("[%s]: error: cannot open file userName.txt", __FUNCTION__);
        }
        qDebug("[%s]: login succeed\n", __FUNCTION__);
        emit sigConnectSucceed(m_userName);
    }
    else if (CLIENT_NUMBER_LIMIT == result.result)  // 服务端已达最大用户数限制
    {
        qDebug("[%s]: login failed because of number limit\n", __FUNCTION__);
        m_labelHint->setText(QString::fromLocal8Bit("登录失败，客户端已达最大用户数限制"));
        m_labelHint->show();
        m_labelLoading->hide();
        m_btnLogin->show();
    }
    else if (OCCUPIED_USERNAME == result.result)    // 用户名被占用
    {
        qDebug("[%s]: login failed because of occupied username\n", __FUNCTION__);
        m_labelHint->setText(QString::fromLocal8Bit("登录失败，用户名被占用"));
        m_labelHint->show();
        m_labelLoading->hide();
        m_btnLogin->show();
    }
}

void LoginScreen::onBtnLoginClicked()
{
    printf("[%s]: called\n", __FUNCTION__);

    if (m_lineEditUserName->text().isEmpty())   // 未输入用户名
    {
        m_labelHint->setText(QString::fromLocal8Bit("用户名不能为空"));
        m_labelHint->show();
    }
    else   // 开始登录
    {
        m_labelHint->hide();
        m_btnLogin->hide();
        m_labelLoading->show();

        // 通知底层
        m_userName = m_lineEditUserName->text();  // 用户输入的用户名
        QString lastUserName;     // 上一次登录时使用的用户名
        QFile fileUserName("userName.txt");
        if (fileUserName.exists())
        {
            fileUserName.open(QIODevice::ReadOnly);
            if (fileUserName.isOpen())
            {
                lastUserName = fileUserName.readAll();
                fileUserName.close();
            }
            qDebug("[%s]: lastUserName: %s,UserName: %s\n", __FUNCTION__, lastUserName.toLocal8Bit().data(), m_userName.toLocal8Bit().data());
        }
        if (lastUserName == m_userName)  // 通知底层用户登录
        {
            printf("[%s]: send log in REQ,userName is: %s\n", __FUNCTION__, m_userName.toLocal8Bit().data());
            ui_clt_log_in_req registMsg;
            strcpy(registMsg.userName, m_userName.toLocal8Bit().data());
            CLIENTAPP::Post(EV_UI_CLT_LOG_IN_REQ, &registMsg, sizeof(registMsg));
        }
        else     // 通知底层用户注册
        {
            printf("[%s]: send regist REQ,userName is: %s\n", __FUNCTION__, m_userName.toLocal8Bit().data());
            ui_clt_regist_req registMsg;
            strcpy(registMsg.userName, m_userName.toLocal8Bit().data());
            CLIENTAPP::Post(EV_UI_CLT_REGIST_REQ, &registMsg, sizeof(registMsg));
        }
    }
}

