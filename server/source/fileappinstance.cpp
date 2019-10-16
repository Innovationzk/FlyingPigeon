#pragma once

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX_
#include <stdarg.h>
#include <sys/stat.h>
#endif
#include "../include/fileappdata.h"
#include "../include/fileappinstance.h"
#include "../include/msgappdata.h"
#include "../include/msgappinstance.h"
#include "../../common/md5.h"

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

extern zTemplate<FileAppInstance, FILE_APP_INSTANCE_NUMBER_SERVER, FileAppData, MAX_ALIAS_LENGTH_SERVER> fileApp;
extern zTemplate<MsgAppInstance, MSG_APP_INSTANCE_NUMBER_SERVER, MsgAppData, MAX_ALIAS_LENGTH_SERVER> msgApp;

FileAppInstance::FileAppInstance() :
    m_clientNo(0),
    m_clientNode(0),
	m_fileProgress(0)
{
	// 初始化状态
	NEXTSTATE(INSTANCE_STATUS_IDLE);
}

FileAppInstance::~FileAppInstance()
{

}

void FileAppInstance::DaemonInstanceEntry(CMessage* const pMsg, CApp* pCApp)
{
	OspPrintf(TRUE, FALSE, "[%s]: called, eventType is:%d\n", __FUNCTION__, pMsg->event);

	switch (pMsg->event)
	{
	case EV_CLT_SER_START_POST_FILE_REQ:
	{
		AssignTask(pMsg);
		break;
	}
	case EV_CLT_SER_CONTINUE_POST_FILE_REQ:
	{
		ContinueTask(pMsg);
		break;
	}
	case EV_CLT_SER_CANCEL_FILE_NTF:
	{
        CancelSuspendedFile(pMsg);
		break;
	}
    case OSP_DISCONNECT:
    {
        StopClientTask(pMsg);
        break;
    }
    default:
        break;
	}
}

void FileAppInstance::AssignTask(CMessage* const pMsg)
{
	/* 查看本地文件记录，若有对应记录则删除
	分配空闲Ins,调用StartReceiveFile()
	若没有空闲，回复 EV_SER_CLT_START_POST_FILE_NACK*/
	OspPrintf(TRUE, FALSE, "[%s]: called,srcNode:%d,srcInsId:%d,fileName:\"%s\"\n", __FUNCTION__, pMsg->srcnode, pMsg->srcid - 0x200000, (char*)(pMsg->content));

    // 遍历instance，若有空闲instance，为其分配文件接收任务
	CApp* pCApp = (CApp*)(&fileApp);
	for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_SERVER; i++)
	{
		FileAppInstance* instance = (FileAppInstance*)(pCApp->GetInstance(i));
		if (instance->CurState() == INSTANCE_STATUS_IDLE)
		{
			// 若有空闲Instance，则分配任务并回复确认
			OspPrintf(TRUE, FALSE, "[%s]: assign task,insId:%d,fileName:%s\n", __FUNCTION__, i, (char*)(pMsg->content));
			if (TRUE == instance->StartReceiveFile(pMsg))
			{
                ser_clt_start_post_file_ack msg;
				msg.InstanceNo = i;
				post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_START_POST_FILE_ACK, &msg, sizeof(msg), pMsg->srcnode);
				return;
			}
		}
	}
	// 没有空闲Instance，回复拒绝接收
	OspPrintf(TRUE, FALSE, "[%s]: all instance busy,refuse to receive file\n", __FUNCTION__);
	post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_START_POST_FILE_NACK, NULL, 0, pMsg->srcnode);
}

