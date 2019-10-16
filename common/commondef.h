#pragma once
#include "../common/kdvtype.h"

// ��������С
#define BUFFER_SIZE                        (u16)(50<<10)

/***************************�����ļ�***************************/
#define EV_CLT_SER_START_POST_FILE_REQ           (u16)1       // �ͻ����������ļ�
typedef struct {
	char fileName[BUFFER_SIZE];     // �ļ���
}clt_ser_start_post_file_req;
#define EV_SER_CLT_START_POST_FILE_ACK           (u16)2       // �����ȷ�Ͽ�ʼ�����ļ�
typedef struct {
	u8 InstanceNo;                  // �����Instance��
}ser_clt_start_post_file_ack;
#define EV_SER_CLT_START_POST_FILE_NACK          (u16)3       // �������Դ����

#define EV_CLT_SER_POST_FILE_REQ                 (u16)4       // �ͻ��˷����ļ��������
typedef struct {
	u32 length;                     // ���ݳ���
	s8 fileContent[BUFFER_SIZE];    // �ļ�����
}clt_ser_post_file_ntf;
#define EV_SER_CLT_POST_FILE_ACK                 (u16)5       // �����ȷ�Ͻ����ļ�����

#define EV_CLT_SER_FILE_MD5_REQ                  (u16)6       // �ͻ����ļ�������ϣ�����md5У����
typedef struct {
	s8 md5[33];                     // �ļ�md5У����
}clt_ser_file_md5_req;

#define EV_SER_CLT_FILE_MD5_ACK                  (u16)7       // ������ļ�У�����
#define EV_SER_CLT_FILE_MD5_NACK                 (u16)8       // ������ļ�У��ʧ��


#define EV_CLT_SER_PAUSE_FILE_REQ                (u16)9       // �ͻ���������ͣ
#define EV_SER_CLT_PAUSE_FILE_ACK                (u16)10      // �����ȷ����ͣ

#define EV_CLT_SER_CONTINUE_POST_FILE_REQ        (u16)11      // �ͻ���������������ļ�
typedef struct {
	char fileName[BUFFER_SIZE];     // �ļ���
}clt_ser_continue_post_file_req;
#define EV_SER_CLT_CONTINUE_POST_FILE_ACK        (u16)12      // �����ȷ�ϼ��������ļ�
typedef struct {
	u8 InstanceNo;                  // �����Instance��
	u32 progress;                   // �ļ�����
}ser_clt_continue_post_file_ack;
#define EV_SER_CLT_CONTINUE_POST_FILE_NACK       (u16)13      // ����˾ܾ���������

#define EV_CLT_SER_CANCEL_FILE_REQ               (u16)14      // �ͻ�������ȡ�������е��ļ�
#define EV_SER_CLT_CANCEL_FILE_ACK               (u16)15      // �����ȷ��ȡ������
#define EV_CLT_SER_CANCEL_FILE_NTF               (u16)16      // �ͻ���֪ͨȡ��������ͣ�е��ļ�
typedef struct{
	char fileName[BUFFER_SIZE];     // �ļ���
}clt_ser_cancel_file_ntf;


/******************************���߿ͻ����б�************************************/
enum CONNECT_RESULT {              // ���ӽ��
    CONNECTION_SUCCEED,
    CLIENT_NUMBER_LIMIT,
    OCCUPIED_USERNAME
};
#define EV_CLT_SER_REGIST_REQ                    (u16)17      // �ͻ��˷���ע������
typedef struct {
	s8 userName[BUFFER_SIZE];      // �û���
}clt_ser_regist_req;
#define EV_SER_CLT_REGIST_ACK                    (u16)18      // �����ȷ��ע��
#define EV_SER_CLT_REGIST_NACK                   (u16)19      // ����˾ܾ�ע��
typedef struct {
    CONNECT_RESULT result;         // ע����
}ser_clt_regist_nack;

#define EV_CLT_SER_LOG_IN_REQ                    (u16)20      // �ͻ��˷��͵�¼����
typedef struct {
    s8 userName[BUFFER_SIZE];      // �û���
}clt_ser_log_in_req;
#define EV_SER_CLT_LOG_IN_ACK                    (u16)21      // �����ȷ�ϵ�¼
#define EV_SER_CLT_LOG_IN_NACK                   (u16)22      // ����˾ܾ���¼
typedef struct {
    CONNECT_RESULT result;         // ���ӽ��
}ser_clt_log_in_nack;

#define EV_SER_CLT_REGIST_LIST_NTF               (u16)23      // �����֪ͨ�ͻ��������б�
typedef struct {
	s32 clientNum;                  // �ͻ�������
	u32 clientNo[100];              // �ͻ��˱��
	s8 clientName[BUFFER_SIZE];     // �ͻ����û���
}ser_clt_regist_list_ntf;


