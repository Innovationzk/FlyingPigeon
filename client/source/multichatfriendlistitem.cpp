#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include "multichatfriendlistitem.h"

MultiChatFriendListItem::MultiChatFriendListItem(const QString &userName, const QString &icon):
m_userName(userName)
{
    // 设置好友头像
    m_labIcon = new QLabel(this);
    QPixmap pixmap(icon);
    m_labIcon->setPixmap(pixmap.scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labIcon->move(10,5);

    // 好友用户名
    m_labUserName = new QLabel(this);
    m_labUserName->setText(userName);
    m_labUserName->move(60,10);
    m_labUserName->setStyleSheet(QString::fromLocal8Bit("QLabel{font: 12pt;"
        "color:rgb(55, 55, 55);"
        "font-family:\"微软雅黑\";}"));

    m_height=40;
}

int MultiChatFriendListItem::height()
{
    return m_height;
}

QString MultiChatFriendListItem::getFriendName()
{
    return m_userName;
}
