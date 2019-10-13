#pragma once

#include <QWidget>
#include <QString>
#include <QVector>

class QLabel;
class QPushButton;
class QListWidget;
class QPlainTextEdit;
class FriendListItem;

class MultiChatScreen : public QWidget
{
    Q_OBJECT
public:
    explicit MultiChatScreen(const QVector<QString> &vecFriendName, const QVector<unsigned int> &clientNo);

    void paintEvent(QPaintEvent *event);


signals:

public slots:
    void onBtnSendClicked();

private:
    QVector<unsigned int> m_vecCliendNo;
    QPushButton *m_btnSend;
    QPushButton *m_btnCancel;
    QListWidget *m_friendListWidget;
    QListWidget *m_chatListWidget;
    QPlainTextEdit *m_inputWidget;
};