/*********************************������Ϣ************************************/
#define EV_CLT_SER_POST_MSG_NTF                  (u16)24      // �ͻ���֪ͨ����˷�����Ϣ
typedef struct {
	u16 clientNum;                  // Ŀ��ͻ�������
	u32 clientNo[16];               // Ŀ��ͻ��˱��
	s8 msgContent[BUFFER_SIZE];     // ��Ϣ����
}clt_ser_post_msg_ntf;

#define EV_SER_CLT_POST_MSG_NTF                  (u16)25      // �������ͻ��˷�����Ϣ
typedef struct {
	u32 srcClientNo;                // Դ�ͻ��˱��
	s8 msgContent[BUFFER_SIZE];     // ��Ϣ����
}ser_clt_post_msg_ntf;


/***************************UI��ײ�ӿ�***************************/
#define EV_UI_CLT_POST_FILE_REQ                  (u16)26      // UI����ʼ�����ļ�
typedef struct {
	u16 fileNum;                    // �ļ�����
	s8 fileName[BUFFER_SIZE];       // �ļ���
}ui_clt_post_file_req;
#define EV_CLT_UI_POST_FILE_ACK                  (u16)27      // �ײ�ȷ�Ϸ����ļ�
#define EV_CLT_UI_POST_FILE_NACK                 (u16)28      // ���������ߣ��ײ�ܾ������ļ�

#define EV_CLT_UI_POST_FILE_PROGRESS_NTF         (u16)29      // �ײ�֪ͨUI�ļ�����
typedef struct {
	u32 fileNo;                     // �ļ����
	u32 progress;                   // �ļ�����
}ui_clt_post_file_progress_ntf;
#define EV_CLT_UI_POST_FILE_FAIL_NTF             (u16)30      // �ײ�֪ͨUI�������Դ����
typedef struct {
    u32 fileNo;                     // �ļ����
}clt_ui_post_file_fail_ntf;

#define EV_CLT_UI_RESEND_FILE_NTF                (u16)31      // �ײ�֪ͨUIУ��ʧ�ܣ����·����ļ�
typedef struct {
	u32 fileNo;                     // �ļ����
}clt_ui_resend_file_ntf;
#define EV_CLT_UI_POST_FILE_COMPLETE_NTF         (u16)32      // �ײ�֪ͨUI�ļ����ͳɹ�
typedef struct {
	u32 fileNo;                     // �ļ����
}clt_ui_post_file_complete_ntf;


#define EV_UI_CLT_PAUSE_FILE_CMD                 (u16)33      // UI֪ͨ�ײ���ͣ����
typedef struct {
    u32 fileNo;                     // �ļ����
}ui_clt_pause_file_cmd;
#define EV_UI_CLT_PAUSE_ALL_FILE_CMD             (u16)34      // UI֪ͨ�ײ�ȫ����ͣ

#define EV_UI_CLT_CONTINUE_FILE_CMD              (u16)35      // UI֪ͨ�ײ��������
typedef struct {
    u32 fileNo;                     // �ļ����
}ui_clt_continue_file_cmd;
#define EV_UI_CLT_CONTINUE_ALL_FILE_CMD          (u16)36      // UI֪ͨ�ײ�ȫ������

#define EV_UI_CLT_CANCEL_FILE_CMD                (u16)37      // UI֪ͨ�ײ�ȡ������
typedef struct {
    u32 fileNo;                     // �ļ����
}ui_clt_cancel_file_cmd;
#define EV_UI_CLT_CANCEL_ALL_FILE_CMD            (u16)38      // UI֪ͨ�ײ�ȫ��ȡ��

//�ͻ���ע�����¼
#define EV_UI_CLT_REGIST_REQ                     (u16)39      // UI֪ͨ�ײ�ע���û���
typedef struct {
	s8 userName[BUFFER_SIZE];       // �û���
}ui_clt_regist_req;
#define EV_CLT_UI_REGIST_ACK                     (u16)40      // �ײ�ȷ��ע��
#define EV_CLT_UI_REGIST_NACK                    (u16)41      // �ײ�ܾ�ע��

#define EV_UI_CLT_LOG_IN_REQ                     (u16)42      // UI֪ͨ�ײ��¼
typedef struct {
    s8 userName[BUFFER_SIZE];       // �û���
}ui_clt_log_in_req;
#define EV_CLT_UI_LOG_IN_ACK                     (u16)43      // �ײ�ȷ�ϵ�¼
#define EV_CLT_UI_LOG_IN_NACK                    (u16)44      // �ײ�ܾ���¼