void FileAppInstance::ContinueTask(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,srcNode:%d,srcInsId:%d,fileName:\"%s\"\n", __FUNCTION__, pMsg->srcnode, pMsg->srcid - 0x200000, (char*)(pMsg->content));

    CApp* pCApp = (CApp*)(&fileApp);
    for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_SERVER; ++i)
    {
        FileAppInstance* instance = (FileAppInstance*)(pCApp->GetInstance(i));
        if (instance->CurState() == INSTANCE_STATUS_IDLE)
        {
            // 若有空闲Instance，则分配任务并回复确认
            OspPrintf(TRUE, FALSE, "[%s]: assign task,insId:%d,fileName:%s\n", __FUNCTION__, i, (char*)(pMsg->content));
            if (TRUE == instance->ContinueReceiveFile(pMsg))
            {
                ser_clt_continue_post_file_ack msg;
                char* fileName = (char*)(pMsg->content);
                msg.InstanceNo = i;
                msg.progress = instance->m_fileProgress;
                OspPrintf(TRUE, FALSE, "[%s]: send continue_post_file_ack\n", __FUNCTION__);
                post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_CONTINUE_POST_FILE_ACK, &msg, sizeof(msg), pMsg->srcnode);
                return;
            }
        }
    }

    // 没有空闲Instance，回复拒绝接收
    OspPrintf(TRUE, FALSE, "[%s]: all instance busy,refuse to receive file\n", __FUNCTION__);
    post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_CONTINUE_POST_FILE_NACK, NULL, 0, pMsg->srcnode);
}

void FileAppInstance::CancelSuspendedFile(CMessage* const pMsg)    // 从本地记录中删除对应记录
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__, pMsg->event);

    u32 clientNo = msgApp.getOnlineClientNo(pMsg->srcnode);
    std::string fileName = ((clt_ser_cancel_file_ntf*)(pMsg->content))->fileName;
    std::string fileNameFullPath = std::to_string((long long)clientNo) + "/" + fileName;
    if (0 == remove(fileNameFullPath.c_str()))
    {
        OspPrintf(TRUE, FALSE, "[%s]: remove file \"%s\"\n", __FUNCTION__, fileNameFullPath.c_str());
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: file \"%s\" does not exist\n", __FUNCTION__, fileNameFullPath.c_str());
    }
    fileApp.RemoveFile(clientNo, fileName.c_str());
}

void FileAppInstance::StopClientTask(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__, pMsg->event);

    u32 clientNode = *pMsg->content;
    OspPrintf(TRUE, FALSE, "[%s]: clentNode: %d disconnect\n", __FUNCTION__, clientNode);
    CApp* pCApp = (CApp*)(&fileApp);
    for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_SERVER; i++)
    {
        FileAppInstance* instance = (FileAppInstance*)(pCApp->GetInstance(i));
        // 若该Instance配对的client已经断开，中断其文件接收任务
        if (INSTANCE_STATUS_RECEIVE_FILE == instance->CurState() && clientNode == instance->m_clientNode)
        {
            OspPrintf(TRUE, FALSE, "[%s]: instanceId:%d stop receive file\n", __FUNCTION__, clientNode);
            instance->ClearData();
        }
    }
}

// 普通Instanc处理逻辑
void FileAppInstance::InstanceEntry(CMessage* const pMsg)
{
	OspPrintf(TRUE, FALSE, "[%s]: called, instanceId:%d, eventType is:%d\n", __FUNCTION__, m_instId, pMsg->event);

	switch (pMsg->event)
	{
	case EV_CLT_SER_POST_FILE_REQ:
	{
		ReceiveFileContent(pMsg);
		break;
	}
	case EV_CLT_SER_FILE_MD5_REQ:
	{
        VerifyMD5Code(pMsg);
        break;
	}
	case EV_CLT_SER_PAUSE_FILE_REQ:
	{
        PauseFile(pMsg);
        break;
	}
	case EV_CLT_SER_CANCEL_FILE_REQ:
	{
        CancelSendingFile(pMsg);
        break;
	}
    default:
        break;
	}
}

