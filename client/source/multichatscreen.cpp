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
#include "multichatscreen.h"
#include "multichatfriendlistitem.h"
#include "friendlistitem.h"
#include "msglistitem.h"
#include "commondef.h"
#include "clientapp/clientapp.h"

#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 26

MultiChatScreen::MultiChatScreen(const QVector<QString> &vecFriendName, const QVector<unsigned int> &clientNo):
m_vecClientNo(clientNo)
{
    if (vecFriendName.size()<2)
    {
        qDebug()<<"vecFriendName.size()="<<vecFriendName.size();
        return;
    }

    // �����б�
    m_friendListWidget=new QListWidget(this);
    m_friendListWidget->setFixedWidth(160);
    m_friendListWidget->setFrameStyle(QFrame::NoFrame);

    for (int i=0;i<vecFriendName.size();i++)
    {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(m_friendListWidget);
        MultiChatFriendListItem *friendListItem = new MultiChatFriendListItem(vecFriendName[i]);
        listWidgetItem->setSizeHint(QSize(m_friendListWidget->width(),friendListItem->height()));
        m_friendListWidget->setItemWidget(listWidgetItem, friendListItem);
    }

    // �Ҳ�ؼ�����
    QWidget *rightWidgetContainer=new QWidget(this);

    // �������
    m_chatListWidget=new QListWidget(rightWidgetContainer);
    m_chatListWidget->setFrameStyle(QFrame::NoFrame);

    //�������
    m_inputWidget=new QPlainTextEdit(rightWidgetContainer);
    m_inputWidget->setFixedHeight(100);
    m_inputWidget->setFrameStyle(QFrame::NoFrame);

    // ���ͺ�ȡ����ť
    QWidget *btnWidget = new QWidget(this);
    btnWidget->setFixedHeight(35);

    m_btnSend = new QPushButton(btnWidget);
    m_btnSend->setText(QString::fromLocal8Bit("����"));
    m_btnSend->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    m_btnSend->setStyleSheet("QPushButton{background-color: rgb(0,180,240); color: white;border:2px; padding: 2px}");

    m_btnCancel = new QPushButton(btnWidget);
    m_btnCancel->setText(QString::fromLocal8Bit("ȡ��"));
    m_btnCancel->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    m_btnCancel->setStyleSheet("QPushButton{background-color: white;border:1px solid rgb(200,200,200)}");

    // ���ò���
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
    hBoxLayout->addWidget(m_friendListWidget);
    hBoxLayout->addWidget(rightWidgetContainer);

    // ���ý�����ʽ
    this->resize(800,480);
    this->setWindowTitle(vecFriendName[0]+","+vecFriendName[1]+"...");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("MultiChatScreen{background-color: rgb(255, 255, 255)}"
        "QPlainTextEdit{ font: 18px; }");

    // ���ź�
    connect(m_btnCancel,SIGNAL(clicked(bool)),m_inputWidget,SLOT(clear()));
    connect(m_btnSend,SIGNAL(clicked(bool)),this,SLOT(onBtnSendClicked()));
}

void MultiChatScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(200,200,200),1));    //���û�����ʽ
    //painter.setPen(QPen(QColor(0,0,0),1));    //���û�����ʽ
    painter.drawLine(185,height()-155,width()-15,height()-155);  //������
    painter.drawLine(175,15,175,height()-15);    //������
    update();
}

void MultiChatScreen::onBtnSendClicked()
{
    QString text = m_inputWidget->toPlainText();
    m_inputWidget->clear();

    if (text.isEmpty())
    {
        return;
    }

    if (m_vecClientNo.size()>6)
    {
        qDebug("[%s]: error: targets number over 16\n", __FUNCTION__);
        return;
    }

    ui_clt_post_msg_req msg;
    msg.clientNum=m_vecClientNo.size();
    for (int i=0;i<m_vecClientNo.size();++i)
    {
        msg.clientNo[i]=m_vecClientNo[i];
    }
    strcpy(msg.msgContent,text.toLocal8Bit().data());
    CLIENTAPP::Post(EV_UI_CLT_POST_MSG_REQ,&msg,sizeof(msg));

    QListWidgetItem *listWidgetItem= new QListWidgetItem(m_chatListWidget);
    MsgListItem *msgListItem = new MsgListItem(text,false);
    listWidgetItem->setSizeHint(QSize(m_chatListWidget->width(),msgListItem->height()));
    m_chatListWidget->setItemWidget(listWidgetItem,msgListItem);

}
