#pragma once

#include <QWidget>
#include <QMap>
#include <QString>
#include <../common/commondef.h>

class QLabel;
class QListWidgetItem;
class QListWidget;
class FriendListItem;

class FriendListScreen : public QWidget
{
    Q_OBJECT
public:

    explicit FriendListScreen(QWidget *parent = 0);
    QString getUserName();
    ~FriendListScreen();

signals:

public slots:
    void onUpdateClientList(clt_ui_regist_list_ntf *list);
    void onItemClicked(QListWidgetItem * item);

private:
    QLabel* m_labUserIcon;
    QLabel* m_labUserName;
    QLabel* m_labUserStatus;
    QListWidget *m_listWidget;
    QMap<int, FriendListItem> m_friendList;
};

