#pragma once
#include "../common/kdvtype.h"

// 缓存区大小
#define BUFFER_SIZE                        (u16)(50<<10)

/***************************传输文件***************************/
#define EV_CLT_SER_START_POST_FILE_REQ           (u16)1       // 客户端请求发送文件
typedef struct {
	char fileName[BUFFER_SIZE];     // 文件名
}clt_ser_start_post_file_req;
#define EV_SER_CLT_START_POST_FILE_ACK           (u16)2       // 服务端确认开始接收文件
typedef struct {
	u8 InstanceNo;                  // 分配的Instance号
}ser_clt_start_post_file_ack;
#define EV_SER_CLT_START_POST_FILE_NACK          (u16)3       // 服务端资源不足

#define EV_CLT_SER_POST_FILE_REQ                 (u16)4       // 客户端发送文件给服务端
typedef struct {
	u32 length;                     // 数据长度
	s8 fileContent[BUFFER_SIZE];    // 文件内容
}clt_ser_post_file_ntf;
#define EV_SER_CLT_POST_FILE_ACK                 (u16)5       // 服务端确认接收文件数据

#define EV_CLT_SER_FILE_MD5_REQ                  (u16)6       // 客户端文件传输完毕，发送md5校验码
typedef struct {
	s8 md5[33];                     // 文件md5校验码
}clt_ser_file_md5_req;

#define EV_SER_CLT_FILE_MD5_ACK                  (u16)7       // 服务端文件校验完成
#define EV_SER_CLT_FILE_MD5_NACK                 (u16)8       // 服务端文件校验失败


#define EV_CLT_SER_PAUSE_FILE_REQ                (u16)9       // 客户端请求暂停
#define EV_SER_CLT_PAUSE_FILE_ACK                (u16)10      // 服务端确认暂停

#define EV_CLT_SER_CONTINUE_POST_FILE_REQ        (u16)11      // 客户端请求继续发送文件
typedef struct {
	char fileName[BUFFER_SIZE];     // 文件名
}clt_ser_continue_post_file_req;
#define EV_SER_CLT_CONTINUE_POST_FILE_ACK        (u16)12      // 服务端确认继续发送文件
typedef struct {
	u8 InstanceNo;                  // 分配的Instance号
	u32 progress;                   // 文件进度
}ser_clt_continue_post_file_ack;
#define EV_SER_CLT_CONTINUE_POST_FILE_NACK       (u16)13      // 服务端拒绝继续发送

#define EV_CLT_SER_CANCEL_FILE_REQ               (u16)14      // 客户端请求取消发送中的文件
#define EV_SER_CLT_CANCEL_FILE_ACK               (u16)15      // 服务端确认取消发送
#define EV_CLT_SER_CANCEL_FILE_NTF               (u16)16      // 客户端通知取消发送暂停中的文件
typedef struct{
	char fileName[BUFFER_SIZE];     // 文件名
}clt_ser_cancel_file_ntf;


/******************************在线客户端列表************************************/
enum CONNECT_RESULT {              // 连接结果
    CONNECTION_SUCCEED,
    CLIENT_NUMBER_LIMIT,
    OCCUPIED_USERNAME
};
#define EV_CLT_SER_REGIST_REQ                    (u16)17      // 客户端发送注册请求
typedef struct {
	s8 userName[BUFFER_SIZE];      // 用户名
}clt_ser_regist_req;
#define EV_SER_CLT_REGIST_ACK                    (u16)18      // 服务端确认注册
#define EV_SER_CLT_REGIST_NACK                   (u16)19      // 服务端拒绝注册
typedef struct {
    CONNECT_RESULT result;         // 注册结果
}ser_clt_regist_nack;

#define EV_CLT_SER_LOG_IN_REQ                    (u16)20      // 客户端发送登录请求
typedef struct {
    s8 userName[BUFFER_SIZE];      // 用户名
}clt_ser_log_in_req;
#define EV_SER_CLT_LOG_IN_ACK                    (u16)21      // 服务端确认登录
#define EV_SER_CLT_LOG_IN_NACK                   (u16)22      // 服务端拒绝登录
typedef struct {
    CONNECT_RESULT result;         // 连接结果
}ser_clt_log_in_nack;

