#include <vector>
#include <fileappdata.h>
#include <fileappinstance.h>
#include <commondef.h>
#include <md5.h>

/****************TODO:正式发布版需要修改******************
#ifdef _DEBUG
#define OspPrintf(TRUE, FALSE, ...) printf(__VA_ARGS__)
#else
#define OspPrintf(a, b, ...)
#endif
********************************************************/

extern zTemplate<FileAppInstance, FILE_APP_INSTANCE_NUMBER_CLIENT, FileAppData, MAX_ALIAS_LENGTH_CLIENT> fileApp;
extern int serverNode;
typedef void (*CallBack)(u16 eventType, const void* content, u16 length);
extern CallBack CallBackFunc;

FileAppInstance::FileAppInstance() :
    m_fileNo(0)
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
    case EV_UI_CLT_POST_FILE_REQ:
    {
        if (OspIsValidTcpNode(serverNode))  // 服务器在线，保存文件信息，分配发送任务
        {
            OspPrintf(TRUE, FALSE, "[%s]: server online,assign task\n", __FUNCTION__);
            CallBackFunc(EV_CLT_UI_POST_FILE_ACK, NULL, 0);
            fileApp.AddFileSendingTasks((ui_clt_post_file_req*)(pMsg->content));
            AssignTask();
        }
        else  // 服务器离线
        {
            OspPrintf(TRUE, FALSE, "[%s]: server offline,refuse to send messages\n", __FUNCTION__);
            CallBackFunc(EV_CLT_UI_POST_FILE_NACK, NULL, 0);
        }
        break;
    }
    case EV_UI_CLT_PAUSE_FILE_CMD:
    {
        PauseTask(pMsg);
        break;
    }
    case EV_UI_CLT_PAUSE_ALL_FILE_CMD:
    {
        PauseAllTask();
        break;
    }
    case EV_UI_CLT_CONTINUE_FILE_CMD:
    {
        ContinueTask(pMsg);
        break;
    }
    case EV_UI_CLT_CONTINUE_ALL_FILE_CMD:
    {
        ContinueAllTask();
        break;
    }
    case EV_UI_CLT_CANCEL_FILE_CMD:
    {
        ui_clt_cancel_file_cmd* msg = (ui_clt_cancel_file_cmd*)(pMsg->content);
        CancelTask(msg->fileNo);
        break;
    }
    case EV_UI_CLT_CANCEL_ALL_FILE_CMD:
    {
        CancelAllTask();
        break;
    }
    case OSP_DISCONNECT:
    {
        OspPrintf(TRUE, FALSE, "[%s]: server node discconnect,clear data\n", __FUNCTION__);
        // 通知instance清除数据，等待重连
        CApp* pCApp = (CApp*)(&fileApp);
        for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
        {
            FileAppInstance* pInstance = (FileAppInstance*)(pCApp->GetInstance(i));
            if (INSTANCE_STATUS_POST_FILE == pInstance->CurState())
            {
                pInstance->ClearData();
            }
        }
        break;
    }
    case EV_CLT_CLT_CONNECT_SUCCEED_NTF:
    {
        OspPrintf(TRUE, FALSE, "[%s]: server reconnect,resume file task\n", __FUNCTION__);
        std::vector<u32> vecSendingFileNo = fileApp.GetAllSendingFileNo();
        int sendingFileNo = 0;
        // 通知instance恢复发送
        CApp* pCApp = (CApp*)(&fileApp);
        for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT && sendingFileNo < vecSendingFileNo.size(); i++)
        {
            OspPrintf(TRUE, FALSE, "[%s]: resume file, fileNo:%d\n", __FUNCTION__, vecSendingFileNo[sendingFileNo]);
            FileAppInstance* pInstance = (FileAppInstance*)(pCApp->GetInstance(i));
            if (INSTANCE_STATUS_IDLE == pInstance->CurState())
            {
                pInstance->ContinueSendFile(vecSendingFileNo[sendingFileNo]);
                ++sendingFileNo;
            }
            else
            {
                OspPrintf(TRUE, FALSE, "[%s]: error:instance status is not idle\n", __FUNCTION__);
            }
        }
        break;
    }
    default:
        break;
    }
}

