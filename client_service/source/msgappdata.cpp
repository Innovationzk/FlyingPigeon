#include <msgappdata.h>
#include <commondef.h>

MsgAppData::MsgAppData()
{

}

MsgAppData::~MsgAppData()
{

}

void MsgAppData::setUserName(const std::string& name)
{
    m_usrName = name;
}

std::string MsgAppData::getUserName()
{
    return m_usrName;
}

