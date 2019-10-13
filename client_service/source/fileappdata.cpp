#include <sys/stat.h>
#include <io.h>
#include <fstream>
#include <fileappdata.h>
#include <commondef.h>

/****************TODO:正式发布版需要修改******************/
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
/********************************************************/

u32 FileAppData::fileNo = 1;

FileAppData::FileAppData()
{
    std::fstream file("localFileSendingTask.json", std::ios::in | std::ios::binary);
    if (file.is_open())   // 读取本地备份文件
    {
        Json::Reader reader;
        reader.parse(file, m_localFileSendingTask);
        FileInfo fileInfo;
        for (Json::Value::iterator it = m_localFileSendingTask.begin(); it != m_localFileSendingTask.end(); it++)
        {
            std::string fileNumber = it.key().asString();
            fileInfo.name = m_localFileSendingTask[fileNumber][0].asString();
            fileInfo.status = (FileAppData::FILE_STATUS)(m_localFileSendingTask[fileNumber][1].asUInt());
            fileInfo.progress = m_localFileSendingTask[fileNumber][2].asUInt();
            fileInfo.size = m_localFileSendingTask[fileNumber][3].asUInt();
            m_fileSendingTask[std::stoi(fileNumber)] = fileInfo;
        }
    }
    else
    {
        file.open("localFileSendingTask.json", std::ios::out | std::ios::binary);
    }
    file.close();
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

void FileAppData::AddFileSendingTasks(ui_clt_post_file_req* tasks)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    char* pFileName = tasks->fileName;
    FileInfo fileInfo;
    for (int i = 0; i < tasks->fileNum; i++)
    {
        if (-1 == _access(pFileName, 04))  // 检查文件
        {
            if (-1 == _access(pFileName, 00))  // 文件不存在
            {
                OspPrintf(TRUE, FALSE, "[%s]: error:file \"%s\" don't exit\n", __FUNCTION__, pFileName);
            }
            else  // 文件不可读
            {
                OspPrintf(TRUE, FALSE, "[%s]: error:can't read file \"%s\"\n", __FUNCTION__, pFileName);
            }
            continue;
        }

        // 保存文件信息
        fileInfo.name = pFileName;
        fileInfo.status = FILE_STATUS_WAIT;
        fileInfo.progress = 0;
        fileInfo.size = GetFileSize(pFileName);
        m_fileSendingTask[fileNo] = fileInfo;
        OspPrintf(TRUE, FALSE, "[%s]: add send-file-task,fileNo is:%d,fileName is:%s\n", __FUNCTION__, fileNo, pFileName);

        // 备份文件信息
        std::string strFileNo = std::to_string((long long)fileNo);
        m_localFileSendingTask[strFileNo].append(pFileName);
        m_localFileSendingTask[strFileNo].append(fileInfo.status);
        m_localFileSendingTask[strFileNo].append(fileInfo.progress);
        m_localFileSendingTask[strFileNo].append(fileInfo.size);

        ++fileNo;
        // 将文件名指针移动至下一个文件名处
        while (*++pFileName != '\0');
        ++pFileName;
    }

    // 备份文件信息至本地
    updateLocalFile();

#ifdef _DEBUG
    for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); it++)
    {
        OspPrintf(TRUE, FALSE, "[%s]: %d %s %d %d\n", __FUNCTION__, it->first, it->second.name.c_str(), it->second.progress, it->second.size);
    }
#endif // _DEBUG

}

u32 FileAppData::GetNextFileTask()
{
    for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); ++it)
    {
        if (FILE_STATUS_WAIT == it->second.status)
        {
            OspPrintf(TRUE, FALSE, "[%s]: next fileNo is:%d,fileName is%s\n", __FUNCTION__, it->first, it->second.name.c_str());
            return it->first;
        }
    }
    OspPrintf(TRUE, FALSE, "[%s]: no file waiting to be sent\n", __FUNCTION__);
    return 0;
}

FileAppData::FileInfo FileAppData::GetFileInfo(u32 fileNo)
{
    if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return FileInfo();
    }
    return m_fileSendingTask[fileNo];
}