void FileAppInstance::AssignTask()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    CApp* pCApp = (CApp*)(&fileApp);
    for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
    {
        u32 nextFileNo = fileApp.GetNextFileTask();
        if (0 == nextFileNo)  // 没有等待发送的文件
        {
            OspPrintf(TRUE, FALSE, "[%s]: no file waiting to be sent\n", __FUNCTION__);
            return;
        }

        FileAppInstance* pInstance = (FileAppInstance*)(pCApp->GetInstance(i));
        if (INSTANCE_STATUS_IDLE == pInstance->CurState())  // 如果Instance状态为空闲，为其分配文件发送任务
        {
            OspPrintf(TRUE, FALSE, "[%s]: assign task,insId:%d,fileNo:%d\n", __FUNCTION__, i, nextFileNo);
            pInstance->StartSendFile((u32)nextFileNo);
        }
    }
    OspPrintf(TRUE, FALSE, "[%s]: all instance busy\n", __FUNCTION__);
}

void FileAppInstance::AssignTask(int insId)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, target instance id: %d\n", __FUNCTION__, insId);

    u32 nextFileNo = fileApp.GetNextFileTask();
    if (0 == nextFileNo)  // 没有等待发送的文件
    {
        OspPrintf(TRUE, FALSE, "[%s]: no file waiting to be sent\n", __FUNCTION__);
        return;
    }

    CApp* pCApp = (CApp*)(&fileApp);
    FileAppInstance* pInstance = (FileAppInstance*)(pCApp->GetInstance(insId));
    if (INSTANCE_STATUS_IDLE == pInstance->CurState())  // 如果Instance状态为空闲，为其分配文件发送任务
    {
        OspPrintf(TRUE, FALSE, "[%s]: assign task,insId:%d,fileNo:%d\n", __FUNCTION__, insId, nextFileNo);
        pInstance->StartSendFile((u32)nextFileNo);
    }
}

void FileAppInstance::PauseTask(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    ui_clt_pause_file_cmd* msg = (ui_clt_pause_file_cmd*)(pMsg->content);
    u32 fileNo = msg->fileNo;

    FileAppData::FILE_STATUS status = fileApp.GetFileStatus(fileNo);
    if (FileAppData::FILE_STATUS_WAIT == status)  // 文件为等待中，直接更新状态为pause
    {
        OspPrintf(TRUE, FALSE, "[%s]: fileNo:%d, current status： waiting, change status to pause\n", __FUNCTION__, fileNo);
        fileApp.SetFileStatus(msg->fileNo, FileAppData::FILE_STATUS_PAUSE);
    }
    else if (FileAppData::FILE_STATUS_SEND == status)  // 文件正在发送中，暂停发送
    {
        OspPrintf(TRUE, FALSE, "[%s]: fileNo:%d, current status： sending, pause sending\n", __FUNCTION__, fileNo);
        CApp* pCApp = (CApp*)& fileApp;
        for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
        {
            FileAppInstance* fileAppInstance = (FileAppInstance*)(pCApp->GetInstance(i));
            if (fileAppInstance->GetFileNo() == fileNo)
            {
                fileAppInstance->PauseFile();
                return;
            }
        }
        OspPrintf(TRUE, FALSE, "[%s]: error:file status:sending,but no instance is sending it\n", __FUNCTION__, fileNo);
    }
}

void FileAppInstance::PauseAllTask()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    // 暂停所有等待中的文件
    fileApp.PauseAllWaitingFile();

    // 暂停所有发送中的文件
    CApp* pCApp = (CApp*)& fileApp;
    for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
    {
        FileAppInstance* fileAppInstance = (FileAppInstance*)(pCApp->GetInstance(i));
        if (INSTANCE_STATUS_POST_FILE == fileAppInstance->CurState())
        {
            fileAppInstance->PauseFile();
        }
    }
}

