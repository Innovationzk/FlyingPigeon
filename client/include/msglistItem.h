#pragma once

#include <QWidget>
#include <QString>
#include <QDateTime>

class QLabel;

class MsgListItem : public QWidget
{
    Q_OBJECT
public:
    explicit MsgListItem(const QString& msg = "", bool isFriendMsg = false,
                         QDateTime dateTime = QDateTime::currentDateTime());

    int height() const
    {
        return m_height;
    }

signals:

public slots:

private:
    QLabel *m_labIcon;
    QLabel *m_labDateTime;
    QLabel *m_labContent;
    int m_height;
};