std::string FileAppData::GetFileName(u32 fileNo)
{
    if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return std::string();
    }
    return m_fileSendingTask[fileNo].name;
}

FileAppData::FILE_STATUS FileAppData::GetFileStatus(u32 fileNo)
{
    if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return FILE_STATUS_ERROR;
    }
    return m_fileSendingTask[fileNo].status;
}

std::vector<u32> FileAppData::GetAllFileNo()
{
    std::vector<u32> ret;

    for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); ++it)
    {
        ret.push_back(it->first);
    }
    return ret;
}

std::vector<u32> FileAppData::GetAllSendingFileNo()
{
    std::vector<u32> ret;

    for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); ++it)
    {
        if (it->second.status == FILE_STATUS_SEND)
        {
            ret.push_back(it->first);
        }
    }
    return ret;
}

void FileAppData::SetFileStatus(u32 fileNo, FILE_STATUS state)
{
    // 更新文件状态
    if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return;
    }
	OspPrintf(TRUE, FALSE, "[%s]: update file status,fileName \"%s\",curStatus:%d,nextStatus:%d\n", __FUNCTION__, m_fileSendingTask[fileNo].name.c_str(), m_fileSendingTask[fileNo].status, state);
    m_fileSendingTask[fileNo].status = state;
    m_localFileSendingTask[std::to_string((long long)fileNo)][1] = state;

    // 备份文件信息至本地
    updateLocalFile();
}

void FileAppData::SetFileProgress(u32 fileNo, u32 progress)
{
    // 更新文件君度
    if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return;
    }
    OspPrintf(TRUE, FALSE, "[%s]: update file progress,fileName \"%s\",progress:%d\n", __FUNCTION__, m_fileSendingTask[fileNo].name.c_str(), progress);
    m_fileSendingTask[fileNo].progress = progress;
    m_localFileSendingTask[std::to_string((long long)fileNo)][2] = progress;

    // 备份文件信息至本地
    updateLocalFile();
}

void FileAppData::PauseAllWaitingFile()
{
    // 更新文件状态
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
	for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); it++)
	{
        if (it->second.status == FILE_STATUS_WAIT)
        {
            it->second.status = FILE_STATUS_PAUSE;
            m_localFileSendingTask[std::to_string((long long)(it->first))][1] = FILE_STATUS_PAUSE;
		}
	}

    // 备份文件信息至本地
    updateLocalFile();
}

void FileAppData::ContinueAllSuspendedFile()
{
    // 更新文件状态
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
    for (std::map<u32, FileInfo>::iterator it = m_fileSendingTask.begin(); it != m_fileSendingTask.end(); it++)
    {
        if (it->second.status == FILE_STATUS_PAUSE)
        {
            it->second.status = FILE_STATUS_WAIT;
            m_localFileSendingTask[std::to_string((long long)(it->first))][1] = FILE_STATUS_WAIT;
        }
    }

    // 备份文件信息至本地
    updateLocalFile();
}

void FileAppData::RemoveFile(u32 fileNo)
{
	if (m_fileSendingTask.find(fileNo) == m_fileSendingTask.end())
	{
		OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
		return;
	}
	OspPrintf(TRUE, FALSE, "[%s]: remove file \"%s\" from fileSendingTask\n", __FUNCTION__, m_fileSendingTask[fileNo].name.c_str());
	m_fileSendingTask.erase(fileNo);
    m_localFileSendingTask.removeMember(std::to_string((long long)fileNo));

    // 备份文件信息至本地
    updateLocalFile();
}

void FileAppData::updateLocalFile()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);
    std::string strLocalFleSendingTask = Json::FastWriter().write(m_localFileSendingTask);
    std::fstream file("localFileSendingTask.json", std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        file.write(strLocalFleSendingTask.c_str(), strLocalFleSendingTask.size());
        file.close();
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: cannot open file \"localFileSendingTask.json\"\n", __FUNCTION__);
    }
}


