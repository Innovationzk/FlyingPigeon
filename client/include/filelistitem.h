#pragma once

#include <QWidget>
#include <QString>
#include <QDateTime>

class QLabel;
class QToolButton;
class QProgressBar;
class QGridLayout;

class FileListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FileListItem(const QString& fileFullPath, unsigned int fileNo);

    int height() const
    {
        return m_height;
    }

    void paintEvent(QPaintEvent *);
    void setProgress(int progress);
    void cancelSendFile();
    // 暂停
    void pauseFile();
    // 继续
    void continueFile();
    // 文件发送完毕
    void sendFileComplete();
    // 是否发送完成
    bool isComplete();

signals:
    void sigRemoveItem(unsigned int m_fileNo);

public slots:
    void onBtnContinueClicked();
    void onBtnPauseClicked();
    void onBtnCancelClicked();

private:
    QLabel* m_labIcon;
    QLabel* m_labName;
    QLabel* m_labComplete;
    QToolButton* m_btnContinue;
    QToolButton* m_btnPause;
    QToolButton* m_btnCancel;
    QProgressBar* m_progressBar;
    QGridLayout* m_layout;
    int m_progress;   // 暂停时保存进度，继续时回复进度
    int m_height;
    unsigned int m_fileNo;
    bool m_bIsComplete;
};


