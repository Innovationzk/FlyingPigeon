#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include "friendlistitem.h"

FriendListItem::FriendListItem(const QString& userName, unsigned int clientNo, const QString& icon):
    m_friendName(userName),
    m_clientNo(clientNo),
    m_status(true)
{
    // 设置好友头像
    m_labFriendIcon = new QLabel(this);
    QPixmap pixmap(icon);
    m_labFriendIcon->setPixmap(pixmap.scaled(35,35,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labFriendIcon->move(15,15);

    // 在线标志
    m_labGreenDot = new QLabel(this);
    pixmap.load("resources/greendot.png");
    m_labGreenDot->setPixmap(pixmap.scaled(7,7,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labGreenDot->move(62,28);

    // 离线标志
    m_labGreyDot = new QLabel(this);
    pixmap.load("resources/greydot.png");
    m_labGreyDot->setPixmap(pixmap.scaled(7, 7, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    m_labGreyDot->move(62, 28);
    m_labGreyDot->hide();

    // 好友用户名
    m_labUserName = new QLabel(this);
    m_labUserName->setPixmap(pixmap.scaled(8,8,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    QFont font(QString::fromLocal8Bit("微软雅黑"), 11);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(55, 55, 55));
    m_labUserName->setFont(font);
    m_labUserName->setPalette(palette);
    m_labUserName->setText(userName);
    m_labUserName->move(78,20);
}

void FriendListItem::setStatus(bool isOnline)
{
    m_status = isOnline;
    if (isOnline)
    {
        m_labGreenDot->show();
        m_labGreyDot->hide();
    }
    else
    {
        m_labGreenDot->hide();
        m_labGreyDot->show();
    }
}

QString FriendListItem::getFriendName()
{
    return m_friendName;
}

unsigned int FriendListItem::getclientNo()
{
    return m_clientNo;
}
