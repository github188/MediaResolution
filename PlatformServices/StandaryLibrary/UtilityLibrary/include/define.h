#ifndef DEFINE_H
#define DEFINE_H

//--------------------------------定义设备类型---------------------------------

#define TYPE_USER				1	// 用户
#define TYPE_TERMINAL			2	// 前端
#define TYPE_DISPATCH			3	// 分发
#define TYPE_STORE				4	// 存储
#define TYPE_CAMERA				5	// 摄像头
#define TYPE_ALARM				6	// 报警器
#define TYPE_DISPLAY			7	// 显示墙
#define TYPE_ALARMOUT			8	// 报警输出

#define MEGAEYES_VERSION 257  //协议版本号

//--------------------------------定义错误编号---------------------------------

#define ERR_LACKMEMORY			        301		// 内存不足
#define ERR_UNKNOWNUSER			        302		// 未知客户端
#define ERR_MSGFORMAT			        303		// 指令格式错误
#define ERR_RETFORMAT			        304		// 返回结果格式错误
#define ERR_AUTHRET                     305		// 验证权限返回结果错误
#define ERR_CONNECT                     306		// 通讯连接错误
#define ERR_SEND                        307		// 通讯发送错误
#define ERR_RECV                        308		// 通讯接收错误
#define ERR_VERSION                       309		// 协议头版本不对
#define ERR_RECV_CLOSE			        310		// 通讯接收错误,对方关闭
#define ERR_RECV_SOCK			        311		// 通讯接收错误，socket无效
#define ERR_GETIP       		        312		// 得到设备 IP 地址失败
#define ERR_OPERSUCCESS			        313		// 服务端返回操作不成功
#define ERR_OPERSUCCESS_ONE             314     // 服务端返回操作不成功
#define ERR_OPERSUCCESS_TWO             315     // 服务端返回操作不成功
#define ERR_RECV_LENERR	                316		// 预分配长度错误
#define ERR_INVALID_MESGID              317     // 错误的命令id             
#define ERR_SERVICENAME                 318     // 错误的中心服务名称
#define ERR_CURL_INIT                   319     //初始化curl失败
#define ERR_CURL_HTTP                   320     //请求http出现网络错误
#define ERR_UNKNOWNDISPLAY              321     //未知显示墙
#define ERR_NULL_USERNAME               322     //用户名为空
#define ERR_OUTRANGE_USERNAME           323     //用户名太长
#define ERR_NULL_MAC_ADDR               324     //MAC地址为空
#define ERR_FORMAT_MAC_ADDR             325     //MAC地址格式错误

#define ERR_NOTEXISTSHISTORY            400		// 不存在历史视频
#define ERR_PTZLOCKED			        401		// 资源已经被他人锁定
#define ERR_TERMREGISTER		        402		// 前端注册失败
#define ERR_DISPATCHREGISTER            403		// 分发注册失败
#define ERR_HDLREGISTER			        404		// 处理函数注册失败
#define ERR_NOTEXISTSDEV		        405		// 不存在该设备
#define ERR_REACHLIMITATION		        406		// 视频连接已达到最大数量
#define ERR_LOGINLIMITATION		        407		// 用户登录超时
#define ERR_NOTEXISTSCTRLPTZ            408		// 不存在该云台的控制
#define ERR_MUTEX                       409		// 锁定表失败
#define ERR_NOTEXISTSVIDEOSVR           410		// 不存在该视频服务
#define ERR_REALTIMESVRRET              411		// 请求实时视频时前端/分发返回信息错误
#define ERR_STOPALARMEVENTMSG           412		// 请求停止报警的用户指令格式错误
#define ERR_DISPREGJ2EERET		        413		// 分发注册时从 J2EE 返回错误
#define ERR_J2EESENDSESSIONID           414		// J2EE 在用户登录时发送用户会话 ID 格式错误
#define ERR_COMMUNICATIONTOJ2EE         415		// 同 J2EE 通讯发生错误（通用接口）
#define ERR_VIDEOBEGIN_END		        416		// 视频开始/停止指令格式错误
#define ERR_TERMREGJ2EERET		        417		// 前端注册时 J2EE 返回错误
#define ERR_TERMALARMBEGIN		        418		// 前端报警开始时指令格式错误
#define ERR_TERMALARMBEGINJ2EERET       419		// 前端报警开始时 J2EE 返回错误
#define ERR_TERMALARMEND		        420		// 前端报警结束时指令格式错误
#define ERR_REALTIMELOCKMEMORY		    421		// 请求实时视频时视频信息内存池不够
#define ERR_NOTEXISTSALARMEVENT		    422		// 报警事件结束时不存在报警事件
#define ERR_STOREREGJ2EERET		        423		// 存储注册失败
#define ERR_FTPINFO			            424		// 获取ftp信息出错
#define ERR_STOREREGISTER               425     // 存储注册失败
#define ERR_INVALID_USERID              426     // 无效的用户id
#define ERR_USER_STATUS_NULL            427     // 用户状态为空
#define ERR_INVALID_NAME                428     // naming为空  
#define ERR_UNKOWN_NODETYPE             429
#define ERR_NOEXITCACHE                 430
#define ERR_REACHSOFTLIMITATION		    431		// 视频连接已达到最大数量
#define ERR_INVALID_XML					432			// 无效的XML
#define ERR_CENTER_NOT_RESP			433       // 中心无应答
#define ERR_CENTER_RESP_XML_ERR   434		// 中心应答xml格式错误
#define ERR_INVALID_STRORAGE_ID	435	   // 无效的中心存储业务ID号

