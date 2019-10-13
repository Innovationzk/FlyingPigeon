#include <QLabel>
#include <QDebug>
#include <QStringList>
#include <QFontMetrics>
#include <QPlainTextEdit>
#include "msglistitem.h"

MsgListItem::MsgListItem(const QString &name, const QString &msg, bool isFriendMsg, QDateTime dateTime)
{
    m_labIcon = new QLabel(this);
    QPixmap pixmap;
    if (isFriendMsg)
    {
        pixmap.load("resources/friendicon.png");
    }
    else
    {
        pixmap.load("resources/usericon.svg");
    }
    m_labIcon->setPixmap(pixmap.scaled(30,30,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labIcon->move(10,15);

    m_labDateTime = new QLabel(this);
    QFont font(QString::fromLocal8Bit("Î¢ÈíÑÅºÚ"),9);
    QPalette palette;
    palette.setColor(QPalette::Text, QColor(100, 100, 100));
    m_labDateTime->setFont(font);
    m_labDateTime->setPalette(palette);
    m_labDateTime->setText(dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    m_labDateTime->move(55,5);

    m_labContent=new QLabel(this);
    m_labContent->setText(msg);
    m_labContent->setScaledContents(true);
    m_labContent->setMargin(5);
    m_labContent->move(m_labDateTime->x(),25);
    if(isFriendMsg)
    {
        m_labContent->setStyleSheet("QLabel{background-color: white;"
            "font: 11pt;"
            "color: rgb(0, 0, 0);"
            "border:2px;"
            "border-radius:11px;}"
            "padding: 2px}");
    }
    else
    {
        m_labContent->setStyleSheet("QLabel{background-color: rgb(0,180,240);"
            "font: 11pt;"
            "color: rgb(255, 255, 255);"
            "border:2px;"
            "border-radius:11px;"
            "padding: 2px}");
    }

    int returnCount=msg.count('\n')+1;
    qDebug()<<"returnCount="<<returnCount;
    m_height=m_labDateTime->height()+returnCount*17+9;
}
