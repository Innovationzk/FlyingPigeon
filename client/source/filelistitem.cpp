#include <QLabel>
#include <QDebug>
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
    m_bIsComplete(false)
{
    // 文件图标
    m_labIcon = new QLabel(this);
    QFileInfo info(fileFullPath);
    QFileIconProvider ip;
    QIcon icon=ip.icon(info);
    m_labIcon->setPixmap(icon.pixmap(25,25));
    m_labIcon->move(10,20);

    // 文件名
    m_labName=new QLabel(this);
    m_labName->setText(QFileInfo(fileFullPath).fileName());
    m_labName->setStyleSheet("QLabel{color: rgb(45, 45, 45);"
                             "font-family:Microsoft YaHei UI;font: 9pt;}");
    m_labName->move(65,10);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->move(65,30);
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QString::fromLocal8Bit("等待中..."));

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

    // 通知底层开始发送文件
    ui_clt_post_file_req msg;
    msg.fileNum = 1;
    strcpy(msg.fileName, fileFullPath.toUtf8().data());
    CLIENTAPP::Post(EV_UI_CLT_POST_FILE_REQ, &msg, sizeof(msg));
}

void FileListItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(200,200,200),1));    // 设置画笔形式
    painter.drawLine(15,height()-1,width()-15,height()-1);  // 画横线
}

void FileListItem::setProgress(int progress)
{
    m_progress=progress;
    m_progressBar->resetFormat();
    m_progressBar->setValue(progress);
    if (progress == 100)  // 发送完成，校验中
    {
        m_progressBar->setMaximum(0);
    }
}

void FileListItem::cancelSendFile()
{
    // TODO
    emit sigRemoveItem(m_fileNo);
}

void FileListItem::pauseFile()
{
    onBtnPauseClicked();
}

void FileListItem::continueFile()
{
    onBtnContinueClicked();
}

void FileListItem::sendFileComplete()
{
    m_bIsComplete = true;
    m_progressBar->setFormat(QString::fromLocal8Bit("已完成"));
    m_btnPause->hide();
    m_btnCancel->hide();
    m_labComplete->show();
    m_progressBar->setProperty("type","complete");
    m_progressBar->setStyleSheet(m_progressBar->styleSheet());
}

bool FileListItem::isComplete()
{
    return m_bIsComplete;
}

void FileListItem::onBtnContinueClicked()
{
    // TODO
    m_btnContinue->hide();
    m_btnPause->show();
    m_progressBar->resetFormat();
    m_progressBar->setValue(m_progress);
}

void FileListItem::onBtnPauseClicked()
{
    qDebug("[%s]: called\n",__FUNCTION__);
    // TODOm_pro
    m_btnContinue->show();
    m_btnPause->hide();
    m_progressBar->setFormat(QString::fromLocal8Bit("暂停中"));
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
        qDebug("[%s]: emit signal sigRemoveItem(%d)\n",__FUNCTION__, m_fileNo);
        emit sigRemoveItem(m_fileNo);
    }
}