#define ERR_NO_HANDLER                  438
#define ERR_TIMEOUT                     440     //处理超时
#define ERR_ALARM_TIMEOUT               441     //报警屏蔽时长内的报?
#define ERR_REMOTEIP                    460     //设备Naming中的远端IP不能是127.0.0.1

#define ERR_NOACCESS_DISPLAY	        500		//没有权限操作显示墙
#define ERR_NOACCESS_REALTIME	        501		//没有权限操作实时视频
#define ERR_NOACCESS_HISTORY	        502		//没有权限操作历史视频
#define ERR_SERNOBOOT			        503		//服务没有启动

#define ERR_CONNECT_FAULT		        510		// 通讯连接故障
#define ERR_DEVLIST_EMPTY               511 
#define ERR_NO_VIDEO_ROUTE              512     //没有发现视频路由
#define ERR_VIDEO_ROUTE_FAULT           513     //视频路由有故障
#define ERR_NULL_NODEID                 514

#define ERR_WOULDBLOCK                  515 
#define ERR_PERSISTENCE_CONN            516 

#define ERR_USERDONTREF_PRIVATERELAY    520     //没有指定分发
#define ERR_USERDONTUSE_PRIVATERELAY    521     //没有使用分发
#define ERR_USERERRREF_PRIVATERELAY     522     //指定的分发不存在
#define ERR_NOUSE_DISPATCH			    523	    // 没有可用的分发

#define ERR_CLUSTER_CONN                600     // 与主节点通信失败
#define ERR_CLUSTER_TRA                 601     // 主节点处理的默认错误号
#define ERR_CLUSTER_TRA_INVALID_MESGID  602
#define ERR_CLUSTER_TRA_SAMENODE        603     // 设备所在节点和发送请求的节点是相同的

#define ERR_NOTEXISTSUSER		        701		// 不存在该用户 add by ydx
#define ERR_GETDISPATCHIP   	        702		// 获取分发IP失败
#define ERR_INVALID_STOREID             703     // 无效的存储id

#define ERR_NOT_FOUND_XML		800		//无效的XML

#define ERR_UNKONWN_MSGID               80000001    //不识别的指令
#define ERR_DISUSE_MSGID                80000002    //废弃的指令

//--------------------------------指令编号定义---------------------------------
#define CMD_MEGA_SERVER_REG_REQ         5002        //注册服务器(mega eyes)
#define CMD_MEGA_SERVER_REG_REP         5003

#define CMD_MSG_SERVER_REG_REQ          150012      //注册消息服务器(message private private)
#define CMD_MSG_SERVER_REG_REP          150013

#define CMD_MSG_SERVER_HEARTBEAT_REQ    150014      //消息服务器心跳
#define CMD_MSG_SERVER_HEARTBEAT_REP    150015

