#pragma once
#include <map>
#include <vector>
#include "../../common/osp.h"
#include "../../common/kdvtype.h"
#include "../../common/commondef.h"
#include <json/json.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

class FileAppData
{
public:
    // 定义文件状态
    enum FILE_STATUS {
        FILE_STATUS_ERROR,     // 异常状态
        FILE_STATUS_WAIT,      // 等待传输
        FILE_STATUS_SEND,      // 传输中
        FILE_STATUS_PAUSE,     // 暂停状态
    };
    // 定义文件信息
    struct FileInfo {
        std::string name;
        FILE_STATUS status;
        u32 progress;
        u32 size;
    };

    static u32 fileNo; // 文件唯一标识，从1开始累加

    FileAppData();
    ~FileAppData();

    // 获取文件大小，最大支持4G文件
    u32 GetFileSize(const char* fileFullPath);
    // 添加文件发送任务
    void AddFileSendingTasks(ui_clt_post_file_req *tasks);
    // 获取下一个等待发送的任务，成功返回文件信息，失败时返回的文件名为空字符串
    u32 GetNextFileTask();
    // 获取文件详情
    FileInfo GetFileInfo(u32 fileNo);
    // 获取文件名
    std::string GetFileName(u32 fileNo);
    // 获取文件状态
    FILE_STATUS GetFileStatus(u32 fileNo);
    // 获取队列中所有文件
    std::vector<u32> GetAllFileNo();
    // 获取队列中所有发送中的文件
    std::vector<u32> GetAllSendingFileNo();
    // 更新文件状态
    void SetFileStatus(u32 fileNo, FILE_STATUS state);
    // 更新文件进度
    void SetFileProgress(u32 fileNo, u32 progress);
    // 暂停所有等待中的文件
    void PauseAllWaitingFile();
    // 继续所有暂停中的文件
    void ContinueAllSuspendedFile();
    // 从队列中移除文件
    void RemoveFile(u32 fileNo);
    // 更新本地文件
    void updateLocalFile();

private:
    std::map<u32,FileInfo> m_fileSendingTask;     // 文件任务队列
    Json::Value m_localFileSendingTask;            // 本地备份文件
};

