#pragma once
#include <map>
#include "../../common/osp.h"
#include "../../common/json/json.h"
#include "../../common/commondef.h"

class FileAppData
{
public:
    FileAppData();
    ~FileAppData();

    // 获取文件大小，最大支持4G文件
    u32 GetFileSize(const char* fileFullPath);
    // 检查是否有文件接收记录
    bool IsFileExist(u32 clientNo, std::string fileName);
    // 添加文件至文件接收列表
    void AddFile(u32 clientNo,std::string fileName);
    // 从文件接收列表中删除文件
    void RemoveFile(u32 clientNo, std::string fileName);
    // 更新文件接收进度
    void SetFileProgress(u32 clientNo, std::string fileName, u32 progress);
    // 更新本地文件
    void UpdateLocalFile(u32 clientNo);

private:
    std::map<u32, Json::Value> m_fileList;
};