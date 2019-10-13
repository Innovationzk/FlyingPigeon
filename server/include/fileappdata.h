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

    // ��ȡ�ļ���С�����֧��4G�ļ�
    u32 GetFileSize(const char* fileFullPath);
    // ����Ƿ����ļ����ռ�¼
    bool IsFileExist(u32 clientNo, std::string fileName);
    // ����ļ����ļ������б�
    void AddFile(u32 clientNo,std::string fileName);
    // ���ļ������б���ɾ���ļ�
    void RemoveFile(u32 clientNo, std::string fileName);
    // �����ļ����ս���
    void SetFileProgress(u32 clientNo, std::string fileName, u32 progress);
    // ���±����ļ�
    void UpdateLocalFile(u32 clientNo);

private:
    std::map<u32, Json::Value> m_fileList;
};