void FileAppInstance::ContinueTask(CMessage* const pMsg)
{
    /*检查该文件状态：
    暂停中：改变状态为等待发送，遍历Ins，若有空闲Ins，为其中一个Ins分配任务：AssignTask(insId);*/
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    ui_clt_continue_file_cmd* msg = (ui_clt_continue_file_cmd*)(pMsg->content);
    FileAppData::FILE_STATUS status = fileApp.GetFileStatus(msg->fileNo);

    if (FileAppData::FILE_STATUS_PAUSE == status)  // 若文件状态为暂停，更新状态为等待中
    {
        OspPrintf(TRUE, FALSE, "[%s]: update file status,curState:pause,nextState:waiting\n", __FUNCTION__);
        fileApp.SetFileStatus(msg->fileNo, FileAppData::FILE_STATUS_WAIT);

        // 遍历Instance，若有空闲Instance，为其分配文件发送任务
        CApp* pCApp = (CApp*)& fileApp;
        for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
        {
            FileAppInstance* fileAppInstance = (FileAppInstance*)(pCApp->GetInstance(i));
            if (INSTANCE_STATUS_IDLE == fileAppInstance->CurState())
            {
                OspPrintf(TRUE, FALSE, "[%s]: continue send file,instanceId:%d,fileNo:%d\n", __FUNCTION__, i, msg->fileNo);
                fileAppInstance->ContinueSendFile(msg->fileNo);
                return;
            }
        }
        OspPrintf(TRUE, FALSE, "[%s]: all instance busy,fileNo:%d waiting to de sent\n", __FUNCTION__, msg->fileNo);
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: file status:%d,do nothing\n", __FUNCTION__, status);
    }
}

void FileAppInstance::ContinueAllTask()
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    // 将所有pause状态更新为waiting状态
    fileApp.ContinueAllSuspendedFile();
    // 为所有空闲Ins分配任务
    AssignTask();
}

