#pragma once

#include <QWidget>
#include <QString>

class QLabel;

class FriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FriendListItem(const QString& userName = "unknown", unsigned int clientNo = 0, const QString & icon = "resources/friendicon.png");

    // 设置在线或离线状态
    void setStatus(bool isOnline);

    QString getFriendName();
    unsigned int getclientNo();

signals:

public slots:

private:
    QLabel* m_labFriendIcon; // 好友头像
    QLabel* m_labUserName;   // 好友用户名
    QLabel* m_labGreenDot;   // 在线标志
    QLabel* m_labGreyDot;    // 离线标志
    bool m_status;           // 状态（在线或离线）
    QString m_friendName;    // 好友用户名
    unsigned int m_clientNo; // 好友客户端编号
};

