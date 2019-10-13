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

	// DaemonInstance处理逻辑
	void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
	// 尝试给Instance分配文件接收任务
	void AssignTask(CMessage* const pMsg);
    // 继续接受文件
	void ContinueTask(CMessage* const pMsg);
    // 取消暂停中的文件
	void CancelSuspendedFile(CMessage* const pMsg);
    // 中断目标客户端的所有文件接收任务
    void StopClientTask(CMessage* const pMsg);

	// 普通Instanc处理逻辑
	void InstanceEntry(CMessage* const pMsg);
	// 开始接收文件
	bool StartReceiveFile(CMessage* const pMsg);
	// 接收文件数据
	void ReceiveFileContent(CMessage* const pMsg);
	// 验证MD5码
	void VerifyMD5Code(CMessage* const pMsg);
    // 暂停文件
    void PauseFile(CMessage* const pMsg);
    // 继续接收文件
	bool ContinueReceiveFile(CMessage* const pMsg);
    // 取消发送中的文件
    void CancelSendingFile(CMessage* const pMsg);
    // 清除数据
    void ClearData();

private:
    u32 m_clientNo;                 // 当前配对的客户端编号
    u32 m_clientNode;               // 当前配对的客户端节点号
	std::fstream m_file;            // 要接收的文件
	std::string m_fileName;         // 文件名
    std::string m_fileNameFullPath; // 文件全路径
	u32 m_fileProgress;             // 接收进度
};