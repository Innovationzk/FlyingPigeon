#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QFileInfo>
#include <QToolButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProgressBar>
#include <QFileIconProvider>
#include "filelistitem.h"
#include "commondef.h"
#include "clientapp/clientapp.h"

#define HEIGHT 65

FileListItem::FileListItem(const QString& fileFullPath, unsigned int fileNo):
    m_fileNo(fileNo),
    m_fileFullPath(fileFullPath),
    m_bIsComplete(false)
{
    qDebug("[%s]: fileFullPath:%s\n",__FUNCTION__,fileFullPath.toLocal8Bit().data());
    // 文件图标
    m_labIcon = new QLabel(this);
    QFileInfo info(fileFullPath);
    QFileIconProvider ip;
    QIcon icon=ip.icon(info);
    if (icon.isNull())
    {
        qDebug()<<"icon.isNull()";
    }
    m_labIcon->setPixmap(icon.pixmap(25,25));
    m_labIcon->move(10,10);

    // 文件名
    m_labName=new QLabel(this);
    m_fileName=QFileInfo(fileFullPath).fileName();
    m_labName->setText(m_fileName);
    m_labName->setStyleSheet("QLabel{color: rgb(45, 45, 45);"
                             "font-family:Microsoft YaHei UI;font: 9pt;}");
    m_labName->move(65,10);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->move(65,30);
    m_progressBar->setValue(0);

    // 按钮
    m_btnContinue = new QToolButton(this);
    m_btnContinue->setIcon(QIcon("resources/continue.png"));
    m_btnContinue->setFixedSize(HEIGHT-10,HEIGHT-10);
    m_btnContinue->setIconSize(QSize(HEIGHT-15,HEIGHT-15));
    m_btnContinue->hide();

    m_btnPause = new QToolButton(this);
    m_btnPause->setIcon(QIcon("resources/pause.png"));
    m_btnPause->setFixedSize(HEIGHT-10,HEIGHT-10);
    m_btnPause->setIconSize(QSize(HEIGHT-35,HEIGHT-35));

    m_btnCancel = new QToolButton(this);
    m_btnCancel->setIcon(QIcon("resources/cancel.png"));
    m_btnCancel->setFixedSize(HEIGHT-10,HEIGHT-10);
    m_btnCancel->setIconSize(QSize(HEIGHT-35,HEIGHT-35));

    m_labComplete = new QLabel(this);
    m_labComplete->setPixmap(QPixmap("resources/complete.png").scaled(HEIGHT-20,HEIGHT-20));
    m_labComplete->setFixedSize(HEIGHT-10,HEIGHT-10);
    m_labComplete->hide();

    // 布局
    m_layout=new QGridLayout(this);
    m_layout->setHorizontalSpacing(15);
    m_layout->addWidget(m_labIcon,0,0,2,1);
    m_layout->addWidget(m_labName,0,1,1,1);
    m_layout->addWidget(m_progressBar,1,1,1,1);
    m_layout->addWidget(m_btnContinue,0,2,2,1);
    m_layout->addWidget(m_btnPause,0,2,2,1);
    m_layout->addWidget(m_btnCancel,0,3,2,1);
    m_layout->addWidget(m_labComplete,0,3,2,1);

    // 界面样式
    this->m_height=HEIGHT;
    this->setStyleSheet("QProgressBar {border: 1px solid rgb(59,153,252);border-radius:8px;color:#373737;text-align:center;width:300px;}"
                        "QProgressBar::chunk {background-color: rgb(60,155,255);border-radius: 8px;}"
                        "QProgressBar[type=success] {border: 1px solid #4cae4c;text-align:center;}"
                        "QProgressBar[type=success]::chunk {background-color: red;}"
                        "QProgressBar[type=complete] {border: 1px solid #c0c0c0;}"
                        "QProgressBar[type=complete]::chunk {background-color: #c0c0c0;}"
                        "QToolButton{border: 1px solid transparent;}");

    // 绑定信号
    connect(m_btnContinue,SIGNAL(clicked(bool)),this,SLOT(onBtnContinueClicked()));
    connect(m_btnPause,SIGNAL(clicked(bool)),this,SLOT(onBtnPauseClicked()));
    connect(m_btnCancel,SIGNAL(clicked(bool)),this,SLOT(onBtnCancelClicked()));

    // 保存备份文件
    m_backUpFile.setFileName(QString("fileList/")+QString::number(m_fileNo));
    m_backUpFile.open(QIODevice::WriteOnly);
    if (m_backUpFile.isOpen())
    {
        m_backUpFile.write((m_fileFullPath+"\n"+QString::number(FILE_STATUS_WAIT)).toUtf8());
        m_backUpFile.close();
    }
}

FileListItem::~FileListItem()
{
    qDebug()<<"in destrustor";
}

void FileListItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(200,200,200),1));    // 设置画笔形式
    painter.drawLine(15,height()-1,width()-15,height()-1);  // 画横线
}

void FileListItem::setProgress(int progress)
{
    qDebug("[%s]: called\n", __FUNCTION__);
    m_progress=progress;
    m_progressBar->setValue(progress);
    if (progress == 100)  // 发送完成，校验中
    {
        m_progressBar->setMaximum(0);
    }
}