#define CMD_DOOR_STATUS_REPORT_REQ      20010       //门状态上报
#define CMD_DOOR_STATUS_REPORT_REP      20011

#define CMD_SWIPING_CARD_REPORT_REQ     20006       //刷卡事件上报
#define CMD_SWIPING_CARD_REPORT_REP     20007

#define CMD_AUTH_REPORT_REQ             150016      //认证事件上报
#define CMD_AUTH_REPORT_REP             150017

#define CMD_FINGERPRINT_REPORT_REQ      150018      //指纹事件上报
#define CMD_FINGERPRINT_REPORT_REP      150019

#define CMD_BW_UNREGISTER_REQ           190002      //注销带宽(bandwidth)服务
#define CMD_BW_UNREGISTER_REP           190003

#define CMD_BW_UPLOAD_PAYLOAD_REQ       190004      //上报负载
#define CMD_BW_UPLOAD_PAYLOAD_REP       190005 

#define CMD_BW_VIDEO_VALIDATE_REQ       190006      //视频请求验证
#define CMD_BW_VIDEO_VALIDATE_REP       190007

#define CMD_BW_UPDATE_COUNT_REQ         190008      //更新带宽占用计数
#define CMD_BW_UPDATE_COUNT_REP         190009

#define CMD_BW_GET_BANK_INFO_REQ        190010      //获取网点状态信息
#define CMD_BW_GET_BANK_INFO_REP        190011

#define CMD_UNHANDLE_ALARM_REQ          190014      //透传未处理报警
#define CMD_UNHANDLE_ALARM_REP          190015 

#define CMD_ISSUED_PUSH_MESSAGE_REQ     190016      
#define CMD_ISSUED_PUSH_MESSAGE_REP     190017      //服务器下发消息 

#define CMD_VIDEO_DISCONNECT_REQ        110004      //视频连接断开
#define CMD_VIDEO_DISCONNECT_REP        110005

#define CMD_DOOR_SERVER_REG_REQ         20000       //门禁网关注册
#define CMD_DOOR_SERVER_REG_REP         20001

#define CMD_DOOR_MULTI_TIMEOUT_REQ      20004       //设置门禁多卡刷卡超时时间
#define CMD_DOOR_MULTI_TIMEOUT_REP      20005   

#define CMD_DOOR_REMOTE_OPEN_REQ        20008       //远程开门
#define CMD_DOOR_REMOTE_OPEN_REP        20009

#define CMD_DOOR_OFFLINELOG_REPORT_REQ  20012       //离线刷卡日志上报
#define CMD_DOOR_OFFLINELOG_REPORT_REP  20013

#define CMD_DOOR_SET_EXTRA_CARD_REQ     20014       //设置巡更/胁迫卡
#define CMD_DOOR_SET_EXTRA_CARD_REP     20015 

#define CMD_DOOR_DEL_EXTRA_CARD_REQ     20016       //清除巡更/胁迫卡
#define CMD_DOOR_DEL_EXTRA_CARD_REP     20017

#define CMD_DOOR_STRATEGY_ISSUED_REQ    20018       //策略下发
#define CMD_DOOR_STRATEGY_ISSUED_REP    20019 

#define CMD_DOOR_TIME_RANGE_ISSUED_REQ  20024       //门禁刷卡时间段策略下发
#define CMD_DOOR_TIME_RANGE_ISSUED_REP  20025

#define CMD_DOOR_CARD_ID_ISSUED_REQ     20020       //门禁卡号下发
#define CMD_DOOR_CARD_ID_ISSUED_REP     20021

#define CMD_DOOR_DEL_CARD_ID_REQ        20022       //删除门禁卡号
#define CMD_DOOR_DEL_CARD_ID_REP        20023

#define CMD_DOOR_DEL_TIME_RANGE_REQ     20026       //删除时间段策略
#define CMD_DOOR_DEL_TIME_RANGE_REP     20027

