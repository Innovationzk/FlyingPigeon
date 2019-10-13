#include <QListWidget>
#include <QListWidgetItem>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "chatscreen.h"
#include "friendlistitem.h"

ChatScreen::ChatScreen(QWidget *parent)
{
    QListWidget *m_friendListWidget=new QListWidget(this);
    m_friendListWidget->setFixedWidth(200);
    QListWidgetItem *listWidgetItem=new QListWidgetItem(m_friendListWidget);
    FriendListItem *friendListItem=new FriendListItem("小明");
    m_friendListWidget->setItemWidget(listWidgetItem,friendListItem);


    QListWidget *m_chatListWidget=new QListWidget(this);

    m_textEdit=new QTextEdit(this);

    QHBoxLayout vLayout;
    vLayout.addWidget(m_chatListWidget);
    vLayout.addWidget(m_textEdit);
    vLayout.setStretch(0,3);
    vLayout.setStretch(0,2);

    QHBoxLayout layout;
    layout.addWidget(m_friendListWidget);
    layout.addItem(&vLayout);
}
