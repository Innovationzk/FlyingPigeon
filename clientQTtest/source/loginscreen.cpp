#include <QLabel>
#include <QFont>
#include <QFile>
#include <QString>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QMovie>
#include <QApplication>
#include "loginscreen.h"

#include <QDebug>

#define LOGIN_SCREEN_WIDTH 500
#define LOGIN_SCREEN_HEIGHT 350


LoginScreen::LoginScreen(QWidget *parent) : QWidget(parent)
{
    QFile file("test.txt");
    if (file.exists()) qDebug()<<file.readAll();
    else qDebug("No");
    file.close();

    this->setWindowFlags(Qt::FramelessWindowHint);  //去掉标题栏
    this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口透明
    this->setFixedSize(LOGIN_SCREEN_WIDTH, LOGIN_SCREEN_HEIGHT);

    QFrame *frame = new QFrame(this);
    frame->setFixedSize(this->width()-40,this->height()-40);
    frame->move(20,20);

    //frame->setStyleSheet("QFrame{background-color: rgb(255, 255, 255);border-radius: 24px;padding: 2px}");
    frame->setStyleSheet("QFrame{background-color: rgb(255, 255, 255);border-radius: 24px;padding: 2px}"
                         "QLineEdit{border-radius: 15px;background: rgb(235,235,235);selection-background-color: green;font-size: 12px}"
                         "QPushButton{background-color: rgb(0,180,240); border:2px; border-radius:15px; padding: 2px}");

    // 设置阴影
    m_frameShadowEffect = new QGraphicsDropShadowEffect();
    m_frameShadowEffect->setOffset(8,8);
    m_frameShadowEffect->setColor(QColor(0,0,0,100));
    m_frameShadowEffect->setBlurRadius(10);
    frame->setGraphicsEffect(m_frameShadowEffect);

    // “用户登录”
    labelLogin = new QLabel(frame);
    QFont font(QString::fromLocal8Bit("微软雅黑"),28,66);
    labelLogin->setFont(font);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(55, 55, 55));
    labelLogin->setPalette(palette);
    labelLogin->setText(QString::fromLocal8Bit("用户登录"));
    labelLogin->move(LOGIN_SCREEN_WIDTH/2-110,40);

    // 输入框
    m_lineEditUserName= new QLineEdit(frame);
    m_lineEditUserName->resize(frame->width()-100,31);
    m_lineEditUserName->setFocus(Qt::MouseFocusReason);
    m_lineEditUserName->move((frame->width()-m_lineEditUserName->width())/2,130);
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

    // 提示
    m_labelHint = new QLabel(frame);
    font.setPixelSize(12);
    font.setWeight(50);
    m_labelHint->setFont(font);
    palette.setColor(QPalette::WindowText, QColor(255, 55, 55));
    m_labelHint->setPalette(palette);
    m_labelHint->move(m_lineEditUserName->x(),170);
    m_labelHint->hide();

    m_btnLogin = new QPushButton(frame);
    m_btnLogin->resize(90,32);
    m_btnLogin->move(m_lineEditUserName->x()+m_lineEditUserName->width()-m_btnLogin->width(),225);
    font.setPixelSize(14);
    font.setWeight(QFont::Normal);
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
    m_btnLogin->setFont(font);
    m_btnLogin->setPalette(palette);
    m_btnLogin->setText(QString::fromLocal8Bit("立即登录"));

    m_btnQuit = new QPushButton(frame);
    m_btnQuit->resize(90,32);
    m_btnQuit->move(m_lineEditUserName->x(),225);
    m_btnQuit->setFont(font);
    m_btnQuit->setPalette(palette);
    m_btnQuit->setText(QString::fromLocal8Bit("退出"));

    m_labelLoading = new QLabel(frame);
    m_movieLoading = new QMovie("D:/Projects/OpsDemo/QT_Test/giphy.gif");
    m_movieLoading->setScaledSize(QSize(40,40));
    m_labelLoading->setMovie(m_movieLoading);
    m_labelLoading->move(m_btnLogin->x()+20,m_btnLogin->y()-8);
    m_labelLoading->hide();

    connect(m_btnLogin, SIGNAL(clicked(bool)), this, SLOT(onBtnLoginClicked()));
    connect(m_btnQuit, SIGNAL(clicked(bool)), this, SLOT(onBtnQuitClicked()));
    connect(m_btnQuit, SIGNAL(clicked(bool)), this, SIGNAL(btnQuitClicked()));
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

void LoginScreen::onBtnLoginClicked()
{
    if (QString("") == m_lineEditUserName->text())
    {
        m_labelHint->setText(QString::fromLocal8Bit("用户名不能为空"));
        m_labelHint->show();
    }
    else
    {
        m_labelHint->hide();
        m_btnLogin->hide();

        m_movieLoading->start();
        m_labelLoading->show();
    }


    emit btnLoginClicked(m_lineEditUserName->text());
}

void LoginScreen::onBtnQuitClicked()
{
    QApplication::quit();
}

