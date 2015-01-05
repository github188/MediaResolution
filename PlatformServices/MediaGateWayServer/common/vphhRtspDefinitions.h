
#ifndef VPHHRTSPDEFINITIONS_H
#define VPHHRTSPDEFINITIONS_H

#define MAX_MEDBUF_LEN (1024*500)

#pragma pack(1)
struct _Rtsp_Stream_Head
{
	BYTE sHead[12];
	unsigned int nFrame;
	int nPos;
	unsigned short videoWidth;
	unsigned short videoHeight;
	int unKownParam1;
	int unKownParam2;
	int unKownParam3;
	int unKownParam4;
	int unKownParam5;
	int unKownParam6;
	int unKownParam7;
	int unKownParam8;
	int unKownParam9;
	int unKownParam10;
	/*
	_Rtsp_Stream_Head()
	{
		memcpy(sHead,"HXHT_H264", sizeof( "HXHT_H264") );
		nFrame=25;
		nPos=64;
	}
	*/
};

/*
typedef char int8;
typedef int int32;
typedef char int8;


//针对安讯士设备进行特殊定义
//当为安讯士的视频服务器时（属于较旧设备），FrameStartMagicString为XVID_MAGIC_STR，视频编码类型为MPEG4，StartCode为XVID_START_CODE
//当为安讯士的球机和枪机（属于较新设备），FrameStartMagicString为H264_MAGIC_STR，视频编码类型为H264，StartCode为H264_START_CODE
//不论为何种设备，对于为音频数据，其FrameStartMagicString均为G711_START_CODE，其支持的音频格式为G.711 μ-law 64 kbit/s
const int8 H264_MAGIC_STR[4] = {'H', '2', '6', '4'};
const int8 XVID_MAGIC_STR[4] = {'X', 'V', 'I', 'D'};
const int8 H264_START_CODE[4] = {0x00, 0x00, 0x00, 0x01};
const int8 XVID_START_CODE[4] = {0x00, 0x00, 0x01, 0x00};
const int8 G711_START_CODE[4] = {'G', '7', '1', '1'};			//默认为G.711 μ-law 64 kbit/s

struct RtspFramHead {
	int8  FrameStartMagicString[4];
	int32 iFrameLen;
	int8  StartCode[4];

	//RtspFramHead() 
	//{
	//	int8 Code[] = {0x00, 0x00, 0x00, 0x01};
	//	memcpy(FrameStartMagicString,"STAT", sizeof(FrameStartMagicString));
	//	memcpy(StartCode, Code, sizeof(StartCode));
	//}
};
*/
const BYTE H264_START_CODE[4] = {0x00, 0x00, 0x00, 0x01};

enum VP_FRAME_TYPE {
	VPFT_VIDEO = 1,
	VPFT_AUDIO = 2,
};

enum VP_CODEC_TYPE {
	VPCT_G711_ULAW = 1,
	VPCT_G711_ALAW,
	VPCT_H264 = 0x10,
	VPCT_XVID = 0x11,
};

typedef struct _VP_FRAME_TIME_T {
	USHORT year;
	BYTE month;
	BYTE day;
	BYTE hour;
	BYTE minute;
	BYTE second;
	USHORT millisecond;
} VP_FRAME_TIME_T;

typedef struct _VP_FRAME_HEAD_T {
	UINT frame_len;				//当前帧长度，不包含VP_FRAME_HEAD_T长度
	BYTE frame_type;				//0:保留，1:视频帧，2:音频帧
	BYTE codec_type;			//编码器类型: 0:保留，0x01:G.711 μ-law 64 kbit/s, 0x10:H264, 0x11:XVID
	VP_FRAME_TIME_T date_time;
	BYTE reverse[5];			//保留
} VP_FRAME_HEAD_T;

#pragma pack()

CONST INT SPS_PPS_LEN	= 26;

#endif