#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPainter>
#include "uicontroller.h"
#include "singlechatscreen.h"
#include "friendlistitem.h"
#include "msglistitem.h"

#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 26

SingleChatScreen::SingleChatScreen(QString userName, unsigned int clientNo):
m_userName(userName),
    m_clientNo(clientNo)
{
    // 好友信息
    m_friendWidget=new QWidget(this);
    m_friendWidget->setFixedWidth(140);

    // 好友头像
    m_labFriendIcon = new QLabel(m_friendWidget);
    QPixmap pixmap("resources/friendicon.png");
    m_labFriendIcon->setPixmap(pixmap.scaled(75,75,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labFriendIcon->move((m_friendWidget->width()-m_labFriendIcon->width())/2+8,40);

    // 好友用户名
    m_labFriendName = new QLabel(m_friendWidget);
    m_labFriendName->setText(userName);
    m_labFriendName->resize(QSize(100,30));
    m_labFriendName->setAlignment(Qt::AlignHCenter);
    m_labFriendName->setStyleSheet("QLabel{font-family:Microsoft YaHei UI;font: 11pt;color: rgb(55, 55, 55)}");
    m_labFriendName->move((m_friendWidget->width()-m_labFriendIcon->width())/2-5,m_labFriendIcon->height()+120);

    // 右侧控件容器
    QWidget *rightWidgetContainer=new QWidget(this);

    // 聊天界面
    m_chatListWidget=new QListWidget(rightWidgetContainer);
    m_chatListWidget->setFrameStyle(QFrame::NoFrame);

    //输入界面
    m_inputWidget=new QPlainTextEdit(rightWidgetContainer);
    m_inputWidget->setFixedHeight(100);
    m_inputWidget->setFrameStyle(QFrame::NoFrame);

    // 发送和取消按钮
    QWidget *btnWidget = new QWidget(this);
    btnWidget->setFixedHeight(35);

    m_btnSend = new QPushButton(btnWidget);
    m_btnSend->setText(QString::fromLocal8Bit("发送"));
    m_btnSend->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    m_btnSend->setStyleSheet("QPushButton{background-color: rgb(0,180,240);color: white;"
                             "font: 11pt;font-family:Microsoft YaHei UI;"
                             "border:1px solid transparent;padding: 0px}");

    m_btnCancel = new QPushButton(btnWidget);
    m_btnCancel->setText(QString::fromLocal8Bit("取消"));
    m_btnCancel->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    m_btnCancel->setStyleSheet("QPushButton{background-color: white;color: rgb(55, 55, 55);"
                               "font: 11pt;font-family:Microsoft YaHei UI;"
                               "border:1px solid rgb(200,200,200);border-radius:15px;padding: 2px}");

    // 设置布局
    QHBoxLayout *btnWidgetHLayout = new QHBoxLayout(btnWidget);
    btnWidgetHLayout->addStretch();
    btnWidgetHLayout->addWidget(m_btnCancel);
    btnWidgetHLayout->addWidget(m_btnSend);
    btnWidgetHLayout->setSpacing(25);

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setMargin(2);
    vBoxLayout->addWidget(m_chatListWidget);
    vBoxLayout->addWidget(m_inputWidget);
    vBoxLayout->addWidget(btnWidget);
    rightWidgetContainer->setLayout(vBoxLayout);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->addWidget(m_friendWidget);
    hBoxLayout->addWidget(rightWidgetContainer);
    hBoxLayout->setSpacing(15);

    // 设置界面样式
    //this->resize(800,480);
    this->setMinimumSize(800,480);
    this->setWindowTitle(userName);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("SingleChatScreen{background-color: rgb(255, 255, 255)}"
                        "QPlainTextEdit{ font: 18px }");

    // 绑定信号
    connect(m_btnCancel,SIGNAL(clicked(bool)),m_inputWidget,SLOT(clear()));
    connect(m_btnSend,SIGNAL(clicked(bool)),this,SLOT(onBtnSendClicked()));
}

void SingleChatScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(200,200,200),1));    //设置画笔形式
    painter.drawLine(m_friendWidget->width()+35,height()-155,width()-15,height()-155);  //画横线
    painter.drawLine(m_friendWidget->width()+15,15,m_friendWidget->width()+15,height()-15);    //画竖线
    update();
}

void SingleChatScreen::closeEvent(QCloseEvent *event)
{
    emit sigSingleChatScreenClose(m_clientNo);
}

void SingleChatScreen::onBtnSendClicked()
{
    QString text = m_inputWidget->toPlainText();
    m_inputWidget->clear();
    if (!text.isEmpty())
    {
        QListWidgetItem *listWidgetItem= new QListWidgetItem(m_chatListWidget);
        MsgListItem *msgListItem = new MsgListItem(UIController::getUserName(),text,false);
        listWidgetItem->setSizeHint(QSize(m_chatListWidget->width()-10,msgListItem->height()));
        m_chatListWidget->setItemWidget(listWidgetItem,msgListItem);
    }
}