#define EV_SER_CLT_REGIST_LIST_NTF               (u16)23      // 服务端通知客户端在线列表
typedef struct {
	s32 clientNum;                  // 客户端数量
	u32 clientNo[100];              // 客户端编号
	s8 clientName[BUFFER_SIZE];     // 客户端用户名
}ser_clt_regist_list_ntf;


/*********************************发送消息************************************/
#define EV_CLT_SER_POST_MSG_NTF                  (u16)24      // 客户端通知服务端发送消息
typedef struct {
	u16 clientNum;                  // 目标客户端数量
	u32 clientNo[16];               // 目标客户端编号
	s8 msgContent[BUFFER_SIZE];     // 消息内容
}clt_ser_post_msg_ntf;

#define EV_SER_CLT_POST_MSG_NTF                  (u16)25      // 服务端向客户端发送消息
typedef struct {
	u32 srcClientNo;                // 源客户端编号
	s8 msgContent[BUFFER_SIZE];     // 消息内容
}ser_clt_post_msg_ntf;


/***************************UI与底层接口***************************/
#define EV_UI_CLT_POST_FILE_REQ                  (u16)26      // UI请求开始发送文件
typedef struct {
	u16 fileNum;                    // 文件数量
	s8 fileName[BUFFER_SIZE];       // 文件名
}ui_clt_post_file_req;
#define EV_CLT_UI_POST_FILE_ACK                  (u16)27      // 底层确认发送文件
#define EV_CLT_UI_POST_FILE_NACK                 (u16)28      // 服务器离线，底层拒绝发送文件

#define EV_CLT_UI_POST_FILE_PROGRESS_NTF         (u16)29      // 底层通知UI文件进度
typedef struct {
	u32 fileNo;                     // 文件编号
	u32 progress;                   // 文件进度
}ui_clt_post_file_progress_ntf;
#define EV_CLT_UI_POST_FILE_FAIL_NTF             (u16)30      // 底层通知UI服务端资源不足
typedef struct {
    u32 fileNo;                     // 文件编号
}clt_ui_post_file_fail_ntf;

#define EV_CLT_UI_RESEND_FILE_NTF                (u16)31      // 底层通知UI校验失败，重新发送文件
typedef struct {
	u32 fileNo;                     // 文件编号
}clt_ui_resend_file_ntf;
#define EV_CLT_UI_POST_FILE_COMPLETE_NTF         (u16)32      // 底层通知UI文件发送成功
typedef struct {
	u32 fileNo;                     // 文件编号
}clt_ui_post_file_complete_ntf;


#define EV_UI_CLT_PAUSE_FILE_CMD                 (u16)33      // UI通知底层暂停发送
typedef struct {
    u32 fileNo;                     // 文件编号
}ui_clt_pause_file_cmd;
#define EV_UI_CLT_PAUSE_ALL_FILE_CMD             (u16)34      // UI通知底层全部暂停

#define EV_UI_CLT_CONTINUE_FILE_CMD              (u16)35      // UI通知底层继续发送
typedef struct {
    u32 fileNo;                     // 文件编号
}ui_clt_continue_file_cmd;
#define EV_UI_CLT_CONTINUE_ALL_FILE_CMD          (u16)36      // UI通知底层全部继续

#define EV_UI_CLT_CANCEL_FILE_CMD                (u16)37      // UI通知底层取消发送
typedef struct {
    u32 fileNo;                     // 文件编号
}ui_clt_cancel_file_cmd;
#define EV_UI_CLT_CANCEL_ALL_FILE_CMD            (u16)38      // UI通知底层全部取消

//客户端注册与登录
#define EV_UI_CLT_REGIST_REQ                     (u16)39      // UI通知底层注册用户名
typedef struct {
	s8 userName[BUFFER_SIZE];       // 用户名
}ui_clt_regist_req;
#define EV_CLT_UI_REGIST_ACK                     (u16)40      // 底层确认注册
#define EV_CLT_UI_REGIST_NACK                    (u16)41      // 底层拒绝注册

#define EV_UI_CLT_LOG_IN_REQ                     (u16)42      // UI通知底层登录
typedef struct {
    s8 userName[BUFFER_SIZE];       // 用户名
}ui_clt_log_in_req;
#define EV_CLT_UI_LOG_IN_ACK                     (u16)43      // 底层确认登录
#define EV_CLT_UI_LOG_IN_NACK                    (u16)44      // 底层拒绝登录

