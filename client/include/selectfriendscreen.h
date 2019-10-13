#pragma once
#include <QWidget>
#include <QVector>
#include <QString>

class QPushButton;
class QListWidget;

class SelectFriendScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SelectFriendScreen(const QVector<QString>& friendName);
    ~SelectFriendScreen();

    void closeEvent(QCloseEvent *);

signals:
    void sigOpenChatScreen();
    void sigCloseScreen();
    void sigOpenSingleChatScreen(QString friendName,unsigned int clientNo);

public slots:
    void onBtnConfirmClicked();
    void onBtnCancelClicked();

private:
    QPushButton *m_btnConfirm;        // 确认按钮
    QPushButton *m_btnCancel;         // 取消按钮
    QListWidget *m_friendListWidget;  // 好友列表
};


