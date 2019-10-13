#pragma once
#include <string>
#include <osp.h>
#include <kdvtype.h>

#ifdef _EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

class MsgAppData
{
public:
    MsgAppData();
    ~MsgAppData();

    void setUserName(const std::string& name);
    std::string getUserName();

private:
    std::string m_usrName;
};