#pragma once

#include <QWidget>
#include <QString>

class QLabel;

class MultiChatFriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit MultiChatFriendListItem(const QString& userName, const QString& icon = "resources/friendicon.png");

    int height();

    QString getFriendName();

signals:

public slots:

private:
    QLabel *m_labIcon;
    QLabel *m_labUserName;
    QString m_userName;
    int m_height;
};