void FileAppInstance::CancelTask(u32 fileNo)
{
    OspPrintf(TRUE, FALSE, "[%s]: called\n", __FUNCTION__);

    FileAppData::FILE_STATUS status = fileApp.GetFileStatus(fileNo);
    if (FileAppData::FILE_STATUS_WAIT == status)  // 文件为等待中，直接删除文件发送任务
    {
        OspPrintf(TRUE, FALSE, "[%s]: file status: waiting,remove file-sending-task\n", __FUNCTION__, fileNo);
        fileApp.RemoveFile(fileNo);
    }
    else if (FileAppData::FILE_STATUS_PAUSE == status)  // 文件为暂停中，通知server取消发送
    {
        OspPrintf(TRUE, FALSE, "[%s]: file status: pause,send cancel_file_req to server daemon instance\n", __FUNCTION__);
        clt_ser_cancel_file_ntf cancelFileMsg;
        std::string fileName = fileApp.GetFileName(fileNo);
        fileName.erase(0, fileName.find_last_of("/") + 1);
        strcpy(cancelFileMsg.fileName, fileName.c_str());
        fileApp.RemoveFile(fileNo);
        post(MAKEIID(FILE_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_CANCEL_FILE_NTF, &cancelFileMsg, sizeof(cancelFileMsg), serverNode);
    }
    else if (FileAppData::FILE_STATUS_SEND == status)  // 文件正在发送中，通知instance取消发送
    {
        OspPrintf(TRUE, FALSE, "[%s]: file status: sending,notify instance to cancel file\n", __FUNCTION__);
        CApp* pCApp = (CApp*)& fileApp;
        for (int i = 1; i <= FILE_APP_INSTANCE_NUMBER_CLIENT; i++)
        {
            FileAppInstance* fileAppInstance = (FileAppInstance*)(pCApp->GetInstance(i));
            if (fileAppInstance->GetFileNo() == fileNo)
            {
                fileAppInstance->CancelFile();
                return;
            }
        }
    }
}

void FileAppInstance::CancelAllTask()
{
    std::vector<u32> vecFileNo = fileApp.GetAllFileNo();
    for (int i = 0; i < vecFileNo.size(); i++)
    {
        CancelTask(vecFileNo[i]);
    }
}

void FileAppInstance::InstanceEntry(CMessage* const pMsg)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, curInstanceID is:%d,eventType is:%d\n", __FUNCTION__, m_instId, pMsg->event);

    switch (pMsg->event)
    {
    case EV_SER_CLT_START_POST_FILE_ACK:
    {
        // 通知UI开始发送（通知发送进度0）
        ui_clt_post_file_progress_ntf msgProgress;
        msgProgress.fileNo = m_fileNo;
        msgProgress.progress = 0;
        CallBackFunc(EV_CLT_UI_POST_FILE_PROGRESS_NTF, &msgProgress, sizeof(msgProgress));
        // 保存服务端Instance编号，发送第一条数据
        ser_clt_start_post_file_ack* msg = (ser_clt_start_post_file_ack*)(pMsg->content);
        m_serverInsId = msg->InstanceNo;
        SendFileContent();
        break;
    }
    case EV_SER_CLT_START_POST_FILE_NACK:
    {
        OspPrintf(TRUE, FALSE, "[%s]: insufficient server resources,pause all file\n", __FUNCTION__);
        // 通知UI服务端资源不足
        clt_ui_post_file_fail_ntf msg;
        msg.fileNo = m_fileNo;
        CallBackFunc(EV_CLT_UI_POST_FILE_FAIL_NTF, &msg, sizeof(msg));
        // 更新文件状态
        fileApp.SetFileStatus(m_fileNo, FileAppData::FILE_STATUS_PAUSE);
        // 暂停所有等待中的文件
        fileApp.PauseAllWaitingFile();
        break;
    }
    case EV_SER_CLT_POST_FILE_ACK:
    {
        if (fileApp.GetFileStatus(m_fileNo) != FileAppData::FILE_STATUS_SEND)  // 文件已暂停发送，不再发送下一条数据包
        {
            OspPrintf(TRUE, FALSE, "[%s]: file paused, stop sending data\n", __FUNCTION__);
            break;
        }


        // 通知UI进度
        if (((u64)m_progress * 100 - BUFFER_SIZE * 100) / m_size != ((u64)m_progress * 100 / m_size))
        {
            ui_clt_post_file_progress_ntf msg;
            msg.fileNo = m_fileNo;
            msg.progress = (u64)m_progress * 100 / m_size;
            OspPrintf(TRUE, FALSE, "[%s]: ***** progress: %d\n", __FUNCTION__, msg.progress);
            CallBackFunc(EV_CLT_UI_POST_FILE_PROGRESS_NTF, &msg, sizeof(msg));
        }
        // 继续发送下一条数据包
        SendFileContent();
        break;
    }
    case EV_SER_CLT_FILE_MD5_ACK:
    {
        // 结束发送任务
        FinishTask();
        // 继续分配下一个任务
        AssignTask(m_instId);
        break;
    }
    case EV_SER_CLT_FILE_MD5_NACK:
    {
        // 通知UI文件发送失败
        clt_ui_resend_file_ntf msg;
        msg.fileNo = m_fileNo;
        CallBackFunc(EV_CLT_UI_RESEND_FILE_NTF, &msg, sizeof(msg));
        // 重新发送文件
        ResendFile();
        break;
    }
    case EV_SER_CLT_PAUSE_FILE_ACK:
    {
        OspPrintf(TRUE, FALSE, "[%s]: receive pause_file_ack, pause file\n", __FUNCTION__);
        // 清除数据
        ClearData();
        // 继续分配下一个任务
        AssignTask(m_instId);
        break;
    }
    case EV_SER_CLT_CONTINUE_POST_FILE_ACK:
    {
        // 根据server回复的进度，确定下一个数据包的位置，继续发送
        ser_clt_continue_post_file_ack* msg = (ser_clt_continue_post_file_ack*)(pMsg->content);
        m_serverInsId = msg->InstanceNo;
        m_progress = msg->progress;
        m_file.seekg(msg->progress);
        OspPrintf(TRUE, FALSE, "[%s]: server progress is %d,continue send file content\n", __FUNCTION__, m_progress);
        SendFileContent();
        break;
    }
    case EV_SER_CLT_CONTINUE_POST_FILE_NACK:
    {
        OspPrintf(TRUE, FALSE, "[%s]: insufficient server resources,pause all file\n", __FUNCTION__);
        // 通知UI服务端资源不足
        clt_ui_post_file_fail_ntf msg;
        msg.fileNo = m_fileNo;
        CallBackFunc(EV_CLT_UI_POST_FILE_FAIL_NTF, &msg, sizeof(msg));
        // 更新文件状态
        fileApp.SetFileStatus(m_fileNo, FileAppData::FILE_STATUS_PAUSE);
        // 暂停所有等待中的文件
        fileApp.PauseAllWaitingFile();
        break;
    }
    case EV_SER_CLT_CANCEL_FILE_ACK:
    {
        // 从队列中删除文件，并尝试分配下一个文件发送任务
        fileApp.RemoveFile(m_fileNo);
        ClearData();
        AssignTask(m_instId);
        break;
    }
    default:
        break;
    }
}

