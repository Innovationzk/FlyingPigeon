#pragma once
#include <fstream>
#include "../../common/osp.h"
#include "../../common/commondef.h"

class DemoInstanceServer:public CInstance
{
public:
    DemoInstanceServer();
    ~DemoInstanceServer();
    void DaemonInstanceEntry( CMessage *const, CApp *);
    void InstanceEntry( CMessage *const pMsg);
    void receiveFile(CMessage *const pMsg);
    inline const char* getFileNameFromFullPath(const char* chr);

private:
    std::fstream file;
};