#define EV_CLT_UI_CONNECT_RESULT_NTF             (u16)45      // 底层通知UI连接结果
typedef struct {
    CONNECT_RESULT result;     // 连接结果
}clt_ui_connect_result_ntf;
#define EV_CLT_UI_REGIST_LIST_NTF                (u16)46      // 底层通知UI在线列表
typedef struct {
    s32 clientNum;                  // 客户端数量
    u32 clientNo[100];              // 客户端编号
    s8 clientName[BUFFER_SIZE];     // 客户端用户名
}clt_ui_regist_list_ntf;

// 发送消息
#define EV_UI_CLT_POST_MSG_REQ                   (u16)47      // UI请求发送消息
typedef struct {
	u16 clientNum;                  // 目标客户端数量
    u32 clientNo[16];               // 目标客户端编号
	s8 msgContent[BUFFER_SIZE];     // 消息内容
}ui_clt_post_msg_req;
#define EV_CLT_UI_POST_MSG_ACK                   (u16)48      // 底层确认发送消息
#define EV_CLT_UI_POST_MSG_NACK                  (u16)49      // 底层拒绝发送消息

#define EV_CLT_UI_RECEIVE_MSG_NTF                (u16)50      // 底层通知UI收到消息
typedef struct {
    u32 srcClientNo;                // 源客户端编号
    s8 msgContent[BUFFER_SIZE];     // 消息内容
}clt_ui_receive_msg_ntf;

#define EV_CLT_UI_DISC_NTF                       (u16)51      // 底层通知UI连接断开

/***************************连接断开与定时重连***************************/
#define EV_CLT_CLT_RECONNECT_NTF                 (u16)52      // 断开时超时重新连接
#define EV_CLT_CLT_CONNECT_SUCCEED_NTF           (u16)53      // 连接成功



// 断开时重连间隔
#define RECONNECT_INTERVAL                       3000

// 最大在线客户端数
#define MAX_ONLINE_CLIENT_NUM                    100

// 客户端Instance状态定义
#define INSTANCE_STATUS_IDLE                     (u32)0       // 空闲
#define INSTANCE_STATUS_POST_FILE                (u32)1       // 发送文件
#define INSTANCE_STATUS_RECEIVE_FILE             (u32)2       // 接收文件

// Instance数量
#define FILE_APP_INSTANCE_NUMBER_CLIENT          (u16)5
#define FILE_APP_INSTANCE_NUMBER_SERVER          (u16)200
#define MSG_APP_INSTANCE_NUMBER_CLIENT           (u16)1
#define MSG_APP_INSTANCE_NUMBER_SERVER           (u16)1

// Tslnet端口
#define TELNET_PORT_CLIENT                       (u16)2500
#define TELNET_PORT_SERVER                       (u16)2500

// 本地监听端口
#define LISTEN_PORT_CLIENT                       (u16)6682
#define LISTEN_PORT_SERVER                       (u16)6682

// APP名称
#define FILE_APP_NAME_CLIENT                     "FileAppClient"
#define MSG_APP_NAME_CLIENT                      "MsgAppClient"
#define FILE_APP_NAME_SERVER                     "FileAppServer"
#define MSG_APP_NAME_SERVER                      "MsgAppClient"

// APP最大别名长度
#define MAX_ALIAS_LENGTH_CLIENT                  20
#define MAX_ALIAS_LENGTH_SERVER                  20

// APPID
#define FILE_APP_ID_CLIENT                       (u16)32
#define MSG_APP_ID_CLIENT                        (u16)33
#define FILE_APP_ID_SERVER                       (u16)34
#define MSG_APP_ID_SERVER                        (u16)35

// APP优先级
#define FILE_APP_PRIORITY_CLIENT                 (u8)100
#define FILE_APP_PRIORITY_SERVER                 (u8)100
#define MSG_APP_PRIORITY_CLIENT                  (u8)100
#define MSG_APP_PRIORITY_SERVER                  (u8)100

// 消息队列中最大等待消息数
#define FILE_APP_QUEUE_SIZE_CLIENT               1000
#define FILE_APP_QUEUE_SIZE_SERVER               1000
#define MSG_APP_QUEUE_SIZE_CLIENT                1000
#define MSG_APP_QUEUE_SIZE_SERVER                1000

// 服务端IP地址
#define IPV4_ADDRESS_SERVER                      ((u32)inet_addr( "192.168.126.128" ))
