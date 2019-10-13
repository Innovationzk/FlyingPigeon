#pragma once
#include <fstream>
#include <osp.h>
#include <kdvtype.h>
#include <commondef.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

class FileAppInstance:public CInstance
{
public:
    FileAppInstance();
    ~FileAppInstance();

    // DaemonInstance�����߼�
    void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
	// �����п���Ins�������񣬵���StartPostFile(int fileNo)������ɹ�������ļ�״̬ΪSEND
	void AssignTask();
    // ���ض���Ins��������
	void AssignTask(int insId);

    // ��ͣ�ļ�����
	void PauseTask(CMessage* const pMsg);
    // ��ͣ�����ļ�����
    void PauseAllTask();
    // �����ļ�����
    void ContinueTask(CMessage* const pMsg);
    // �������������ļ�
    void ContinueAllTask();
    // ȡ�������ļ�
    void CancelTask(u32 fileNo);
    // ȡ�������ļ�
    void CancelAllTask();


	// ��ͨInstanc�����߼�
	void InstanceEntry(CMessage* const pMsg);
	// ��ʼ�����ļ�
    void StartSendFile(u32 fileNo);
    // ���������ļ�
    void ContinueSendFile(u32 fileNo);
	// �����ļ�����
	void SendFileContent();
    // ��ͣ����
    void PauseFile();
    // ȡ������
    void CancelFile();
	// ���·����ļ�
	void ResendFile();
	// ��������
	void FinishTask();
    // ��������
    void ClearData();
    // ��ȡ���ڷ��͵��ļ����
    u32 GetFileNo();

private:
	u32 m_fileNo;                    // ��ǰ���ڷ��͵��ļ����
    std::string m_fileName;          // ��ǰ���ڷ��͵��ļ���
    std::string m_fileNameFullPath;  // ��ǰ���ڷ��͵��ļ�ȫ·��
	std::fstream m_file;             // ��ǰ���ڷ��͵��ļ�
	u32 m_serverInsId;               // ��ǰ��Եķ����Instance���
    u32 m_progress;                  // ���ͽ���
    u32 m_size;                      // �ļ���С
};