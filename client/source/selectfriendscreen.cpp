#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include "selectfriendscreen.h"
#include "multichatfriendlistitem.h"
#include "multichatscreen.h"
#include "friendlistscreen.h"

SelectFriendScreen::SelectFriendScreen(const QVector<QString>& friendName)
{
    m_friendListWidget = new QListWidget(this);
    m_friendListWidget->setFrameStyle(QFrame::NoFrame);
    m_friendListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i= 0;i<friendName.size();++i)
    {
        MultiChatFriendListItem *friendItem = new MultiChatFriendListItem(friendName[i]);
        QListWidgetItem *listWidgetItem = new QListWidgetItem(m_friendListWidget);
        listWidgetItem->setSizeHint(QSize(m_friendListWidget->width()-20,friendItem->height()));
        m_friendListWidget->setItemWidget(listWidgetItem,friendItem);
    }

    QWidget *btnWidget = new QWidget(this);
    btnWidget->setFixedHeight(30);

    m_btnCancel=new QPushButton(btnWidget);
    m_btnCancel->setText(QString::fromLocal8Bit("ȡ��"));
    m_btnCancel->resize(90,30);
    m_btnCancel->move(15,0);
    m_btnCancel->setStyleSheet(QString::fromLocal8Bit("QPushButton{font: 12pt;font-family:\"΢���ź�\";"
        "background-color: white;border:1px solid rgb(200,200,200);border-radius:15px;}"));

    m_btnConfirm=new QPushButton(btnWidget);
    m_btnConfirm->setText(QString::fromLocal8Bit("ȷ��"));
    m_btnConfirm->resize(90,30);
    m_btnConfirm->move(120,0);
    m_btnConfirm->setStyleSheet(QString::fromLocal8Bit("QPushButton{font: 12pt;font-family:\"΢���ź�\";"
        "background-color: rgb(0,180,240); color: white;border:2px;border-radius:15px}"));

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(m_friendListWidget);
    vBoxLayout->addWidget(btnWidget);

    this->setFixedWidth(250);
    this->setWindowTitle(QString::fromLocal8Bit("ѡ�����"));
    this->setStyleSheet("SelectFriendScreen{background-color: rgb(255, 255, 255)}");
    this->setAttribute(Qt::WA_DeleteOnClose);  //���ô����Զ��ͷ�

    connect(m_btnConfirm,SIGNAL(clicked(bool)),this,SLOT(onBtnConfirmClicked()));
    connect(m_btnCancel,SIGNAL(clicked(bool)),this,SLOT(onBtnCancelClicked()));
}

SelectFriendScreen::~SelectFriendScreen()
{

}

void SelectFriendScreen::closeEvent(QCloseEvent *)
{
    emit sigCloseScreen();
}

void SelectFriendScreen::onBtnConfirmClicked()
{
    QVector<QString> vecFriendName;
    QVector<unsigned int> vecClientNo;
    FriendListScreen *friendListScreen = FriendListScreen::getInstance();
    if (NULL == friendListScreen)
    {
        printf("[%s]: friendListScreen is NULL");
        return;
    }
    for (int i=0;i<m_friendListWidget->count();i++)
    {
        QListWidgetItem *listWidgetItem = m_friendListWidget->item(i);
        if(listWidgetItem->isSelected())
        {
            MultiChatFriendListItem *friendItem = (MultiChatFriendListItem *)(m_friendListWidget->itemWidget(listWidgetItem));
            vecFriendName.push_back(friendItem->getFriendName());
            vecClientNo.push_back(friendListScreen->getClientNo(friendItem->getFriendName()));
        }
    }
    if (vecFriendName.isEmpty())
    {
        QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),QString::fromLocal8Bit("δѡ���κκ���"));
    }
    else if (vecFriendName.size()==1)
    {
        emit sigOpenSingleChatScreen(vecFriendName[0],vecClientNo[0]);
        this->close();
    }
    else if (vecFriendName.size()>16)
    {
        QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),QString::fromLocal8Bit("����16�����ѷ�����Ϣ"));
    }
    else
    {
        qDebug()<<vecFriendName.size();
        MultiChatScreen *multiChatScreen = new MultiChatScreen(vecFriendName,vecClientNo);
        multiChatScreen->show();
        this->close();
    }
}

void SelectFriendScreen::onBtnCancelClicked()
{
    this->close();
}
