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

// ��������Ϣ�ṹ
typedef struct  
{
	int  nPort;			// ����˿�
	int  nID;			// �û���
	char ServerIP[16];	// ������ip��ַ(xxx.xxx.xxx.xxx)
}ST_SERVER_INFO;

/* ��̨����API
** �������� : OB_Matrix_PTZControl(...)
** ����˵�� : 
**     dwCamera		Ҫ��������������
**     dwCommand	����ָ���� (���������ָ���붨��)
**     dwStop		��־��, 0��ʾ��ʼָ�����ִ��, 1��ʾִֹͣ��
**                  �����������ǳɶ�ִ��, ��һ��ִ��ʱdwStop = 0������ʱdwStop=1, ��dwStop=1ʱdwCommand��������
**                  dwCamera�����Ǻ�֮ǰһ��
**     pSvrInfo		������������Ϣ
*/
OB_DLL BOOL WINAPI OB_Matrix_PTZControl(const DWORD dwCamera, const DWORD dwCommand, const DWORD dwStop, ST_SERVER_INFO* pSvrInfo);

/* ָ���붨�忪ʼ */
#define ZOOM_IN			11 /* ��д(�Ŵ�) */
#define ZOOM_OUT		12 /* ȫ��(��С) */
#define FOCUS_IN		13 /* ����ǰ�� */
#define FOCUS_OUT		14 /* ������ */
#define IRIS_ENLARGE	15 /* ��Ȧ���� */
#define IRIS_SHRINK		16 /* ��Ȧ��С */
#define TILT_UP			21 /* ��̨���� */
#define TILT_DOWN		22 /* ��̨���� */
#define PAN_LEFT		23 /* ��̨��ת */
#define PAN_RIGHT		24 /* ��̨��ת */

/* ָ���붨����� */

/* Ԥ��λ����API
** �������� : OB_Matrix_Position(...)
** ����˵�� : 
**     dwCamera		Ҫ��������������
**     dwPosition	Ԥ��λ���
**     dwOption		0��ʾ����Ԥ��λ, 1��ʾ����Ԥ��λ
**     pSvrInfo		������������Ϣ
*/
OB_DLL BOOL WINAPI OB_Matrix_Position(const DWORD dwCamera, const DWORD dwPosition, const DWORD dwOption, ST_SERVER_INFO* pSvrInfo);

/* �����л�����API
** �������� : OB_Matrix_Switch(...)
** ����˵�� : 
**     dwMonitor	���л��ļ�������
**     dwCamera		���л����������
**     pSvrInfo		������������Ϣ
*/
OB_DLL BOOL WINAPI OB_Matrix_Switch(const DWORD dwMonitor, const DWORD dwCamera, ST_SERVER_INFO* pSvrInfo);

/* �����л�����API
** �������� : OB_Matrix_SwitchEncode(...)
** ����˵�� : 
**     pEncodeCode	���л��ı���������
**     dwChannelNo	Ҫ�л����������ĵڼ���ͨ������1����������������ͨ����
**     dwCamera		���л����������
**     pSvrInfo		������������Ϣ
*/
OB_DLL BOOL WINAPI OB_Matrix_SwitchEncode(char* pEncodeCode, const DWORD dwChannelNo, const DWORD dwCamera, ST_SERVER_INFO* pSvrInfo);

/* �û��ǳ� : ���ñ��ӿڵ�����˳�ʱ������뱣��֮ǰ�û�ռ�õ���Դ
**            ���Ե������������û��ĵǳ��������ͷ���Դ
** �������� : OB_Matrix_CleanUp(...)
** ����˵�� : 
**     pSvrInfo		������������Ϣ(pSvrInfo->nID��дҪ�ǳ����û���Ϣ)
*/
OB_DLL BOOL WINAPI OB_Matrix_CleanUp(ST_SERVER_INFO* pSvrInfo);


// ������������ڸ�������
#define MAX_CAMERA		100

enum SwitchPriority
{
	AlarmPriorityMin = 0,	/*For Bound Check*/
	AlarmManual,			/*1 �ֶ����ֱ���*/
	AlarmMainCenter,		/*2 ʡ�����ֶ�*/	
	AlarmSubCenter,			/*3 �������ֶ�*/
	AlarmDetector,			/*4 ̽�������ֱ���*/
	AlarmInvasion,			/*5 ���ּ�ⱨ��*/
	AlarmHedgeLane,			/*6 ���ճ�������*/
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