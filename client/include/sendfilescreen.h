#pragma once
#include <QMap>
#include <QSet>
#include <QWidget>
#include <QString>
#include "commondef.h"
#include "json/json.h"
#include "filelistitem.h"

class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class FriendListItem;

class SendFileScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SendFileScreen(QString userName="My Server");

    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void addFile(const QString &filePath);
    void restoreExistFileList();
    void addExistFile(const QString &filePath,u32 fileNo,FileListItem::FILE_STATUS fileStatus);

    static unsigned int sm_fileNo;

signals:
    //void sigCloseEvent();

public slots:
    // 添加新文件
    void onBtnSendFileClicked();
    // 移除发送项
    void onNotifyRemoveItem(unsigned int fileNo);
    // 全部暂停
    void onBtnPauseAllClicked();
    // 全部继续
    void onBtnContinueAllClicked();
    // 底层回复是否确认发送
    void onNotifySendFileResult(u16 eventType);
    // 进度更新
    void onNotifyFileProgress(u32 fileNo, u32 progress);
    // 服务器资源不足
    void onNotifySendFileFalied(u32 fileNo);
    // 文件发送完毕
    void onNotifyPostFileComplete(u32 fileNo);
    // 文件校验失败
    void onNotifyResendFile(u32 fileNo);

private:
    QLabel *m_labIcon;             // 图标
    QLabel *m_labName;             // 名称
    QWidget *m_leftWidgetContainer;// 图标、名称的容器
    QPushButton *m_btnPauseAll;    // 全部暂停
    QPushButton *m_btnContinueAll; // 全部继续
    QPushButton *m_btnSendFile;    // 文件发送按钮
    QListWidget *m_fileList;       // 文件列表
    QSet<QString> m_setAllFile;    // 所有文件
    QMap<unsigned int,FileListItem*> m_mapSendingList;         // 正在发送中的文件
    QMap<unsigned int,QListWidgetItem*> m_mapListWidgetItem;   // QListWidgetItem索引
};


