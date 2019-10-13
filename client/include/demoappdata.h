#include "../../common/kdvtype.h"

class DemoAppData
{
public:
    DemoAppData();
    inline void setServerNode(s32 serverNode)
    {
        m_nServerNode = serverNode;
    }
    inline s32 getServerNode()
    {
        return m_nServerNode;
    }
    inline void setCallBackSendProgress(void (*p)(int))
    {
        m_callBackSendProgress=p;
    }
    inline void (*getCallBackSendProgress())(int)
    {
        return m_callBackSendProgress;
    }

private:
    s32 m_nServerNode;
    void (*m_callBackSendProgress)(int);
};