#define EV_CLT_UI_CONNECT_RESULT_NTF             (u16)45      // �ײ�֪ͨUI���ӽ��
typedef struct {
    CONNECT_RESULT result;     // ���ӽ��
}clt_ui_connect_result_ntf;
#define EV_CLT_UI_REGIST_LIST_NTF                (u16)46      // �ײ�֪ͨUI�����б�
typedef struct {
    s32 clientNum;                  // �ͻ�������
    u32 clientNo[100];              // �ͻ��˱��
    s8 clientName[BUFFER_SIZE];     // �ͻ����û���
}clt_ui_regist_list_ntf;

// ������Ϣ
#define EV_UI_CLT_POST_MSG_REQ                   (u16)47      // UI��������Ϣ
typedef struct {
	u16 clientNum;                  // Ŀ��ͻ�������
    u32 clientNo[16];               // Ŀ��ͻ��˱��
	s8 msgContent[BUFFER_SIZE];     // ��Ϣ����
}ui_clt_post_msg_req;
#define EV_CLT_UI_POST_MSG_ACK                   (u16)48      // �ײ�ȷ�Ϸ�����Ϣ
#define EV_CLT_UI_POST_MSG_NACK                  (u16)49      // �ײ�ܾ�������Ϣ

#define EV_CLT_UI_RECEIVE_MSG_NTF                (u16)50      // �ײ�֪ͨUI�յ���Ϣ
typedef struct {
    u32 srcClientNo;                // Դ�ͻ��˱��
    s8 msgContent[BUFFER_SIZE];     // ��Ϣ����
}clt_ui_receive_msg_ntf;

#define EV_CLT_UI_DISC_NTF                       (u16)51      // �ײ�֪ͨUI���ӶϿ�

/***************************���ӶϿ��붨ʱ����***************************/
#define EV_CLT_CLT_RECONNECT_NTF                 (u16)52      // �Ͽ�ʱ��ʱ��������
#define EV_CLT_CLT_CONNECT_SUCCEED_NTF           (u16)53      // ���ӳɹ�



// �Ͽ�ʱ�������
#define RECONNECT_INTERVAL                       3000

// ������߿ͻ�����
#define MAX_ONLINE_CLIENT_NUM                    100

// �ͻ���Instance״̬����
#define INSTANCE_STATUS_IDLE                     (u32)0       // ����
#define INSTANCE_STATUS_POST_FILE                (u32)1       // �����ļ�
#define INSTANCE_STATUS_RECEIVE_FILE             (u32)2       // �����ļ�

// Instance����
#define FILE_APP_INSTANCE_NUMBER_CLIENT          (u16)5
#define FILE_APP_INSTANCE_NUMBER_SERVER          (u16)200
#define MSG_APP_INSTANCE_NUMBER_CLIENT           (u16)1
#define MSG_APP_INSTANCE_NUMBER_SERVER           (u16)1

// Tslnet�˿�
#define TELNET_PORT_CLIENT                       (u16)2500
#define TELNET_PORT_SERVER                       (u16)2500

// ���ؼ����˿�
#define LISTEN_PORT_CLIENT                       (u16)6682
#define LISTEN_PORT_SERVER                       (u16)6682

// APP����
#define FILE_APP_NAME_CLIENT                     "FileAppClient"
#define MSG_APP_NAME_CLIENT                      "MsgAppClient"
#define FILE_APP_NAME_SERVER                     "FileAppServer"
#define MSG_APP_NAME_SERVER                      "MsgAppClient"

// APP����������
#define MAX_ALIAS_LENGTH_CLIENT                  20
#define MAX_ALIAS_LENGTH_SERVER                  20

// APPID
#define FILE_APP_ID_CLIENT                       (u16)32
#define MSG_APP_ID_CLIENT                        (u16)33
#define FILE_APP_ID_SERVER                       (u16)34
#define MSG_APP_ID_SERVER                        (u16)35

// APP���ȼ�
#define FILE_APP_PRIORITY_CLIENT                 (u8)100
#define FILE_APP_PRIORITY_SERVER                 (u8)100
#define MSG_APP_PRIORITY_CLIENT                  (u8)100
#define MSG_APP_PRIORITY_SERVER                  (u8)100

// ��Ϣ���������ȴ���Ϣ��
#define FILE_APP_QUEUE_SIZE_CLIENT               1000
#define FILE_APP_QUEUE_SIZE_SERVER               1000
#define MSG_APP_QUEUE_SIZE_CLIENT                1000
#define MSG_APP_QUEUE_SIZE_SERVER                1000

// �����IP��ַ
#define IPV4_ADDRESS_SERVER                      ((u32)inet_addr( "192.168.126.128" ))
