# OspDemo 概要设计
--------


[TOC]

## 需求

* 实现客户端文件上传功能
* 支持客户端同时上传多个文件
* 支持断点续传(客户端程序崩溃重启, 客户端和服务端断链,服务端崩溃重启)
* 上传文件完整性验证
* 定义统一的消息体，可以支持文件和短消息传输
* 支持在线客户端列表的获取和实时刷新
* 支持客户端之间的消息转发
* 支持一到多的客户端消息聊天、文件传输

## 相关文档
* 《C++编程规范V1.5.pdf》
* 《科达工程创建规范.pdf》
* 《OSP 用户手册_v1.8.pdf》

## 设计原则
* 事件驱动、跨平台、面向对象、模块化设计

## 体系架构
### Service
```mermaid
graph BT
    CApp(CApp)
    CInstance(CInstance)
    DemoAppData(DemoAppData)
    DemoInstance(DemoInstance)
    DemoDaemonInstance(DemoDaemonInstance)
    DemoApp(DemoApp)
    DemoInstance --Inherit--> CInstance
    DemoDaemonInstance --Inherit--> CInstance
    DemoApp --Inherit--> DemoAppData
    DemoApp -.Contain.-> DemoDaemonInstance
    DemoApp -.Contain.-> DemoInstance
    DemoApp --Inherit--> CApp
```
### UI
```mermaid
graph BT
    QLabel(QLabel)
    QLineEdit(QLineEdit)
    QWidget(QWidget)
    QPushButton(QPushButton)
    ...(...)
    MainWindow(MainWindow)
    MainWindow -.Contain.-> QLabel
    MainWindow -.Contain.-> QLineEdit
    MainWindow --Inherit--> QWidget
    MainWindow -.Contain.-> QPushButton
    MainWindow -.Contain.-> ...
```
### Instance状态
```C++
#define INSTANCE_STATUS_IDLE             u32(0)    // 空闲
#define INSTANCE_STATUS_POST_FILE        u32(1)    // 发送文件
#define INSTANCE_STATUS_RECEIVE_FILE     u32(2)    // 接收文件
#define INSTANCE_STATUS_WAIT_FILE        u32(3)    // 一对多文件传输时，其他Instance等待文件上传完毕
#define INSTANCE_STATUS_POST_MESSAGE     u32(4)    // 发送Message
```

## 时序图
### UI端与Service数据交互
```mermaid
sequenceDiagram
    participant Service
    participant UI
    UI ->> Service: OspPost向本地发送数据
    UI ->> + Service: 设置回调函数
    Service ->> UI:调用函数，在回调函数中emit signal
    deactivate SignalInstance
```
* UI侧通过OspPost接口发送消息给Service
* Service向UI发送数据，需要UI侧先设置回调函数，Service侧调用函数，并在函数内emit signal


### 文件上传
```mermaid
sequenceDiagram
    participant ClientUI
    participant ClientService
    ClientUI->>ClientService: 通知Service发送文件
    ClientService ->> Server: 请求上传文件
    alt 没有空闲Instance
        Server ->> + ClientService: 拒绝上传
        ClientService ->> - ClientUI:服务器繁忙
    else 有空闲Instance
        Server ->> Server: 更新Instance状态
        Server ->> ClientService: 分配空闲Instance
        ClientService->>Server: 发送第一条数据
    loop transfer
        Note over Server: 存储文件
        Server ->> + ClientService: 确认接收数据包
        ClientService ->> ClientUI:通知进度
        ClientService ->> - Server: 发送数据包
    end
        ClientService ->> + Server: 文件发送完毕
        Server ->> - ClientService: 回复确认
    end
```
* 客户端每次最多可同时发送5个文件
* UI侧通知Service开始传输文件
* Service侧发送request到Server
* Server根据实际情况选择分配空闲的Instance或拒绝文件上传
* Service侧获得Instance号开始循环发送数据包
* 服务端对每一条消息都要验证进度，并回复当前已记录的文件进度
* 客户端发送的每一条消息，都需要服务端确认后才会继续发送下一条，否则等待，直至超时重传
* 客户端文件发送完毕，服务端回复确认
* 消息体结构：
``` C++
typedef struct MsgSendFileReq {
	 u8 msgType;      // 消息类型
	 u8 num;          // 文件数量
	 s8 buf[];        // 文件名，以'\0'分隔
}MsgSendFileReq;

typedef struct MsgSendFile {
	 u8 msgType;      // 消息类型
	 u8 num;          // 当前文件编号
	 s32 progress;    // 文件进度
	 s8 buf[];        // 文件内容
}MsgSendFile;

typedef struct MsgFileConfirm {
	 u8 msgType;      // 消息类型
	 u8 num;          // 当前文件编号
	 s32 progress;    // 确认收到的文件进度
}MsgFileConfirm;
```
### 一对多消息发送
```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant Client...
    Client ->> + Server: 发送消息
    Server ->> - Client: 确认接收
    Server ->> Server:保存所有目标客户端
    Server ->> Client...: 发送消息给所有目标客户端
    alt 确认接收的客户端
        Client... ->> Server: 标记发送完成
    else 未收到确认消息的客户端
        Server ->> Client...: 超时重新发送
    end
    Server ->> Server:所有目标客户端都确认接收，结束
```
* 客户端一次最多给另外8个客户端发送消息
* 客户端或服务器发送的每一条消息都要确认回复，否则超时重传
* 消息体结构：
``` C++
typedef struct MsgMessage {
	 u8 msgType;        // 消息类型:短消息传输
	 u8 num;            // 目标客户端数量
	 u16 clientNum[8];  // 目标APPID
	 s8 buf[];          // 消息内容
}MsgMessage;
```

