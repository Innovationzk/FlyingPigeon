#pragma once

#include <QWidget>
#include <QString>

class QLabel;

class FriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FriendListItem(const QString& userName = "unknown", const QString& icon = "resources/usericon.png");

    void setUserName(const QString& userName);

signals:

public slots:

private:
    QLabel *m_labIcon;
    QLabel *m_labUserName;
    QLabel *m_labDot;
};