void FileListItem::setStatus(FileListItem::FILE_STATUS state)
{
    if (state==FILE_STATUS_WAIT)
    {
        m_bIsComplete = false;
        m_btnContinue->hide();
        m_btnPause->show();
    }
    else if (state==FILE_STATUS_PAUSE)
    {
        m_bIsComplete = false;
        //m_progressBar->setFormat(QString::fromLocal8Bit("%p% 暂停中"));
        m_btnContinue->show();
        m_btnPause->hide();
    }
    else if (state==FILE_STATUS_COMPLETE)
    {
        m_bIsComplete = true;
        m_progressBar->setMaximum(100);
        m_progressBar->setValue(100);
        m_progressBar->setFormat(QString::fromLocal8Bit("已完成"));
        m_btnContinue->hide();
        m_btnPause->hide();
        m_btnCancel->hide();
        m_labComplete->show();
        m_progressBar->setProperty("type","complete");
        m_progressBar->setStyleSheet(m_progressBar->styleSheet());
    }
}

void FileListItem::cancelSendFile()
{
    qDebug("[%s]: called\n",__FUNCTION__);
    ui_clt_cancel_file_cmd msg;
    msg.fileNo = m_fileNo;
    CLIENTAPP::Post(EV_UI_CLT_CANCEL_FILE_CMD,&msg, sizeof(msg));
    // 删除备份文件
    m_backUpFile.remove();
    emit sigRemoveItem(m_fileNo);
}

void FileListItem::pauseFile()
{
    onBtnPauseClicked();
}

void FileListItem::continueFile()
{
    onBtnContinueClicked();
    // 保存备份文件
    m_backUpFile.open(QIODevice::WriteOnly);
    if (m_backUpFile.isOpen())
    {
        m_backUpFile.write((m_fileFullPath+"\n"+QString::number(FILE_STATUS_WAIT)).toUtf8());
        m_backUpFile.close();
    }
}

void FileListItem::sendFileComplete()
{
    m_bIsComplete = true;
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(100);
    m_progressBar->setFormat(QString::fromLocal8Bit("已完成"));
    m_btnPause->hide();
    m_btnCancel->hide();
    m_labComplete->show();
    m_progressBar->setProperty("type","complete");
    m_progressBar->setStyleSheet(m_progressBar->styleSheet());

    // 保存备份文件
    m_backUpFile.open(QIODevice::WriteOnly);
    if (m_backUpFile.isOpen())
    {
        m_backUpFile.write((m_fileFullPath+"\n"+QString::number(FILE_STATUS_COMPLETE)).toUtf8());
        m_backUpFile.close();
    }
}

bool FileListItem::isComplete()
{
    return m_bIsComplete;
}

bool FileListItem::isWaiting()
{
    if (m_progressBar->format() == QString::fromLocal8Bit("%p% 等待发送..."))
    {
        return true;
    }
    return false;
}

void FileListItem::startSendFile()
{
    // 通知底层开始发送文件
    ui_clt_post_file_req msg;
    msg.fileNum = 1;
    strcpy(msg.fileName, m_fileFullPath.toLocal8Bit().data());
    CLIENTAPP::Post(EV_UI_CLT_POST_FILE_REQ, &msg, sizeof(msg));
}

void FileListItem::onBtnContinueClicked()
{
    qDebug("[%s]: called\n",__FUNCTION__);

    m_btnContinue->hide();
    m_btnPause->show();
    //m_progressBar->setFormat(QString::fromLocal8Bit("%p% 等待发送..."));
    m_progressBar->setValue(m_progress);
    update();

    ui_clt_continue_file_cmd msg;
    msg.fileNo = m_fileNo;
    CLIENTAPP::Post(EV_UI_CLT_CONTINUE_FILE_CMD, &msg, sizeof(msg));

    // 保存备份文件
    m_backUpFile.open(QIODevice::WriteOnly);
    if (m_backUpFile.isOpen())
    {
        m_backUpFile.write((m_fileFullPath+"\n"+QString::number(FILE_STATUS_WAIT)).toUtf8());
        m_backUpFile.close();
    }
}

void FileListItem::onBtnPauseClicked()
{
    qDebug("[%s]: called\n",__FUNCTION__);

    m_btnContinue->show();
    m_btnPause->hide();

    ui_clt_pause_file_cmd msg;
    msg.fileNo = m_fileNo;
    CLIENTAPP::Post(EV_UI_CLT_PAUSE_FILE_CMD, &msg, sizeof(msg));

    // 保存备份文件
    m_backUpFile.open(QIODevice::WriteOnly);
    if (m_backUpFile.isOpen())
    {
        m_backUpFile.write((m_fileFullPath+"\n"+QString::number(FILE_STATUS_PAUSE)).toUtf8());
        m_backUpFile.close();
    }
}

void FileListItem::onBtnCancelClicked()
{
    qDebug("[%s]: called\n",__FUNCTION__);
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, QString::fromLocal8Bit("警告"),
        QString(QString::fromLocal8Bit("确认取消发送吗？")),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {
        // do nothing
    }
    else if (button == QMessageBox::Yes) {
        cancelSendFile();
    }
    // 保存备份文件
    m_backUpFile.remove();
}
