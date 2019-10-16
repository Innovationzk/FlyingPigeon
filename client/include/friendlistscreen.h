#pragma once
#include <QWidget>
#include <QMap>
#include <QString>
#include "commondef.h"

class QLabel;
class QPushButton;
class QListWidgetItem;
class QListWidget;
class FriendListItem;
class SendFileScreen;
class SingleChatScreen;
class SelectFriendScreen;

class FriendListScreen : public QWidget
{
    Q_OBJECT

public:
    explicit FriendListScreen(QWidget *parent = 0);
    ~FriendListScreen();

    void setUserName(const QString& userName);
    unsigned int getClientNo(const QString& userName);

    static FriendListScreen* getInstance();

    void closeEvent(QCloseEvent *);

    // 恢复未完成的文件发送窗口
    void restoreSendFileScreen();

signals:

public slots:
    // 客户端列表更新
    void onNotifyClientList(clt_ui_regist_list_ntf list);
    // 双击好友列表
    void onListItemDoubleClicked(QListWidgetItem* listWidgetItem);
    // 一对一聊天窗口关闭
    void onSingleChatScreenClose(unsigned int clientNo);
    // 点击群发消息按钮
    void onBtnMassMsgClicked();
    // 选择好友界面关闭
    void onSelectFriendsSceenClosed();
    // 打开单人聊天界面
    void onOpenSingleChatScreen(const QString &clientName,unsigned int clientNo);
    // 服务器离线，消息发送失败
    void onNotifySendMsgNack();
    // 收到消息
    void onReceiveMsg(clt_ui_receive_msg_ntf msg);
    // 连接断开
    void onNotifyServerDisconnect();
    // 连接成功
    void onNotifyConnectReult(clt_ui_connect_result_ntf result);

private:
    QLabel* m_labUserIcon;            // 用户头像
    QLabel* m_labUserName;            // 用户名界面
    QLabel* m_labUserStatus;          // 用户状态
    QPushButton *m_btnMassMsg;        // 群发消息
    QListWidget *m_friendListWidget;  // 好友界面
    QString m_userName;
    QMap<QString, FriendListItem*> m_friendList;  // 好友列表
    QMap<unsigned int,SingleChatScreen*> m_chatScreenList;  // 已经打开的一对一聊天对话框
    SelectFriendScreen *m_selectFriendsSceen;    // 选择好友界面
    SendFileScreen *m_sendFileScreen;
    static FriendListScreen *sm_friendListScreen;
};