void FileAppInstance::StartSendFile(u32 fileNo)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, instanceId:%d,fileNo:%d\n", __FUNCTION__, m_instId, fileNo);

    // 获取文件信息
    FileAppData::FileInfo fileInfo = fileApp.GetFileInfo(fileNo);
    if (!fileInfo.name.empty())
    {
        m_fileNo = fileNo;
        m_fileNameFullPath = fileInfo.name;
        m_fileName = m_fileNameFullPath.substr(m_fileNameFullPath.find_last_of('/') + 1);
        m_progress = fileInfo.progress;
        m_size = fileInfo.size;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return;
    }

    // 打开文件流
    m_file.open(m_fileNameFullPath.c_str(), std::ios::in | std::ios::binary);
    if (!m_file.is_open())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:cannot open file \"%d\"\n", __FUNCTION__, fileNo);
        return;
    }

    NEXTSTATE(INSTANCE_STATUS_POST_FILE);
    if (0 == m_progress)
    {
        // 向服务端请求开始发送文件
        clt_ser_start_post_file_req msg;
        strcpy(msg.fileName, m_fileName.c_str());
        OspPrintf(TRUE, FALSE, "[%s]: send start-post-file REQ,fileName:%s\n", __FUNCTION__, msg.fileName);
        post(MAKEIID(FILE_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_START_POST_FILE_REQ, &msg, sizeof(msg), serverNode);
    }
    else
    {
        // 向服务端请求继续发送文件
        clt_ser_continue_post_file_req msg;
        strcpy(msg.fileName, m_fileName.c_str());
        OspPrintf(TRUE, FALSE, "[%s]: send continue-post-file REQ,fileName:%s\n", __FUNCTION__, msg.fileName);
        post(MAKEIID(FILE_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_CONTINUE_POST_FILE_REQ, &msg, sizeof(msg), serverNode);
    }
    // 更新文件状态
    fileApp.SetFileStatus(fileNo, FileAppData::FILE_STATUS_SEND);
}

void FileAppInstance::ContinueSendFile(u32 fileNo)
{
    OspPrintf(TRUE, FALSE, "[%s]: called, instanceId:%d,fileNo:%d\n", __FUNCTION__, m_instId, fileNo);

    // 获取文件信息
    FileAppData::FileInfo fileInfo = fileApp.GetFileInfo(fileNo);
    if (!fileInfo.name.empty())
    {
        m_fileNo = fileNo;
        m_fileNameFullPath = fileInfo.name;
        m_fileName = m_fileNameFullPath.substr(m_fileNameFullPath.find_last_of('/') + 1);
        m_progress = fileInfo.progress;
        m_size = fileInfo.size;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:file %d do not exist\n", __FUNCTION__, fileNo);
        return;
    }

    // 打开文件流
    m_file.open(m_fileNameFullPath.c_str(), std::ios::in | std::ios::binary);
    if (!m_file.is_open())
    {
        OspPrintf(TRUE, FALSE, "[%s]: error:cannot open file \"%d\"\n", __FUNCTION__, fileNo);
        return;
    }

    NEXTSTATE(INSTANCE_STATUS_POST_FILE);
    // 向服务端请求继续发送文件
    clt_ser_continue_post_file_req msg;
    strcpy(msg.fileName, m_fileName.c_str());
    OspPrintf(TRUE, FALSE, "[%s]: send continue-post-file REQ,fileName:%s\n", __FUNCTION__, msg.fileName);
    post(MAKEIID(FILE_APP_ID_SERVER, CInstance::DAEMON), EV_CLT_SER_CONTINUE_POST_FILE_REQ, &msg, sizeof(msg), serverNode);
    // 更新文件状态
    fileApp.SetFileStatus(fileNo, FileAppData::FILE_STATUS_SEND);
}

void FileAppInstance::SendFileContent()
{
    OspPrintf(TRUE, FALSE, "[%s]: called, fileName: \"%s\", progress: %d(%d%%)\n", __FUNCTION__, m_fileName.c_str(), m_progress, (u64)m_progress * 100 / m_size);

    if (m_file.eof())  // 文件内容已发送完毕，发送MD5校验码
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
        clt_ser_file_md5_req md5Msg;
        MD5 md5;
        strcpy(md5Msg.md5, md5.digestFile(m_fileNameFullPath.c_str()));
        OspPrintf(TRUE, FALSE, "[%s]: send file \"%s\" complete, send MD5 check code \"%s\"\n", __FUNCTION__, m_fileName.c_str(), md5Msg.md5);
        post(MAKEIID(FILE_APP_ID_SERVER, m_serverInsId), EV_CLT_SER_FILE_MD5_REQ, &md5Msg, sizeof(md5Msg), serverNode);
        return;
    }

    clt_ser_post_file_ntf msg;
    m_file.read(msg.fileContent, sizeof(msg.fileContent));  // 读取文件内容
    if (m_file.eof())   // 已经读到文件尾，发送最后一个数据包
    {
        msg.length = m_file.gcount();
        m_progress += msg.length;
        OspPrintf(TRUE, FALSE, "[%s]: send ths last content,length of content:%d\n", __FUNCTION__, m_fileName.c_str(), m_progress, msg.length);
        post(MAKEIID(FILE_APP_ID_SERVER, m_serverInsId), EV_CLT_SER_POST_FILE_REQ, &msg, sizeof(msg), serverNode);
    }
    else  // 发送文件内容
    {
        OspPrintf(TRUE, FALSE, "[%s]: send file content, data length: %d\n", __FUNCTION__, BUFFER_SIZE);
        msg.length = BUFFER_SIZE;
        m_progress += msg.length;
        post(MAKEIID(FILE_APP_ID_SERVER, m_serverInsId), EV_CLT_SER_POST_FILE_REQ, &msg, sizeof(msg), serverNode);
    }
}

