OspPost
OspNodeIpGet
OspCreateTcpNode
OspConnectTcpNode
OspSetHBParam     // 设置结点的断链检测参数
OspNodeDiscCBRegQ  // 增加在node连接中断时需通知的appid和InstId
IsOspInitd
OspInit
OspQuit   // 推出OSP，调用所有APP的所有Instance的InstanceExit方法，断开OSP连接
OspSetPrompt  // 设置OSP上Tslnet的前缀提示符
OspRegistModule  // （Windows平台下有效)注册模块、dll文件
SetTimer  // 设置相对计时器
KillTimer  // 删除相对计时器
OspSemBCreate
OspSemDelete
OspSemTake  // 信号量p操作，获取信号量
OspSemGive  // 信号量x操作，释放信号量

OspAppShow
OspInstShow
OspNodeShow
OspEventDescShow
OspSetLogLevel
OspTaskShow

* 什么是一个APP？为什么要设计不同的APP？
APP是OSP通讯体系中最小的调度单位，每个APP对应一个功能模块，多个APP可以协同工作完成一组功能、服务，OSP会为每个APP创建一个消息处理任务，该任务中只有一个消息处理逻辑
* 什么是一个Instance?
Instance是OSP通讯体系中最小的消息处理单位，一个APP可以包含多个Instance，他们使用相同的消息处理逻辑，但每个Instance的状态可以不同
* 同一个APP的不同Instance之间的消息会相互影响吗？
* 什么是OSP的一个Node?
一个Node连接对应一个TCP连接；每个应用程序都可以创建一个本地监听节点，用与监听外部连接，本地监听节点没有对应节点号；本地节点用于本地通讯，有固定节点号0；外部节点用于与外部应用程序通讯，外部节点可以通过ConnectTcpNode连接目的应用程序，返回对端通讯节点
* OSP的Deamon实例的设计目的和应用场合是什么？
一个APP有多个Instance，当用户无法指定使用哪个Instance处理消息的时候，可以指定DaemonInstance作为最后的选择；DaemonInstance可以作为特殊Instance，用来管理其他Instance
* 为什么要封装OSP？目的是什么？
1、实现跨平台的开发，实现不同平台下应用程序之间的消息通讯；2、TCP属于无边界字节流协议，用户每次发送消息，未必能发送一条完整的消息，而是对裸字节流进行拆分组合，通过OSP，用户就能发送一条完整的消息
* APP线程与UI线程实现数据共享、传递的做法有哪些？
UI线程可以通过OspPost向本地节点发送数据，APP可以通过Qt的信号与槽机制向UI进程发送信号；APP可以创建在全局变量区从而实现数据共享