bool FileAppInstance::StartReceiveFile(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,instanceId:%d\n", __FUNCTION__, m_instId);

    // 如果目标客户端文件夹不存在，创建文件夹
    m_clientNo = msgApp.getOnlineClientNo(pMsg->srcnode);
	std::string strClientNo = std::to_string((long long)m_clientNo);
	if (0 != ACCESS(strClientNo.c_str(), 0))
	{
		MKDIR(strClientNo.c_str());
	}

    // 保存文件名并新建本地文件
    m_clientNode = pMsg->srcnode;
    clt_ser_start_post_file_req* msg = (clt_ser_start_post_file_req*)(pMsg->content);
    m_fileName = std::string(msg->fileName);
    m_fileNameFullPath = strClientNo + "/" + m_fileName;
    m_fileProgress = 0;
	m_file.open(m_fileNameFullPath, std::ios::out | std::ios::binary);
	if (!m_file.is_open())
	{
		OspPrintf(TRUE, FALSE, "[%s]: error:cannot create file \"%s\"\n", __FUNCTION__, m_fileNameFullPath.c_str());
		NextState(INSTANCE_STATUS_IDLE);
		return FALSE;
	}
    OspPrintf(TRUE, FALSE, "[%s]: start receive file \"%s\"\n", __FUNCTION__, m_fileNameFullPath.c_str());
    NextState(INSTANCE_STATUS_RECEIVE_FILE);

    // 若本地有该文件接收记录，则删除
    fileApp.RemoveFile(m_clientNo, m_fileName);

    // 保存信息至本地文件
    fileApp.AddFile(m_clientNo,m_fileName);
	return TRUE;
}

void FileAppInstance::ReceiveFileContent(CMessage* const pMsg)
{
	OspPrintf(TRUE, FALSE, "[%s]: called,file \"%s\" receive content\n", __FUNCTION__, m_fileName.c_str());

	if (INSTANCE_STATUS_RECEIVE_FILE != m_curState)  // Instance状态异常
	{
		OspPrintf(TRUE, FALSE, "[%s]: error:Instance status error,curState:%d\n", __FUNCTION__, m_curState);
		return;
	}
	// 保存数据并更新进度
	clt_ser_post_file_ntf* msg = (clt_ser_post_file_ntf*)(pMsg->content);
	m_file.write(msg->fileContent, msg->length);
    m_file.flush();
	m_fileProgress += msg->length;
    fileApp.SetFileProgress(m_clientNo, m_fileName, m_fileProgress);
    OspPrintf(TRUE, FALSE, "[%s]: curProgress:%d, send post_file_ack to client\n", __FUNCTION__, m_fileProgress);
	post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_POST_FILE_ACK, NULL, 0, pMsg->srcnode);
}

void FileAppInstance::VerifyMD5Code(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called", __FUNCTION__);

    if (m_file.is_open())
    {
        m_file.close();
    }
    MD5 md5;
    char* md5Code = md5.digestFile(m_fileNameFullPath.c_str());
    clt_ser_file_md5_req *md5Msg=(clt_ser_file_md5_req*)(pMsg->content);
    OspPrintf(TRUE, FALSE, "[%s]: client md5Code: %s,server md5Code: %s\n", __FUNCTION__, md5Msg->md5, md5Code);
    if (0==strcmp(md5Msg->md5, md5Code))
    {
        OspPrintf(TRUE, FALSE, "[%s]: file \"%s\" verify success,end task\n", __FUNCTION__, m_fileName.c_str());
        fileApp.RemoveFile(m_clientNo, m_fileName);
        ClearData();
        OspPrintf(TRUE, FALSE, "[%s]: send FILE_MD5_ACK to client\n", __FUNCTION__, m_fileName.c_str());
        post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_FILE_MD5_ACK, NULL, 0, pMsg->srcnode);
    }
    else
    {
        // else 校验失败，重新接收文件
        OspPrintf(TRUE, FALSE, "[%s]: file \"%s\" verify failed,clear local data\n", __FUNCTION__, m_fileName.c_str());
        fileApp.SetFileProgress(m_clientNo, m_fileName, 0);
        m_fileProgress = 0;
        if (m_file.is_open())
        {
            m_file.close();
        }
        m_file.open(m_fileName.c_str(), std::ios::out | std::ios::binary);
        OspPrintf(TRUE, FALSE, "[%s]: send FILE_MD5_NACK to client\n", __FUNCTION__, m_fileName.c_str());
        post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_FILE_MD5_NACK, NULL, 0, pMsg->srcnode);
    }
}