void FileAppInstance::PauseFile()
{
    OspPrintf(TRUE, FALSE, "[%s]: called,send pause_file_req\n", __FUNCTION__);
    fileApp.SetFileStatus(m_fileNo, FileAppData::FILE_STATUS_PAUSE);
    fileApp.SetFileProgress(m_fileNo, m_progress);
    post(MAKEIID(FILE_APP_ID_SERVER, m_serverInsId), EV_CLT_SER_PAUSE_FILE_REQ, NULL, 0, serverNode);
}

void FileAppInstance::CancelFile()
{
    OspPrintf(TRUE, FALSE, "[%s]: called, instanceId:%d,fileNo:%d\n", __FUNCTION__, m_instId, m_fileNo);
    fileApp.SetFileStatus(m_fileNo, FileAppData::FILE_STATUS_PAUSE);
    fileApp.SetFileProgress(m_fileNo, m_progress);

    OspPrintf(TRUE, FALSE, "[%s]: send cancel_file_req to server\n", __FUNCTION__, m_instId, m_fileNo);
    post(MAKEIID(FILE_APP_ID_SERVER, m_serverInsId), EV_CLT_SER_CANCEL_FILE_REQ, NULL, 0, serverNode);
}

void FileAppInstance::ResendFile()
{
    OspPrintf(TRUE, FALSE, "[%s]: called，cleaar progress, send first content\n", __FUNCTION__);

    // 重新发送第一条数据包
    m_progress = 0;
    if (m_file.is_open())
    {
        m_file.seekg(0);
    }
    else
    {
        m_file.open(m_fileNameFullPath.c_str(), std::ios::in | std::ios::binary);
    }
    SendFileContent();
}

void FileAppInstance::FinishTask()
{
    OspPrintf(TRUE, FALSE, "[%s]: called,clear data\n", __FUNCTION__);

    // 通知UI文件发送完成
    clt_ui_post_file_complete_ntf msg;
    msg.fileNo = m_fileNo;
    CallBackFunc(EV_CLT_UI_POST_FILE_COMPLETE_NTF, &msg, sizeof(msg));

    // 从文件队列中移除发送任务
    fileApp.RemoveFile(m_fileNo);

    ClearData();
}

void FileAppInstance::ClearData()
{
    // 清空数据
    NEXTSTATE(INSTANCE_STATUS_IDLE);
    if (m_file.is_open())
    {
        m_file.close();
    }
    m_fileNo = 0;
    m_fileName.clear();
    m_fileNameFullPath.clear();
    m_serverInsId = 0;
    m_progress = 0;
    m_size = 0;
}

u32 FileAppInstance::GetFileNo()
{
    return m_fileNo;
}


