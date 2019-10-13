#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QString>
#include <QFileDialog>
#include <QGroupBox>
#include <QTimer>
#include <string>
#include "../include/client.h"
#include "../../common/osp.h"
#include "../../common/commondef.h"
#include "../include/demoinstanceclient.h"
#include "../include/demoappdata.h"

#pragma execution_character_set("utf-8")

#define OspPrintf(a, b, ...) printf(__VA_ARGS__)

typedef zTemplate<DemoInstanceClient, MAX_FILE_INSTANCE_NUMBER_CLIENT, DemoAppData, MAX_ALIAS_LEN_CLIENT> DemoAppClient;
extern DemoAppClient demoAppClient;

extern s32 serverNode;

client* client::pClient = NULL;

client::client(QWidget *parent)
    : QWidget(parent)
{
    pClient=this;

    m_labelPleaseChoose= new QLabel(this);
    m_fllePath = new QLineEdit(this);
    m_btnChoose = new QPushButton(this);
    m_labelProgress= new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_btnSend = new QPushButton(this);
    m_gridLayout = new QGridLayout();
    m_verticalLayout = new QVBoxLayout(this);
    m_groupBox = new QGroupBox(this);

    m_labelPleaseChoose->resize(QSize(35,15));
    m_labelPleaseChoose->setText(QString("选择文件："));

    m_groupBox->setMinimumSize(QSize(400,250));

    m_btnChoose->setText(QString("浏览"));

    m_labelProgress->resize(QSize(35,15));
    m_labelProgress->setText(QString("传输进度："));

    m_progressBar->setTextVisible(false);

    m_btnSend->setText(QString("发送"));

    m_gridLayout->addWidget(m_labelPleaseChoose,0,0);
    m_gridLayout->addWidget(m_fllePath,0,1);
    m_gridLayout->addWidget(m_btnChoose,0,2);
    m_gridLayout->addWidget(m_labelProgress,1,0);
    m_gridLayout->addWidget(m_progressBar,1,1);
    m_gridLayout->addWidget(m_btnSend,1,2);

    m_gridLayout->setHorizontalSpacing(10);
    m_gridLayout->setVerticalSpacing(5);

    m_verticalLayout->addItem(m_gridLayout);
    m_verticalLayout->addWidget(m_groupBox);

    connect(m_btnChoose,SIGNAL(clicked(bool)),this,SLOT(onBtnChooseClicked()));
    connect(m_btnSend,SIGNAL(clicked(bool)),this,SLOT(onBtnSendClicked()));
    connect(this,SIGNAL(sigUpdateProgressBar(int)),m_progressBar,SLOT(setValue(int)));

    setWindowTitle(QString("客户端"));
}

client::~client()
{

}

void client::onBtnChooseClicked()
{
    QString strPath = QFileDialog::getOpenFileName(this,QString("选择要传输的文件"));
    m_fllePath->setText(strPath);
    m_groupBox->setTitle(strPath);

    filePath = strPath.toStdString();
}

void client::onBtnSendClicked()
{
    OspPrintf(TRUE, FALSE, "[%s]: Notify client to send file,file path is:%s\n", __FUNCTION__, filePath.c_str());
    demoAppClient.setCallBackSendProgress(callBackUpdateProgressBar);

    printf("onBtnSendClicked\n");
    char buf[FILE_BUF_SIZE];
    int i=0;
    while(++i<100000){
        if (0 == OspPost(MAKEIID(APP_ID_SERVER, 1), EV_CLT_SER_POST_MSG_REQ, buf, FILE_BUF_SIZE, serverNode)) printf("success\n");
        else printf("failed\n");
    }
}

void client::onTimerTimeout()
{
    OspPost(MAKEIID(APP_ID_CLIENT, CInstance::DAEMON), EV_CLT_CLT_LOG_CMD, "-->>on UI timeout<<--\n\0", 23, 0);
}

void client::callBackUpdateProgressBar(int progress)
{
    emit pClient->sigUpdateProgressBar(progress);
}
