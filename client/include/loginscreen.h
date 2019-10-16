#pragma once
#include <QWidget>
#include <QString>
#include "commondef.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QMovie;
class QGraphicsDropShadowEffect;

class LoginScreen : public QWidget
{
    Q_OBJECT
public:
    explicit LoginScreen(QWidget *parent = 0);
    ~LoginScreen();

signals:
    void sigConnectSucceed(QString userName);

public slots:
    void onBtnLoginClicked();
    void onBtnQuitClicked();

    // 底层通知服务器状态
    void onNotifyServerStatus(u16 eventType);
    // 底层通知登录结果
    void onNotifyConnectResult(clt_ui_connect_result_ntf result);

private:
    QString m_userName;             // 用户名
    QLabel *m_labelLoading;         // 文字：用户登录
    QLabel* m_labelHint;            // 提示
    QLabel* labelLogin;             // 加载按钮
    QLineEdit *m_lineEditUserName;  // 输入框
    QPushButton *m_btnLogin;        // 登录按钮
    QPushButton *m_btnQuit;         // 退出按钮
    QMovie *m_movieLoading;         // 加载动画
    QGraphicsDropShadowEffect *m_frameShadowEffect;
};

