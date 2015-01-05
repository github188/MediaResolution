#pragma once
#include <Windows.h>
namespace MegaClient
{

const long IDX_FILE_END = 1;
const long IDX_SNAPSHOT = 2;

struct FRAME_INFO
{
	enum 
	{ 
		AUDIO16 = 1, 
		RGB32, 
		YV12, 
		UYVY, 
		YUV420, 
		YUY2, 
		YUYV, 
		AUDIO8 
	};

	long nWidth;
	long nHeight;
	long nStamp;
	long nType;
	long nFrameRate; 
};

class IVideoDecoder
{
public:
//----------------文件播放专用接口-------------------------//
	// 打开文件
	virtual long OpenFile( const char * sFileName ) = 0;
	// 关闭文件
	virtual long CloseFile() = 0;
	virtual long GetFileTime( long *lTimeLength ) = 0;
	virtual long SetPlayTime( long lSecond ) = 0;
	virtual long GetPlayTime( long * lSecond ) = 0;
	virtual long SetPlayPos( long lPos ) = 0;
	virtual long GetPlayPos( long * lPos ) = 0;
	virtual long OneByOneBack() = 0;
//---------------------------------------------------------//

//----------------流播放专用接口---------------------------//
	// 打开流播放模式
	virtual long OpenStream( const unsigned char * pVideoHead, long lSize, long lSpeedControl ) = 0;
	// 关闭流播放模式
	virtual long CloseStream() = 0;
	// 输入流数据，返回值一定为0.
	virtual long InputData( const unsigned char * pVideoStream, long lSize ) = 0;
//---------------------------------------------------------//

//----------------公用接口---------------------------------//
	// 开始播放，OpenFile和OpenStream之后解码库处于暂停状态，调用Play之后才开始
	// 播放，如果在快放或慢放状态调用此接口，则恢复正常播放速度。如果在OpenFile
	// 和OpenStream之后首次调用Play时指定的hWnd为NULL，则解码库只进行解码，不显
	// 示图像，此时客户程序可以通过设置回调自行处理图像显示部分。
	virtual long Play( HWND hWnd ) = 0;
	// 停止播放
	virtual long Stop() = 0;
	// 暂停播放
	virtual long Pause( long lPause ) = 0;

	// 设置播放速度，lSpeed取值可以为-16到+16之间，lSpeed为正值时，表示以正常速
	// 度的lSpeed倍速播放，lSpeed为负值时，表示以正常速度的lSpeed的绝对值分之一
	// 倍速播放
	virtual long SetSpeed( long lSpeed ) = 0;
	// 读取播放速度
	virtual long GetSpeed( long * lSpeed ) = 0;

	// 解码库对于声音播放以共享方式实现
	virtual long PlayAudio() = 0;
	virtual long StopAudio() = 0;

	// 设置音量大小，取值范围0-0XFFFF
	virtual long SetVolume( long lVolume ) = 0;

	// 抓图
	virtual long Snapshot( const char * sFileName ) = 0;

	// 录像
	virtual long StartRecord( const char * sFileName ) = 0;
	virtual long StopRecord() = 0;

	virtual long OneByOne() = 0;
	virtual long Refresh() = 0;
	//virtual long GetPictureSize( long * lWidth, long * lHeight ) = 0;
	//virtual long SetMessage( long idx, HWND hWnd, long lMsg ) = 0;

	virtual long SetDecodeCallBack( long ( __stdcall * DecodeCallBack )( 
		const char * pBuf, long nSize, FRAME_INFO * pFrameInfo,
			long lParam), long lParam ) = 0;
// 	virtual long SetDrawCallBack( long(__stdcall * DrawCallBack )(
// 			HDC hDC, long lParam ), long lParam ) = 0;

//	virtual long GetDecoderInfo( char * sDecodeName, long lNameLen, long * lVersion ) = 0;
//	virtual long QueryFunction( const char * sFunction, long * lCanUse ) = 0;
//---------------------------------------------------------//
};

class IVideoDecoderManager
{
public:
	virtual long Initial( HWND hWnd ) = 0;
	virtual long CreateDecoder( const char * sProtocol, IVideoDecoder ** pDecoder ) = 0;
	virtual long CreateDecoder( const unsigned char * sStreamHead, long lStreamHeadLen, IVideoDecoder ** pDecoder ) = 0;
	virtual long ReleaseDecoder( IVideoDecoder * pDecoder ) = 0;
};

};



