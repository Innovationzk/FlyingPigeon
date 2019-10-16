#include <QLabel>
#include <QPixmap>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>
#include "commondef.h"
#include "friendlistscreen.h"
#include "friendlistitem.h"
#include "singlechatscreen.h"
#include "multichatscreen.h"
#include "multichatfriendlistitem.h"
#include "sendfilescreen.h"
#include "selectfriendscreen.h"
#include "uicontroller.h"

FriendListScreen* FriendListScreen::sm_friendListScreen = NULL;

FriendListScreen::FriendListScreen(QWidget* parent):
    QWidget(parent),
    m_selectFriendsSceen(NULL)
{
    sm_friendListScreen = this;

    // 设置窗口属性
    this->setWindowTitle(QString::fromLocal8Bit("客户端"));
    this->resize(280, 600);

    // 初始化用户信息
    QWidget* userInfo = new QWidget(this);
    userInfo->setMinimumSize(280, 100);

    // 用户图标
    m_labUserIcon = new QLabel(userInfo);
    QPixmap userIcon("resources/usericon.svg");
    m_labUserIcon->setPixmap(userIcon.scaled(62, 62, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    m_labUserIcon->move(15, 15);

    // 用户名
    m_labUserName = new QLabel(userInfo);
    QFont font(QString::fromLocal8Bit("微软雅黑"), 12);
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(55, 55, 55));
    m_labUserName->setFont(font);
    m_labUserName->setPalette(palette);
    m_labUserName->setText(QString::fromLocal8Bit("用户："));
    m_labUserName->move(100, 20);

    // 用户状态
    m_labUserStatus = new QLabel(userInfo);
    m_labUserStatus->move(100, 50);
    m_labUserStatus->setFont(font);
    m_labUserStatus->setPalette(palette);
    m_labUserStatus->setText(QString::fromLocal8Bit("状态：在线"));

    // 好友列表
    m_friendListWidget = new QListWidget(this);
    m_friendListWidget->setFrameStyle(QFrame::NoFrame);
    //m_friendListWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // 添加“我的服务器”
    FriendListItem* itemMyServer = new FriendListItem(QString("My server"), 0, QString("resources/servericon.svg"));
    QListWidgetItem* listWidgetItemm = new QListWidgetItem();
    listWidgetItemm->setSizeHint(QSize(250, 65));
    m_friendListWidget->addItem(listWidgetItemm);
    m_friendListWidget->setItemWidget(listWidgetItemm, itemMyServer);

    // “群发消息”按钮
    QWidget *massMsgContainer = new QWidget(this);
    massMsgContainer->setFixedHeight(30);
    //massMsgContainer->setStyleSheet("QWidget{background-color:white}");

    m_btnMassMsg = new QPushButton(massMsgContainer);
    m_btnMassMsg->setFixedSize(100,massMsgContainer->height());
    m_btnMassMsg->setText(QString::fromLocal8Bit("群发消息"));
    m_btnMassMsg->setStyleSheet("QPushButton{background-color:rgb(0,180,240);color:rgb(255,255,255);"
                                 "font: 11pt;font-family:Microsoft YaHei UI;"
                                 "border-radius:15px;padding: 2px}");

    QHBoxLayout* massMsgLayout = new QHBoxLayout(massMsgContainer);
    massMsgLayout->setMargin(0);
    massMsgLayout->addStretch();
    massMsgLayout->addWidget(m_btnMassMsg);
    massMsgLayout->addSpacing(20);

    // 设置窗口布局
    QVBoxLayout* vLabelLayout = new QVBoxLayout();
    vLabelLayout->addWidget(userInfo);
    vLabelLayout->addWidget(m_friendListWidget);
    vLabelLayout->addWidget(massMsgContainer);
    this->setLayout(vLabelLayout);

    // 发送文件窗口
    m_sendFileScreen = new SendFileScreen();
    QDir().mkdir("fileList");

    // 绑定信号
    UIController* uiController = UIController::getInstance();
    connect(m_friendListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(onListItemDoubleClicked(QListWidgetItem*)));
    connect(m_btnMassMsg,SIGNAL(clicked(bool)),this,SLOT(onBtnMassMsgClicked()));
    qRegisterMetaType<clt_ui_regist_list_ntf>("clt_ui_regist_list_ntf");
    connect(uiController, SIGNAL(sigRegistList(clt_ui_regist_list_ntf)), this, SLOT(onNotifyClientList(clt_ui_regist_list_ntf)));
    qRegisterMetaType<clt_ui_receive_msg_ntf>("clt_ui_receive_msg_ntf");
    connect(uiController, SIGNAL(sigReceiveMsg(clt_ui_receive_msg_ntf)),this,SLOT(onReceiveMsg(clt_ui_receive_msg_ntf)));
    connect(uiController,SIGNAL(sigDisconnect()),this,SLOT(onNotifyServerDisconnect()));
    connect(uiController,SIGNAL(sigConnectReult(clt_ui_connect_result_ntf)),this,SLOT(onNotifyConnectReult(clt_ui_connect_result_ntf)));
}

FriendListScreen::~FriendListScreen()
{

}

void FriendListScreen::setUserName(const QString& userName)
{
    m_userName = userName;
    m_labUserName->setText(QString::fromLocal8Bit("用户：") + userName);
}

unsigned int FriendListScreen::getClientNo(const QString &userName)
{
    printf("[%s]: called\n", __FUNCTION__);

    if (m_friendList.find(userName)==m_friendList.end())
    {
        printf("[%s]: errot: friend %s not exist\n", __FUNCTION__, userName.toUtf8());
        return 0;
    }
    return m_friendList[userName]->getclientNo();
}

FriendListScreen *FriendListScreen::getInstance()
{
    return sm_friendListScreen;
}

void FriendListScreen::closeEvent(QCloseEvent *)
{
    QApplication::quit();
}

void FriendListScreen::restoreSendFileScreen()
{
    m_sendFileScreen->restoreExistFileList();
}

void FriendListScreen::onNotifyClientList(clt_ui_regist_list_ntf list)
{
    printf("[%s]: called\n", __FUNCTION__);

    // 将所有好友全部置为离线
    for (QMap<QString, FriendListItem*>::iterator it = m_friendList.begin(); it != m_friendList.end(); ++it)
    {
        it.value()->setStatus(false);
    }

    // 遍历客户端列表，更新在线客户端并添加新客户端
    char* name = list.clientName;
    for (int i = 0; i < list.clientNum; i++)
    {
        QString clientName(QString::fromLocal8Bit(name));
        if (m_friendList.find(clientName) != m_friendList.end())  // 客户端在列表中
        {
            m_friendList[clientName]->setStatus(true);
        }
        else if(clientName!= m_userName)  // 新客户端上线
        {
            printf("[%s]: add new client %s\n", __FUNCTION__, name);
            FriendListItem* clientListItem = new FriendListItem(clientName, list.clientNo[i]);
            QListWidgetItem* listWidgetItemm = new QListWidgetItem();
            listWidgetItemm->setSizeHint(QSize(250, 65));
            m_friendListWidget->addItem(listWidgetItemm);
            m_friendListWidget->setItemWidget(listWidgetItemm, clientListItem);
            m_friendList[clientName] = clientListItem;
        }

        // 将name移动到下一个用户名的位置
        while ('\0' != *name)
        {
            name++;
        }
        name++;
    }
}

void FriendListScreen::onListItemDoubleClicked(QListWidgetItem *listWidgetItem)
{
    qDebug()<<__FUNCTION__;

    FriendListItem *friendLiistItem = (FriendListItem *)(m_friendListWidget->itemWidget(listWidgetItem));
    unsigned int clientNo = friendLiistItem->getclientNo();
    QString clientName = friendLiistItem->getFriendName();

    qDebug()<<"friendLiistItem:"<<friendLiistItem<<"clientNo:"<<clientNo<<",clientName:"<<clientName.toUtf8().data();
    qDebug()<<"m_sendFileScreen:"<<m_sendFileScreen;
    if (clientName == QString("My server"))
    {
        qDebug()<<"My server DoubleClicked,m_sendFileScreen:"<<m_sendFileScreen;
        m_sendFileScreen->show();
        //m_sendFileScreen->restoreExistFileList();
        return;
    }
    qDebug()<<"onOpenSingleChatScreen,clientName:"<<clientName<<",clientNo:"<<clientNo;
    onOpenSingleChatScreen(clientName,clientNo);
}

void FriendListScreen::onSingleChatScreenClose(unsigned int clientNo)
{
    if (m_chatScreenList.find(clientNo)==m_chatScreenList.end())
    {
        printf("[%s]: error:clientNo %d not exist\n",__FUNCTION__,clientNo);
        return;
    }
    m_chatScreenList.remove(clientNo);
}

void FriendListScreen::onBtnMassMsgClicked()
{
    if (m_friendList.isEmpty())
    {
        QMessageBox::information(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("暂时没有好友"));
        return;
    }

    if (NULL == m_selectFriendsSceen)
    {
        QVector<QString> vecFriendName;
        for (QMap<QString, FriendListItem*>::iterator it = m_friendList.begin();it != m_friendList.end();++it)
        {
            vecFriendName.push_back(it.key());
        }
        m_selectFriendsSceen = new SelectFriendScreen(vecFriendName);
        connect(m_selectFriendsSceen,SIGNAL(sigCloseScreen()),this,SLOT(onSelectFriendsSceenClosed()));
        connect(m_selectFriendsSceen,SIGNAL(sigOpenSingleChatScreen(QString,uint)),this,SLOT(onOpenSingleChatScreen(QString,uint)));
        m_selectFriendsSceen->show();
    }
    else
    {
        m_selectFriendsSceen->hide();
        m_selectFriendsSceen->show();
    }
}

void FriendListScreen::onSelectFriendsSceenClosed()
{
    qDebug()<<__FUNCTION__;
    disconnect(m_selectFriendsSceen,SIGNAL(sigCloseScreen()),this,SLOT(onSelectFriendsSceenClosed()));
    disconnect(m_selectFriendsSceen,SIGNAL(sigOpenSingleChatScreen(QString,uint)),this,SLOT(onOpenSingleChatScreen(QString,uint)));
    m_selectFriendsSceen=NULL;
}

void FriendListScreen::onOpenSingleChatScreen(const QString &clientName, unsigned int clientNo)
{
    if (m_chatScreenList.find(clientNo)==m_chatScreenList.end())
    {
        SingleChatScreen *screen = new SingleChatScreen(clientName,clientNo);
        screen->show();
        m_chatScreenList[clientNo]=screen;
        connect(screen,SIGNAL(sigSingleChatScreenClose(unsigned int)),this,SLOT(onSingleChatScreenClose(unsigned int)));
    }
    else
    {
        m_chatScreenList[clientNo]->hide();
        m_chatScreenList[clientNo]->show();
    }
}

void FriendListScreen::onNotifySendMsgNack()
{
    QMessageBox::information(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("消息发送失败，请检查网络连接"));
}

void FriendListScreen::onReceiveMsg(clt_ui_receive_msg_ntf msg)
{
    u32 srcClientNo=msg.srcClientNo;
    QString friendName;

    for (QMap<QString, FriendListItem*>::iterator it = m_friendList.begin();it!=m_friendList.end();++it)
    {
        if (it.value()->getclientNo()==srcClientNo)
        {
            friendName=it.key();
            break;
        }
    }
    if (friendName.isEmpty())
    {
        qDebug("[%s]: error: srcCliendNo %d friendName not found\n", __FUNCTION__, srcClientNo);
        return;
    }

    if (m_chatScreenList.find(srcClientNo)==m_chatScreenList.end())
    {
        SingleChatScreen *screen = new SingleChatScreen(friendName,srcClientNo);
        screen->show();
        m_chatScreenList[srcClientNo]=screen;
        connect(screen,SIGNAL(sigSingleChatScreenClose(unsigned int)),this,SLOT(onSingleChatScreenClose(unsigned int)));
        screen->onReceiveMsg(QString::fromLocal8Bit(msg.msgContent));
    }
    else
    {
        m_chatScreenList[srcClientNo]->hide();
        m_chatScreenList[srcClientNo]->show();
        m_chatScreenList[srcClientNo]->onReceiveMsg(QString::fromLocal8Bit(msg.msgContent));
    }
}

void FriendListScreen::onNotifyServerDisconnect()
{
    QMessageBox::information(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("服务器连接断开，正在尝试重新连接"));
    m_sendFileScreen->onBtnPauseAllClicked();
    m_labUserStatus->setText(QString::fromLocal8Bit("状态：离线"));
}

void FriendListScreen::onNotifyConnectReult(clt_ui_connect_result_ntf result)
{
    if (CONNECTION_SUCCEED == result.result)  // 连接成功
    {
        m_labUserStatus->setText(QString::fromLocal8Bit("状态：在线"));
    }
}

//void FriendListScreen::onSendFileScreenClose()
//{
//    qDebug()<<("[%s] called");
//    disconnect(m_sendFileScreen,SIGNAL(sigCloseEvent()),this,SLOT(onSendFileScreenClose()));
//    m_sendFileScreen=NULL;
//}




