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
    // �����ļ�״̬
    enum FILE_STATUS {
        FILE_STATUS_ERROR,     // �쳣״̬
        FILE_STATUS_WAIT,      // �ȴ�����
        FILE_STATUS_SEND,      // ������
        FILE_STATUS_PAUSE,     // ��ͣ״̬
    };
    // �����ļ���Ϣ
    struct FileInfo {
        std::string name;
        FILE_STATUS status;
        u32 progress;
        u32 size;
    };

    static u32 fileNo; // �ļ�Ψһ��ʶ����1��ʼ�ۼ�

    FileAppData();
    ~FileAppData();

    // ��ȡ�ļ���С�����֧��4G�ļ�
    u32 GetFileSize(const char* fileFullPath);
    // ����ļ���������
    void AddFileSendingTasks(ui_clt_post_file_req *tasks);
    // ��ȡ��һ���ȴ����͵����񣬳ɹ������ļ���Ϣ��ʧ��ʱ���ص��ļ���Ϊ���ַ���
    u32 GetNextFileTask();
    // ��ȡ�ļ�����
    FileInfo GetFileInfo(u32 fileNo);
    // ��ȡ�ļ���
    std::string GetFileName(u32 fileNo);
    // ��ȡ�ļ�״̬
    FILE_STATUS GetFileStatus(u32 fileNo);
    // ��ȡ�����������ļ�
    std::vector<u32> GetAllFileNo();
    // ��ȡ���������з����е��ļ�
    std::vector<u32> GetAllSendingFileNo();
    // �����ļ�״̬
    void SetFileStatus(u32 fileNo, FILE_STATUS state);
    // �����ļ�����
    void SetFileProgress(u32 fileNo, u32 progress);
    // ��ͣ���еȴ��е��ļ�
    void PauseAllWaitingFile();
    // ����������ͣ�е��ļ�
    void ContinueAllSuspendedFile();
    // �Ӷ������Ƴ��ļ�
    void RemoveFile(u32 fileNo);
    // ���±����ļ�
    void updateLocalFile();

private:
    std::map<u32,FileInfo> m_fileSendingTask;     // �ļ��������
    Json::Value m_localFileSendingTask;            // ���ر����ļ�
};

