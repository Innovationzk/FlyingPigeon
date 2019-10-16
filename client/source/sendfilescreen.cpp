#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <QListWidget>
#include <QStringList>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStringList>
#include <string>
#include "sendfilescreen.h"
#include "filelistitem.h"
#include "uicontroller.h"

unsigned int SendFileScreen::sm_fileNo=1;

SendFileScreen::SendFileScreen(QString userName)
{
    // 图标、名称等信息
    m_leftWidgetContainer=new QWidget(this);
    m_leftWidgetContainer->setFixedWidth(140);

    // 图标
    m_labIcon = new QLabel(m_leftWidgetContainer);
    QPixmap pixmap("resources/servericon.svg");
    m_labIcon->setPixmap(pixmap.scaled(75,75,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    m_labIcon->move((m_leftWidgetContainer->width()-m_labIcon->width())/2+8,40);

    // 名称
    m_labName = new QLabel(m_leftWidgetContainer);
    m_labName->setStyleSheet("QLabel{font-family:Microsoft YaHei UI;font: 11pt;color: rgb(55, 55, 55)}");
    m_labName->setText(userName);
    m_labName->resize(QSize(100,30));
    m_labName->setAlignment(Qt::AlignCenter);
    m_labName->move((m_leftWidgetContainer->width()-m_labIcon->width())/2-5,m_labIcon->height()+120);

    // 右侧控件容器
    QWidget *rightWidgetContainer=new QWidget(this);

    // 文件列表
    m_fileList=new QListWidget(rightWidgetContainer);
    m_fileList->setFrameStyle(QFrame::NoFrame);

    // 按钮容器
    QWidget *btnWidget = new QWidget(this);

    //全部暂停
    m_btnPauseAll=new QPushButton(btnWidget);
    m_btnPauseAll->setText(QString::fromLocal8Bit("全部暂停"));
    m_btnPauseAll->setFixedSize(90,30);
    m_btnPauseAll->setStyleSheet("QPushButton{background-color: white;color: rgb(55, 55, 55);"
                                 "font: 11pt;font-family:Microsoft YaHei UI;"
                                 "border-radius:15px;border:1px solid rgb(200,200,200);border-radius:15px;padding: 2px}");

    // 全部开始
    m_btnContinueAll=new QPushButton(btnWidget);
    m_btnContinueAll->setText(QString::fromLocal8Bit("全部开始"));
    m_btnContinueAll->setFixedSize(100,30);
    m_btnContinueAll->setStyleSheet("QPushButton{background-color: white;color: rgb(55, 55, 55);"
                                 "font: 11pt;font-family:Microsoft YaHei UI;"
                                 "border-radius:15px;border:1px solid rgb(200,200,200);border-radius:15px;padding: 2px}");

    // 发送按钮
    m_btnSendFile=new QPushButton(btnWidget);
    m_btnSendFile->setText(QString::fromLocal8Bit("发送文件"));
    m_btnSendFile->setFixedSize(105,30);
    m_btnSendFile->setStyleSheet("QPushButton{background-color:rgb(0,180,240);color:rgb(255,255,255);"
                                 "font: 11pt;font-family:Microsoft YaHei UI;"
                                 "border-radius:15px;padding: 2px}");

    // 布局
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->addWidget(m_btnPauseAll);
    btnLayout->addWidget(m_btnContinueAll);
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnSendFile);
    btnLayout->setSpacing(20);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_leftWidgetContainer,0,0,2,1);
    layout->addWidget(m_fileList,0,1,1,1);
    layout->addWidget(btnWidget,1,1,1,1);

    // 设置界面样式
    this->setMinimumSize(800,480);
    this->setWindowTitle(userName);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet("SendFileScreen{background-color: rgb(255, 255, 255)}");

    connect(m_btnSendFile,SIGNAL(clicked(bool)),this,SLOT(onBtnSendFileClicked()));
    connect(m_btnPauseAll,SIGNAL(clicked(bool)),this,SLOT(onBtnPauseAllClicked()));
    connect(m_btnContinueAll,SIGNAL(clicked(bool)),this,SLOT(onBtnContinueAllClicked()));
    UIController* uiController = UIController::getInstance();
    qRegisterMetaType<u32>("u32");
    connect(uiController, SIGNAL(sigSendFileResult(u16)), this, SLOT(onNotifySendFileResult(u16)));
    connect(uiController, SIGNAL(sigNotifyFileProgress(u32, u32)), this, SLOT(onNotifyFileProgress(u32, u32)));
    connect(uiController, SIGNAL(sigNotifySendFileFalied(u32)), this, SLOT(onNotifySendFileFalied(u32)));
    connect(uiController, SIGNAL(sigPostFileComplete(u32)), this, SLOT(onNotifyPostFileComplete(u32)));
}

void SendFileScreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(200,200,200),1));    //设置画笔形式
    painter.drawLine(m_leftWidgetContainer->width()+15,15,m_leftWidgetContainer->width()+15,height()-15);    //画竖线
}

void SendFileScreen::closeEvent(QCloseEvent *event)
{
    // 如果发送列表中有未完成的文件
    if (m_mapSendingList.isEmpty())
    {
        this->hide();
        event->ignore();
        return;
    }

    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, QString::fromLocal8Bit("警告"),
                                   QString(QString::fromLocal8Bit("关闭该窗口会取消所有正在发送的文件，确认关闭吗？")),
                                   QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No)  //忽略退出信号，程序继续运行
    {
        event->ignore();
    }
    else if (button == QMessageBox::Yes)  //接受退出信号，程序退出
    {
        for (QMap<unsigned int,FileListItem*>::iterator it = m_mapSendingList.begin();it != m_mapSendingList.end();)
        {
            FileListItem *fileListItem = it.value();
            ++it;    // 防止迭代器失效
            fileListItem->cancelSendFile();
        }
        this->hide();
        event->ignore();
    }
}

void SendFileScreen::addFile(const QString &filePath)
{
    // 在列表中新增项
    FileListItem *fileListItem = new FileListItem(filePath,sm_fileNo);
    QListWidgetItem *listWidgetItem = new QListWidgetItem(m_fileList);
    listWidgetItem->setSizeHint(QSize(m_fileList->width()-20,fileListItem->height()));
    m_fileList->setItemWidget(listWidgetItem,fileListItem);

    // 添加到正在发送列表
    m_mapSendingList[sm_fileNo]=fileListItem;
    m_mapListWidgetItem[sm_fileNo]=listWidgetItem;
    ++sm_fileNo;

    // 开始发送
    fileListItem->startSendFile();

    // 绑定信号
    connect(fileListItem,SIGNAL(sigRemoveItem(unsigned int)),this,SLOT(onNotifyRemoveItem(unsigned int)));
}

void SendFileScreen::restoreExistFileList()
{
    // 读取本地备份列表
    QDir directory("fileList");
    QStringList strFileList = directory.entryList(QStringList(),QDir::Files);
    for (int i = 0;i<strFileList.size();++i)
    {
        qDebug("[%s]: open file %s\n",__FUNCTION__,strFileList[i].toLocal8Bit().data());
        QFile file("fileList/"+strFileList[i]);
        file.open(QIODevice::ReadOnly);
        if (file.isOpen())
        {
            QString fileFullPath = file.readLine();
            fileFullPath.remove(fileFullPath.size()-1,1);
            qDebug()<<fileFullPath;
            FileListItem::FILE_STATUS state = (FileListItem::FILE_STATUS)(file.readLine().toInt());
            qDebug("[%s]: fileFullPath:%s,state:%d\n",__FUNCTION__,fileFullPath.toLocal8Bit().data(),state);
            addExistFile(fileFullPath,strFileList[i].toInt(),state);
            if (state != FileListItem::FILE_STATUS_COMPLETE)
            {
                this->show();
            }
        }
    }
}

void SendFileScreen::addExistFile(const QString &filePath, u32 fileNo, FileListItem::FILE_STATUS fileStatus)
{
    FileListItem *fileListItem = new FileListItem(filePath,fileNo);
    QListWidgetItem *listWidgetItem = new QListWidgetItem(m_fileList);
    listWidgetItem->setSizeHint(QSize(m_fileList->width()-20,fileListItem->height()));
    m_fileList->setItemWidget(listWidgetItem,fileListItem);

    fileListItem->setStatus(fileStatus);
    // 添加到正在发送列表
    if (fileStatus!=FileListItem::FILE_STATUS_COMPLETE)
    {
        m_mapSendingList[fileNo]=fileListItem;
        m_mapListWidgetItem[fileNo]=listWidgetItem;
    }

    // 绑定信号
    connect(fileListItem,SIGNAL(sigRemoveItem(unsigned int)),this,SLOT(onNotifyRemoveItem(unsigned int)));
}

