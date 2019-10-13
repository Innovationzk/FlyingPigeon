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

    // DaemonInstance处理逻辑
    void DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp);
	// 给所有空闲Ins分配任务，调用StartPostFile(int fileNo)，分配成功则更新文件状态为SEND
	void AssignTask();
    // 给特定的Ins分配任务
	void AssignTask(int insId);

    // 暂停文件发送
	void PauseTask(CMessage* const pMsg);
    // 暂停所有文件发送
    void PauseAllTask();
    // 继续文件发送
    void ContinueTask(CMessage* const pMsg);
    // 继续发送所有文件
    void ContinueAllTask();
    // 取消发送文件
    void CancelTask(u32 fileNo);
    // 取消所有文件
    void CancelAllTask();


	// 普通Instanc处理逻辑
	void InstanceEntry(CMessage* const pMsg);
	// 开始发送文件
    void StartSendFile(u32 fileNo);
    // 继续发送文件
    void ContinueSendFile(u32 fileNo);
	// 发送文件内容
	void SendFileContent();
    // 暂停发送
    void PauseFile();
    // 取消发送
    void CancelFile();
	// 重新发送文件
	void ResendFile();
	// 结束任务
	void FinishTask();
    // 重置数据
    void ClearData();
    // 获取正在发送的文件编号
    u32 GetFileNo();

private:
	u32 m_fileNo;                    // 当前正在发送的文件编号
    std::string m_fileName;          // 当前正在发送的文件名
    std::string m_fileNameFullPath;  // 当前正在发送的文件全路径
	std::fstream m_file;             // 当前正在发送的文件
	u32 m_serverInsId;               // 当前配对的服务端Instance编号
    u32 m_progress;                  // 发送进度
    u32 m_size;                      // 文件大小
};