//--------------------------------公用指令---------------------------------
#define MSG_HEARTBEAT			        2		// 心跳
#define MSG_HEARTBEAT_RESP     3     //心跳回复
#define MSG_HEARTBEAT_TICKET		4 
#define MSG_HEARTBEAT_SYSSTATUS         20
#define MSG_HEARTBEAT_CAMSTATUS         22
#define MSG_PROTOCOLTRANSTOR_HEART		40000 //协转心跳指令

//--------------------------------视频设备指令---------------------------------
#define MSG_FAULTNOTIFY_NEW		1100	// 向J2EE发送故障通知

#define MSG_REGTERM			    2002	//前端注册
#define MSG_REGTERM_RESP		2003	//前端注册回应

#define MSG_PERFORMANCE_THRESHOLD	2004 // 性能阀值

//分发向接入服务器的指令编号
#define MSG_DISPATCHREGISTER		3002	// 分发注册
#define MSG_REGDISPATCH_RESP    3003   //分发注册回复

#define MSG_REGSTORE			4002 //前端支持本地存储注册命令
#define MSG_REGSTORE_RESP		4003

#define MSG_DEVREALTIME		    5010	//向请求实时视频
#define MSG_DEVREALTIME_RESP	5011	//请求实时视频回应
#define MSG_STORESER_QUERY      1200    // 用户请求历史视频
#define REAL_VIDEO_CONN       	8000	//	
#define REAL_VIDEO_CONN_RESP    8001	//
#define REAL_VIDEOHEAD_SEND   	8002	//
#define REAL_VIDEOSTREAM_SEND	8004    //

#define MSG_USERHISTORY			5004	//查询历史视频列表
#define MSG_USERHISTORY_RESP	5005	//查询历史视频列表回应
#define HISTORY_VIDEO_CONN      8300	//
#define HISTORY_VIDEO_CONN_RESP 8301	//
#define HISTORY_Data_SEND   	8303	//
#define HISTORY_VIDEO_HEART   	8304	//

#define MSG_ALARM_NOTIFY            5214    //报警及相关联动动作通知
#define MSG_USERSETVIDEOPARAM		5032	// 用户设置视频参数策略
#define MSG_USERGETVIDEOPARAM		5034	// 用户得到视频参数策略
#define MSG_CENTERSTORAGE_QUERY	5408	// 中心存储视频查询
#define MSG_CENTERSTORAGE_RESP		5409    // 中心存储视频应答
#define MSG_CENTERSTORAGE_REQ		5410   // 中心存储视频请求

#define MSG_USERSETALARMOUTSTATE	5064
#define MSG_USERGETALARMOUTSTATE	5066
#define MSG_REMOTE_SETTING          5046
#define MSG_USERSETMOTION_PLAN          5080    //设置移动侦测的布撤防时间
#define MSG_USERGETMOTION_PLAN          5082    //查询移动侦测的布撤防时间
#define MSG_USERSETGPIN_PLAN            5084    //设置报警输入设备的布撤防时间
#define MSG_USERGETGPIN_PLAN            5086    //查询报警输入设备的布撤防时间
#define MSG_USERGETPTZSTATUS		5042	// 用户得到云台状态
#define MSG_USERPTZ			5006	// 用户控制云台
#define MSG_USERPTZ_CONTROL	8200    // 云台控制指令
#define MSG_USERPTZ_CONTROL_STOP   8202  // 停止云台控制
#define MSG_USERPTZ_CLEARPRESET		 8208  // 清除预置位
#define MSG_USERPTZ_SETPRESET				8210	// 设置预置位
#define MSG_USERPTZ_CLEARALLPRESET		8212	// 清除所有预置位
#define MSG_USERPTZ_ZOOM						8302  // 变焦
#define MSG_USERPTZ_ZOOM_STOP			8308  // 停止变焦

#define MSG_VOICETALK_QUERY		6112		// 用户查询语音对讲信息
#define MSG_VOICETALK_RESP			6113		// 语音对讲应答信息
#define MSG_VOICETALK_HEAD		6114			// 语音对讲包头
#define MSG_DEVVOICETALK_REQ			6116		// 语音呼叫
#define MSG_DEVVOICETALK_RESP			6117        // 语音呼叫应答信息