### 一对多文件发送
```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant OtherClients
    Client ->> Server: post file req
    alt 服务端空闲Instance不足
        Server ->> Client: 拒绝接收
    else 分配空闲Instance
        Server ->> Client: 回复确认
    end
    Client ->> Server: 循环发送文件
    Server ->> Server: 为每个Instance创建任务
    Server ->> OtherClients: 循环发送文件
```
* 一对多的文件发送方式全部采用服务器中转的发送方式，且每次只支持发送一个文件
* 可以最多指定8个目标客户端
* 客户端请求发送文件后，服务端为每个目标客户端分配一个Instance，如果资源不足，将拒绝发送请求
* 服务端收到文件后，为每个Instance创建任务并向目标客户端发送文件
* 消息体结构：
``` C++
typedef struct MsgSendFileMultiClientReq {
	 u8 msgType;        // 消息类型:多客户端文件传输
	 u8 num;            // 目标客户端数量
	 u16 clientNum[8];  // 目标客户端APPID
	 s8 buf[];          // 文件名
}MsgSendFileMultiClientReq;
```

### 在线客户端列表的获取和实时刷新
```mermaid
sequenceDiagram
    participant Client
    participant Server
    participant AllClients
    Client ->> + Server: 发送注册消息
    Server ->> Server: 更新在线服务端列表，设置断链机制
    Server ->> - AllClients: 通知所有在线客户端
    Client ->> + Server: 客户端关闭或超时断链
    Server ->> Server: 更新在线服务端列表
    Server ->> - AllClients: 通知所有在线客户端
```
* 服务端或服务器发送的每一条消息都要确认回复，否则超时重传
* 消息体结构：
``` C++
typedef struct MsgClientRegist {
	 u8 msgType;        // 消息类型:在线客户端注册
	 u8 num;            // 在线客户端数量
	 s8 buf[];          // 客户端用户名
}MsgClientRegist;
```

## 异常处理
### 超时断开链接
```mermaid
sequenceDiagram
    participant Client
    participant Server
    Server->>Server:超时断链, 记录当前文件发送进度
    Client ->> Server: 恢复链接，发送注册消息
    Server ->> Client: 通知进度，要求重传
    loop 传输文件
        Client->>Server: post file notify
    Note over Server: receive file
    end
    Client->>Server: Post file complete
    Server->>Server: close file
```
* 服务端超时断开链接，记录下文件传输进度
* 客户端重新连接，发送注册消息
* 服务端检索本地是否有未完成的任务，若有则回复文件传输进度
* 客户端继续发送文件
* 消息体结构：
``` C++
typedef struct MsgSendFileReconnect {
	 u8 msgType;      // 消息类型：断链重连继续发送文件
	 u8 num;          // 文件数量
	 s32 buf[];       // 文件进度
}MsgSendFileReconnect;

typedef struct MsgSendFile {
	 u8 msgType;      // 消息类型
	 u8 num;          // 当前文件编号
	 s32 progress;    // 文件进度
	 s8 buf[];        // 文件内容
}MsgSendFile;
```

### Client异常崩溃重启
```mermaid
sequenceDiagram
    participant Client
    participant Server
    Client->>Client:客户端崩溃
    Client ->> Server: 恢复链接，发送注册消息
    alt 服务端已检测到断链
        Server ->> Client: 读取本地缓存，检测到未完成的任务，通知任务进度
    else 服务端尚未检测到断链
        Server ->> Client: 通知进度，要求继续传输
    end
    loop 传输文件
        Client->>Server: post file notify
    Note over Server: receive file
    end
    Client->>Server: Post file complete
    Server->>Server: close file
```
* 客户端在发送文件前，会保存文件路径和文件编号信息
* 客户端崩溃重启，发送客户端注册请求
* 服务端收到请求，回复文件传输进度
* 客户端继续发送文件

### Server异常崩溃
```mermaid
sequenceDiagram
    participant Client
    participant Server
    Server->>Server: 服务端重启
    Client ->> Server: 恢复链接，发送注册消息
    alt 客户端已检测到断链
        Client ->> Server: 恢复链接，发送注册消息
        Server ->> Client: 读取本地缓存，恢复Instance状态，通知进度
    else 服务端尚未检测到断链
        Client ->> Server: 继续发送文件
        Server ->> Client: 状态异常，读取本地缓存，恢复Instance状态，通知进度
    end
    loop 传输文件
        Client->>Server: post file notify
    Note over Server: receive file
    end
    Client->>Server: Post file complete
    Server->>Server: close file
```
* 客户端在发送文件前，会保存文件路径和文件编号信息
* 服务端每次收到文件后，会保存文件进度，客户端编号，当前状态，当前任务等信息
* 客户端检测到断链后，会不断尝试连接
* 客户端连接成功，服务端搜索该客户端是否有未完成的任务，若有则继续
* 若客户端未检测到断链，继续文件传输，服务端会检测到Instance状态异常，会读取本地缓存，恢复Instance状态



