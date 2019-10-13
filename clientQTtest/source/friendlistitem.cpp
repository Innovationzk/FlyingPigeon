#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include "friendlistitem.h"

FriendListItem::FriendListItem(const QString& userName, const QString& icon)
{
    m_labIcon = new QLabel(this);
    QPixmap pixmap(icon);
    m_labIcon->setPixmap(pixmap.scaled(35,35,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labIcon->move(15,15);

    m_labDot = new QLabel(this);
    pixmap.load("resources/greendot.svg");
    m_labDot->setPixmap(pixmap.scaled(7,7,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labDot->move(60,28);

    QFont font(QString::fromLocal8Bit("微软雅黑"),11);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(55, 55, 55));

    m_labUserName = new QLabel(this);
    m_labUserName->setPixmap(pixmap.scaled(8,8,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labUserName->setFont(font);
    m_labUserName->setPalette(palette);
    m_labUserName->setText(userName);
    m_labUserName->move(74,20);
}

void FriendListItem::setUserName(const QString &userName)
{
    m_labUserName->setText(userName);
}

