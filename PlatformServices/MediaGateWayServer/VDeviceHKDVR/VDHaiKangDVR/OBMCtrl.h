/* 
** VAR Matrix Control Interface Definition Header v1.0
** OB Telecom 2008.8 
*/

#ifndef _OB_MATRIX_CTRL_H
#define _OB_MATRIX_CTRL_H

#ifdef  OB_DLL_EXPORTS 
#define OB_DLL _declspec(dllexport)
#else
#define OB_DLL _declspec(dllimport)
#endif

// 服务器信息结构
typedef struct  
{
	int  nPort;			// 服务端口
	int  nID;			// 用户号
	char ServerIP[16];	// 服务器ip地址(xxx.xxx.xxx.xxx)
}ST_SERVER_INFO;

/* 云台控制API
** 函数名称 : OB_Matrix_PTZControl(...)
** 参数说明 : 
**     dwCamera		要操作的摄像机编号
**     dwCommand	操作指令码 (具体见下面指令码定义)
**     dwStop		标志码, 0表示开始指令码的执行, 1表示停止执行
**                  本函数必须是成对执行, 第一次执行时dwStop = 0，结束时dwStop=1, 当dwStop=1时dwCommand可以任意
**                  dwCamera必须是和之前一致
**     pSvrInfo		服务器连接信息
*/
OB_DLL BOOL WINAPI OB_Matrix_PTZControl(const DWORD dwCamera, const DWORD dwCommand, const DWORD dwStop, ST_SERVER_INFO* pSvrInfo);

/* 指令码定义开始 */
#define ZOOM_IN			11 /* 特写(放大) */
#define ZOOM_OUT		12 /* 全景(缩小) */
#define FOCUS_IN		13 /* 焦点前调 */
#define FOCUS_OUT		14 /* 焦点后调 */
#define IRIS_ENLARGE	15 /* 光圈扩大 */
#define IRIS_SHRINK		16 /* 光圈缩小 */
#define TILT_UP			21 /* 云台向上 */
#define TILT_DOWN		22 /* 云台向下 */
#define PAN_LEFT		23 /* 云台左转 */
#define PAN_RIGHT		24 /* 云台右转 */

/* 指令码定义结束 */

/* 预置位操作API
** 函数名称 : OB_Matrix_Position(...)
** 参数说明 : 
**     dwCamera		要操作的摄像机编号
**     dwPosition	预置位编号
**     dwOption		0表示调用预置位, 1表示设置预置位
**     pSvrInfo		服务器连接信息
*/
OB_DLL BOOL WINAPI OB_Matrix_Position(const DWORD dwCamera, const DWORD dwPosition, const DWORD dwOption, ST_SERVER_INFO* pSvrInfo);

/* 矩阵切换操作API
** 函数名称 : OB_Matrix_Switch(...)
** 参数说明 : 
**     dwMonitor	待切换的监视器号
**     dwCamera		待切换的摄像机号
**     pSvrInfo		服务器连接信息
*/
OB_DLL BOOL WINAPI OB_Matrix_Switch(const DWORD dwMonitor, const DWORD dwCamera, ST_SERVER_INFO* pSvrInfo);

/* 矩阵切换操作API
** 函数名称 : OB_Matrix_SwitchEncode(...)
** 参数说明 : 
**     pEncodeCode	待切换的编码器编码
**     dwChannelNo	要切换到编码器的第几个通道，从1到编码器的最大编码通道数
**     dwCamera		待切换的摄像机号
**     pSvrInfo		服务器连接信息
*/
OB_DLL BOOL WINAPI OB_Matrix_SwitchEncode(char* pEncodeCode, const DWORD dwChannelNo, const DWORD dwCamera, ST_SERVER_INFO* pSvrInfo);

/* 用户登出 : 调用本接口的软件退出时如果不想保留之前用户占用的资源
**            可以调本函数进行用户的登出操作，释放资源
** 函数名称 : OB_Matrix_CleanUp(...)
** 参数说明 : 
**     pSvrInfo		服务器连接信息(pSvrInfo->nID填写要登出的用户信息)
*/
OB_DLL BOOL WINAPI OB_Matrix_CleanUp(ST_SERVER_INFO* pSvrInfo);


// 下面的内容用于福建永武
#define MAX_CAMERA		100

enum SwitchPriority
{
	AlarmPriorityMin = 0,	/*For Bound Check*/
	AlarmManual,			/*1 手动火灾报警*/
	AlarmMainCenter,		/*2 省中心手动*/	
	AlarmSubCenter,			/*3 分中心手动*/
	AlarmDetector,			/*4 探测器火灾报警*/
	AlarmInvasion,			/*5 入侵检测报警*/
	AlarmHedgeLane,			/*6 避险车道报警*/
	AlarmPriorityMax		/*For Bound Check*/
};

typedef struct _STRUCT_VideoMatrixEquip
{
	int equipType;
	char* producerCode;
	char* ip;
	int port;
	int serialPortNumber;
	int baudRate;
	int parity;
	int dataBits;
	int stopBits;
} VideoMatrixEquip;

typedef struct _STRUCT_SwitchResultArg
{
	int result;
	char* message;
} SwitchResultArg;

typedef struct _STRUCT_RecordResultArg
{
	int result;
	char* message;
	int cameras[MAX_CAMERA];
} RecordStartResultArg, RecordStopResultArg;

OB_DLL SwitchResultArg WINAPI switchVideo(int cameras[], int monitors[], VideoMatrixEquip videoMatrixEquip, int priority);
OB_DLL int WINAPI getRecordableChannelCount(VideoMatrixEquip videoMatrixEquip, char* regionCode);
OB_DLL RecordStartResultArg WINAPI startRecording(int cameras[], VideoMatrixEquip videoMatrixEquip, bool isOverwrite);
OB_DLL RecordStopResultArg WINAPI stopRecording(int cameras[], VideoMatrixEquip videoMatrixEquip);

#endif