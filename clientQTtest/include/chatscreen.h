#pragma once

#include <QWidget>
#include <QString>

class QLabel;
class QListWidget;
class FriendListItem;
class QTextEdit;

class ChatScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ChatScreen(QWidget *parent = NULL);


signals:

public slots:

private:
    QListWidget *m_friendListWidget;
    QListWidget *m_chatListWidget;
    QTextEdit *m_textEdit;
};


