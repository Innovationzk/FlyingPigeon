#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "../../common/osp.h"
#include "../../common/commondef.h"

class FileAppInstance :public CInstance
{
public:
	FileAppInstance();
	~FileAppInstance();

	// DaemonInstance�����߼�
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
	// ���Ը�Instance�����ļ���������
	void AssignTask(CMessage* const pMsg);
    // ���������ļ�
	void ContinueTask(CMessage* const pMsg);
    // ȡ����ͣ�е��ļ�
	void CancelSuspendedFile(CMessage* const pMsg);
    // �ж�Ŀ��ͻ��˵������ļ���������
    void StopClientTask(CMessage* const pMsg);

	// ��ͨInstanc�����߼�
	void InstanceEntry(CMessage* const pMsg);
	// ��ʼ�����ļ�
	bool StartReceiveFile(CMessage* const pMsg);
	// �����ļ�����
	void ReceiveFileContent(CMessage* const pMsg);
	// ��֤MD5��
	void VerifyMD5Code(CMessage* const pMsg);
    // ��ͣ�ļ�
    void PauseFile(CMessage* const pMsg);
    // ���������ļ�
	bool ContinueReceiveFile(CMessage* const pMsg);
    // ȡ�������е��ļ�
    void CancelSendingFile(CMessage* const pMsg);
    // �������
    void ClearData();

private:
    u32 m_clientNo;                 // ��ǰ��ԵĿͻ��˱��
    u32 m_clientNode;               // ��ǰ��ԵĿͻ��˽ڵ��
	std::fstream m_file;            // Ҫ���յ��ļ�
	std::string m_fileName;         // �ļ���
    std::string m_fileNameFullPath; // �ļ�ȫ·��
	u32 m_fileProgress;             // ���ս���
};