void FileAppInstance::PauseFile(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,instanceId:%d,pause file \"%s\"\n", __FUNCTION__, m_instId, m_fileName.c_str());

    // 清除数据
    ClearData();

    OspPrintf(TRUE, FALSE, "[%s]: send pause_file_ack\n", __FUNCTION__);
    post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_PAUSE_FILE_ACK, NULL, 0, pMsg->srcnode);
}

bool FileAppInstance::ContinueReceiveFile(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called,instanceId:%d\n", __FUNCTION__, m_instId);

    // 如果目标客户端文件夹不存在，创建文件夹
    m_clientNo = msgApp.getOnlineClientNo(pMsg->srcnode);
    m_clientNode = pMsg->srcnode;
    std::string strClientNo = std::to_string((long long)m_clientNo);
    if (0 != ACCESS(strClientNo.c_str(), 0))
    {
        MKDIR(strClientNo.c_str());
    }

    // 恢复文件数据
    clt_ser_continue_post_file_req* msg = (clt_ser_continue_post_file_req*)(pMsg->content);
    m_fileName = std::string(msg->fileName);
    m_fileNameFullPath = strClientNo + "/" + m_fileName;
    if (0 == ACCESS(m_fileNameFullPath.c_str(), 0))  // 若文件存在，恢复进度
    {
        m_fileProgress = fileApp.GetFileSize(m_fileNameFullPath.c_str());
        m_file.open(m_fileNameFullPath, std::ios::out | std::ios::app | std::ios::binary);
        OspPrintf(TRUE, FALSE, "[%s]: file progress:%d, continue receive file\n", __FUNCTION__, m_fileProgress);
    }
    else // 文件不存在，新建文件
    {
        m_fileProgress = 0;
        m_file.open(m_fileNameFullPath, std::ios::out | std::ios::binary);
        OspPrintf(TRUE, FALSE, "[%s]: file \"%s\" do not exist,create new\n", __FUNCTION__, m_fileName.c_str());
    }
    if (!m_file.is_open())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:cannot open file \"%s\"\n", __FUNCTION__, m_fileNameFullPath.c_str());
        NextState(INSTANCE_STATUS_IDLE);
        return FALSE;
    }
    NextState(INSTANCE_STATUS_RECEIVE_FILE);
    fileApp.SetFileProgress(m_clientNo, m_fileName, m_fileProgress);
    return TRUE;
}

void FileAppInstance::CancelSendingFile(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    m_file.close();
    if (0 == remove(m_fileNameFullPath.c_str()))  // 删除本地文件
    {
        OspPrintf(TRUE, FALSE, "[%s]: remove file \"%s\"\n", __FUNCTION__, m_fileNameFullPath.c_str());
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: file \"%s\" does not exist\n", __FUNCTION__, m_fileNameFullPath.c_str());
    }
    // 清除数据
    fileApp.RemoveFile(m_clientNo, m_fileName.c_str());
    ClearData();
    OspPrintf(TRUE, FALSE, "[%s]: send cancel_file_ack to client\n", __FUNCTION__);
    post(MAKEIID(FILE_APP_ID_CLIENT, pMsg->srcid), EV_SER_CLT_CANCEL_FILE_ACK, NULL, 0, pMsg->srcnode);
}

void FileAppInstance::ClearData()
{
    // 清除数据
    m_clientNo = 0;
    m_clientNode = 0;
    if (m_file.is_open())
    {
        m_file.close();
    }
    m_fileName.clear();
    m_fileNameFullPath.clear();
    m_fileProgress = 0;
    NEXTSTATE(INSTANCE_STATUS_IDLE);
}
