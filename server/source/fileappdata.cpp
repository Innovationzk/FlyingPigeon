#include <fstream>
#include <sys/stat.h>
#include "../include/fileappdata.h"
#include "../include/msgappdata.h"
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX_
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(dir) _mkdir((dir))
#elif _LINUX_
#define ACCESS access
#define MKDIR(dir) mkdir((dir),0755)
#endif

/****************TODO:正式发布版需要修改******************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

FileAppData::FileAppData()
{
    std::string strFileList = "fileList.json";
    for (long long i = 1; i < MsgAppData::clientNo; i++)
    {
        std::fstream file((std::to_string(i) + "/" + strFileList).c_str(), std::ios::in | std::ios::binary);
        if (file.is_open())
        {
            Json::Reader reader;
            reader.parse(file, m_fileList[i]);
            file.close();
        }
    }
}

FileAppData::~FileAppData()
{

}

u32 FileAppData::GetFileSize(const char* fileFullPath)
{
    struct stat stat_buf;
    int ret = stat(fileFullPath, &stat_buf);
    return ret == 0 ? stat_buf.st_size : 0;
}

bool FileAppData::IsFileExist(u32 clientNo, std::string fileName)
{
    return m_fileList[clientNo].isMember(fileName);
}

void FileAppData::AddFile(u32 clientNo, std::string fileName)
{
    m_fileList[clientNo][fileName] = 0;
    UpdateLocalFile(clientNo);
}

void FileAppData::RemoveFile(u32 clientNo, std::string fileName)
{
    m_fileList[clientNo].removeMember(fileName);
    UpdateLocalFile(clientNo);
}

void FileAppData::SetFileProgress(u32 clientNo, std::string fileName, u32 progress)
{
    m_fileList[clientNo][fileName] = progress;
    UpdateLocalFile(clientNo);
}

void FileAppData::UpdateLocalFile(u32 clientNo)
{
    // 如果目标客户端文件夹不存在，创建文件夹
    std::string strClientNo = std::to_string((long long)clientNo);
    if (0 != ACCESS(strClientNo.c_str(), 0))
    {
        MKDIR(strClientNo.c_str());
    }

    Json::FastWriter fw;
    std::string strFileList = fw.write(m_fileList[clientNo]);
    std::fstream file(strClientNo + "/fileList.json", std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        file.write(strFileList.c_str(), strFileList.size());
        file.close();
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: cannot open file \"%d/fileList.json\"\n", __FUNCTION__, clientNo);
    }
}
