#ifndef CLIENT_H
#define CLIENT_H

#include <QtWidgets/QWidget>
#include <string>

class QPushButton;
class QLabel;
class QLineEdit;
class QProgressBar;
class QGridLayout;
class QVBoxLayout;
class QGroupBox;

class client;

class client : public QWidget
{
    Q_OBJECT

public:
    client(QWidget *parent = 0);
    ~client();
    static void callBackUpdateProgressBar(int progress);
    static client *pClient;

signals:
    void sigUpdateProgressBar(int progress);

public slots:
    void onBtnChooseClicked();
    void onBtnSendClicked();
    void onTimerTimeout();

private:
    QLabel* m_labelPleaseChoose;
    QLineEdit* m_fllePath;
    QPushButton* m_btnChoose;
    QLabel* m_labelProgress;
    QProgressBar* m_progressBar;
    QPushButton* m_btnSend;
    QGridLayout* m_gridLayout;
    QVBoxLayout* m_verticalLayout;
    QGroupBox* m_groupBox;

    std::string filePath;
};

#endif // CLIENT_H
