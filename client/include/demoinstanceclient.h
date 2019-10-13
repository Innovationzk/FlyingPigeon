#pragma once

#include <fstream>
#include <sys/stat.h>
#include "../../common/osp.h"
#include "../../common/commondef.h"

class DemoInstanceClient:public CInstance
{
public:
    DemoInstanceClient(void);
    ~DemoInstanceClient(void);
    void InstanceEntry(CMessage *const pMsg);
    void DaemonInstanceEntry(CMessage *const pMsg, CApp *pCApp);
    //void ReqFunction(CMessage *const pMsg);

    void postFile();

    inline void setServerNode(s32 serverNode);
    inline s32 getServerNode(s32 serverNode);
    inline int GetFileSize(char *fileName) {
        struct stat stat_buf;
        int ret = stat(fileName, &stat_buf);
        return ret == 0 ? stat_buf.st_size : -1;
    }

private:
    s32 m_dwServerNode;
    std::fstream file;
    int m_fileSize;
    int m_progress;
    char buffer[FILE_BUF_SIZE];
};