//--------------------------------数显设备指令---------------------------------
#define MSG_DISPLAY_REGISTER	6000	 //注册
#define MSG_DISPLAY_REALTIME	6002	 //实时视频
#define MSG_DISPLAY_HISTORY		6006	 //历史视频      
#define MSG_DISPLAY_RESPONSE_STATUS	6008 //发送状态信息，对应状态查询MSG_USER_QUERY_DISPLAY_STATUS

//---------------------客户端发出的对显示墙的控制指令   客户端-->接入服务器-------------------
#define MSG_USERREALTIME_TO_DISPLAY	5100	// 1.1 实时视频
#define MSG_USERLOOP_TO_DISPLAY		5102	// 1.2 实时轮询
#define MSG_USERHISTORY_TO_DISPLAY	5104	// 1.3 历史视频
#define MSG_USER_STOP_DISPLAY_PLAY	5106	// 1.4 停止播放
#define MSG_USER_QUERY_DISPLAY_STATUS	5108	// 1.5 状态查询

//-----------------------------存储通讯向接入服务器的指令编号--------------------------------------
#define MSG_STOREREGISTER_DV            4102    // DV存储服务器注册

//----------------------------------vod相关指令-----------------------------------------------------
#define MSG_RECORD_QUERY                5408    //录像查询         client->store   
#define MSG_CU_VOD                      5410    //用户进行录像点播
#define MSG_VOD_GETCAMCOUNT             4012    //GET CAMERA COUNT store->center
#define MSG_VOD_GETCAMSTOREINFO         4016    //GET CAMERA STORE INFO store->center
#define MSG_VOD_STOREPLANUPDATE         4024    //存储计划变更通知 center->store
#define MSG_VOD_RECORDSTATUS            4020    //存储状况查询 client->store
#define MSG_VOD_RECORD_UPLOAD           4028    //前端通知中心存储上传录像 terminal->store
#define MSG_VOD_CLICONNSTORE            4032    //客户端申请和存储服务器建立直连 client -> store 
#define MSG_VOD_PARAGET                 4080    //存储服务器参数查询 center->store
#define MSG_VOD_PARASET                 4084    //              设置 center->store
#define MSG_VOD_REBOOT                  4088    //重启存储服务器     center->store
#define MSG_KEYFRAME_QUERY              5110

//--------------------------------报警消息转发-----------------------
#define EVENT_ALARM_NOTIFY              9000   //from access server
#define EVENT_ALARM_NOTIFY2USER         9002   //from access server
#define EVENT_ALARM_NOTIFY2DISPLAY      9004   //from access server
#define EVENT_ALARM_NOTIFY2TERM         9006   //from access server
#define EVENT_ALARM_NOTIFY2DISPATCH     9008   //from access server
#define EVENT_ALARM_NOTIFY2STORE        9010   //from access server

//--------------------------------网管指令---------------------------------
#define MSG_FAULTNOTIFY_NEW		1100	// 向J2EE发送故障通知

//--------------------------------协转分包-------------------------------
#define SECOND_FRAME_HEADER_LEN 12
#define PACK_LENGTH 1024 * 10


//--------------------------------j2ee url path-------------------------------
//#define CMSURLPATH   "/megaeyes_access_support/main"
#define CMSURLPATH   "/route/rest/service/serve.do"/*"/megaeyes_access_support/main"*/ 
#define NMSURLPATH   "/nms/gate/msgd"
#define LOGURLPATH   "/logserver/add.log"

//----------------------------接入自定义宏-------------------------------
#define LEN_INTEGER 9			// 整形最大长度
#define LEN_TICKET  32
#define BUFFER_LEN			10 * 1024 //普通操作结果支持最大长度
#define HISBUFFER_LEN		10 * 1024 * 1024 //录像查询结果支持最大的长度
#define PASSIVE_MODE					8006			// 被动模式
#define ACTIVE_MODE				8008			// 主动模式
#define ACTIVE_CONN				8000
#define PASSIVE_CONN				8022
#define  MSG_ONOFF_MEDIA             110004     //分发上报视频

#define  MSG__DISCONNECT	110008		//设备连接成功
#define MSG__CONNECTED   110009        //客户端断开连接
#endif