void SendFileScreen::onBtnSendFileClicked()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this,QString::fromLocal8Bit("选择文件"));
    for (QStringList::iterator it = fileList.begin(); it != fileList.end(); ++it)
    {
        if (m_setAllFile.find(*it)!=m_setAllFile.end())
        {
            QMessageBox::information(this,QString::fromLocal8Bit("提示"),*it+QString::fromLocal8Bit("已经在队列中"));
        }
        else
        {
            m_setAllFile.insert(*it);
            addFile(*it);
        }
    }
}

void SendFileScreen::onNotifyRemoveItem(unsigned int fileNo)
{
    qDebug("[%s]: called\n",__FUNCTION__);

    if (m_mapSendingList.find(fileNo)==m_mapSendingList.end())
    {
        qDebug("[%s]: error:cannot find file:%d\n",__FUNCTION__,fileNo);
        return;
    }
    FileListItem *fileListItem = m_mapSendingList[fileNo];
    QListWidgetItem *listWidgetItem = m_mapListWidgetItem[fileNo];

    qDebug("[%s]: disconnect signal and delete fileListItem\n",__FUNCTION__);
    // 解绑信号
    disconnect(fileListItem,SIGNAL(sigRemoveItem(unsigned int)),this,SLOT(onNotifyRemoveItem(unsigned int)));
    // 释放内存
    m_mapSendingList.remove(fileNo);
    m_setAllFile.remove(fileListItem->getFileFullPath());
    delete m_fileList->takeItem(m_fileList->row(listWidgetItem));
    //delete fileListItem;
}

void SendFileScreen::onBtnPauseAllClicked()
{
    for (QMap<unsigned int,FileListItem*>::iterator it = m_mapSendingList.begin();it != m_mapSendingList.end();++it)
    {
        FileListItem *fileListItem = it.value();
        fileListItem->pauseFile();
    }
}

void SendFileScreen::onBtnContinueAllClicked()
{
    for (QMap<unsigned int,FileListItem*>::iterator it = m_mapSendingList.begin();it != m_mapSendingList.end();++it)
    {
        FileListItem *fileListItem = it.value();
        fileListItem->continueFile();
    }
}

void SendFileScreen::onNotifySendFileResult(u16 eventType)
{
    qDebug("[%s]: called\n", __FUNCTION__);

    switch (eventType)
    {
    case EV_CLT_UI_POST_FILE_ACK:
        break;
    case EV_CLT_UI_POST_FILE_NACK:
    {
        qDebug("[%s]: server offine,pause all file\n", __FUNCTION__);
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("服务器离线，请检查网络连接"));
        onBtnPauseAllClicked();
        break;
    }
    default:
        break;
    }
}

void SendFileScreen::onNotifyFileProgress(u32 fileNo, u32 progress)
{
    if (m_mapSendingList.find(fileNo)==m_mapSendingList.end())
    {
        return;
    }
    qDebug("[%s]: called, fileNo:%d, progress:%d\n", __FUNCTION__, fileNo, progress);
    m_mapSendingList[fileNo]->setProgress(progress);
}

void SendFileScreen::onNotifySendFileFalied(u32 fileNo)
{
    qDebug("[%s]: called\n", __FUNCTION__);

    QMessageBox::information(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("服务器资源不足，将暂停等待中的文件"));

    for (QMap<unsigned int,FileListItem*>::iterator it = m_mapSendingList.begin();it != m_mapSendingList.end();++it)
    {
        FileListItem *fileListItem = m_mapSendingList[fileNo];
        if(fileListItem->isWaiting())
        {
            fileListItem->pauseFile();
        }
    }
}

void SendFileScreen::onNotifyPostFileComplete(u32 fileNo)
{
    qDebug("[%s]: called,file %d complete\n", __FUNCTION__, fileNo);

    if (m_mapSendingList.find(fileNo)==m_mapSendingList.end())
    {
        qDebug("[%s]: error:file %d not in file sending task\n", __FUNCTION__, fileNo);
        return;
    }
    FileListItem *fileListItem = m_mapSendingList[fileNo];
    m_mapSendingList.remove(fileNo);
    m_mapListWidgetItem.remove(fileNo);
    fileListItem->sendFileComplete();
}

void SendFileScreen::onNotifyResendFile(u32 fileNo)
{
    qDebug("[%s]: called, fileNo %d\n", __FUNCTION__, fileNo);

    FileListItem *fileListItem = m_mapSendingList[fileNo];
    fileListItem->setProgress(0);
    QMessageBox::information(this,QString::fromLocal8Bit("提示"),fileListItem->getFileName()+QString::fromLocal8Bit("校验失败，需要重新发送"));
}





