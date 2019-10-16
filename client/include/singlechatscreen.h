#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QPushButton;
class QListWidget;
class QPlainTextEdit;
class FriendListItem;

class SingleChatScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SingleChatScreen(QString m_userName="unknown", unsigned int m_clientNo=0);

    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void sigSingleChatScreenClose(unsigned int m_clientNo);

public slots:
    void onBtnSendClicked();
    void onReceiveMsg(const QString &msg);

private:
    QLabel *m_labFriendIcon;
    QLabel *m_labFriendName;
    QPushButton *m_btnSend;
    QPushButton *m_btnCancel;
    QString m_userName;
    unsigned int m_clientNo;
    QWidget *m_friendWidget;
    QListWidget *m_chatListWidget;
    QPlainTextEdit *m_inputWidget;
};


