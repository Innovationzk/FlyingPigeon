#include "friendlistscreen.h"
#include "friendlistitem.h"
#include "chatscreen.h"
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QProcess>
#include <QFont>
#include <QRegExp>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTimer>
#include <QProgressBar>
#include <QPushButton>
#include <QVector>
#include <QSet>
#include <QFile>
#include <QThread>
#include <QVector>

FriendListScreen::FriendListScreen(QWidget *parent) : QWidget(parent)
{
    // 设置窗口属性
    this->setWindowTitle(QString::fromLocal8Bit("客户端"));
    this->resize(280, 600);

    // 初始化用户信息
    QWidget* userInfo = new QWidget(this);
    userInfo->setMinimumSize(280, 100);

    // 用户图标
    m_labUserIcon = new QLabel(userInfo);
    QPixmap userIcon("./resources/usericon.svg");
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
    m_labUserStatus->setText(QString::fromLocal8Bit("状态：离线"));

    // 好友列表
    m_listWidget = new QListWidget(this);
    m_listWidget->setFrameStyle(QFrame::NoFrame);

    // 添加“我的服务器”
    FriendListItem* itemMyServer3 = new FriendListItem(QString("My server"), QString("resources/servericon.svg"));
    QListWidgetItem* listWidgetItemm13 = new QListWidgetItem();
    listWidgetItemm13->setSizeHint(QSize(250, 65));
    m_listWidget->addItem(listWidgetItemm13);
    m_listWidget->setItemWidget(listWidgetItemm13, itemMyServer3);

    // 设置窗口布局
    QVBoxLayout* vLabelLayout = new QVBoxLayout();
    vLabelLayout->addWidget(userInfo);
    vLabelLayout->addWidget(m_listWidget);
    this->setLayout(vLabelLayout);

    connect(m_listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(onItemClicked(QListWidgetItem*)));
}

QString FriendListScreen::getUserName(){
    QStringList envVariables;
    envVariables << "^USERNAME.*" << "^USER.*" << "^USERDOMAIN.*" << "^HOSTNAME.*" << "^DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    for (int i = 0; i < envVariables.size(); i++) {
        QStringList matchEnvironment = environment.filter(QRegExp(envVariables[i]));
        for (int j = 0; j < matchEnvironment.size(); j++) {
            QStringList stringList = matchEnvironment[j].split('=');
            if (stringList.size() == 2) {
                return stringList[1];
            }
        }
    }
    return "unknown";
}

FriendListScreen::~FriendListScreen()
{

}

void FriendListScreen::onUpdateClientList(clt_ui_regist_list_ntf *list)
{
    /*// 解析数据
    QMap<int,QString> newClientList;
    char *name = list->userName;
    for (int i = 0; i < list->clientNum; i++)
    {
        newClientList[list->clientNode[i]]=QString::fromLocal8Bit(name);

        // 将name移动到下一个用户名的位置
        while('\0' != *name) name++;
        name++;
    }

    for (auto it=newClientList.begin(); it != newClientList.end(); it++) qDebug()<<"key:"<<it.key()<<"  value="<<it.value();

    // 删除已经离线的好友
    QVector<int> offineList;
    // 标记所有需要删除的节点号
    for (QMap<int,ClientInfo>::iterator it = m_clientList.begin(); it != m_clientList.end(); it++)
    {
        if (newClientList.find(it.key()) == newClientList.end())
        {
            offineList.push_back(it.key());
        }
    }
    // 释放内存，并将m_clientList中相应数据删除
    for (int i =0; i < offineList.size(); i++)
    {
        int row = m_listWidget->row(m_clientList[offineList[i]].listWidgetItem);
        if (0 == row)
        {
            qDebug()<<"error newclientList";
            return;
        }
        if (0 == m_listWidget->takeItem(row))
        {
            qDebug()<<"error takeItem";
            return;
        }
        delete m_clientList[offineList[i]].listItem;
        m_clientList.remove(offineList[i]);
    }

    // 添加新上线的好友
    for (QMap<int,QString>::iterator it = newClientList.begin(); it != newClientList.end(); it++)
    {
        // 如果该节点不在m_clientList中，则添加节点
        if (m_clientList.find(it.key()) == m_clientList.end())
        {
            // 添加新项目到m_listWidget
            ListItem *friendListItem = new ListItem(it.value());
            QListWidgetItem *listWidgetItemm = new QListWidgetItem();
            listWidgetItemm->setSizeHint(QSize(250,65));
            m_listWidget->addItem(listWidgetItemm);
            m_listWidget->setItemWidget(listWidgetItemm,friendListItem);

            // 添加新项目到m_clientList
            ClientInfo tmpClientInfo;
            tmpClientInfo.listWidgetItem = listWidgetItemm;
            tmpClientInfo.listItem = friendListItem;
            m_clientList.insert(it.key(),tmpClientInfo);
        }
    }*/
}

void FriendListScreen::onItemClicked(QListWidgetItem *item)
{
    ChatScreen *screen=new ChatScreen();
    //screen->show();
}




