#pragma once

#include <QWidget>

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
    void showMainWindow();
    void btnQuitClicked();
    void btnLoginClicked(const QString& userName);

public slots:
    void onBtnLoginClicked();
    void onBtnQuitClicked();

private:
    QLabel *m_labelLoading;         // 文字：用户登录
    QLineEdit *m_lineEditUserName;  // 输入框
    QLabel *m_labelHint;            // 提示
    QPushButton *m_btnLogin;        // 登录按钮
    QPushButton *m_btnQuit;         // 退出按钮
    QMovie *m_movieLoading;         // 加载动画
    QLabel *labelLogin;             // 加载按钮
    QGraphicsDropShadowEffect *m_frameShadowEffect;
};


