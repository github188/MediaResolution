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
//----------------�ļ�����ר�ýӿ�-------------------------//
	// ���ļ�
	virtual long OpenFile( const char * sFileName ) = 0;
	// �ر��ļ�
	virtual long CloseFile() = 0;
	virtual long GetFileTime( long *lTimeLength ) = 0;
	virtual long SetPlayTime( long lSecond ) = 0;
	virtual long GetPlayTime( long * lSecond ) = 0;
	virtual long SetPlayPos( long lPos ) = 0;
	virtual long GetPlayPos( long * lPos ) = 0;
	virtual long OneByOneBack() = 0;
//---------------------------------------------------------//

//----------------������ר�ýӿ�---------------------------//
	// ��������ģʽ
	virtual long OpenStream( const unsigned char * pVideoHead, long lSize, long lSpeedControl ) = 0;
	// �ر�������ģʽ
	virtual long CloseStream() = 0;
	// ���������ݣ�����ֵһ��Ϊ0.
	virtual long InputData( const unsigned char * pVideoStream, long lSize ) = 0;
//---------------------------------------------------------//

//----------------���ýӿ�---------------------------------//
	// ��ʼ���ţ�OpenFile��OpenStream֮�����⴦����ͣ״̬������Play֮��ſ�ʼ
	// ���ţ�����ڿ�Ż�����״̬���ô˽ӿڣ���ָ����������ٶȡ������OpenFile
	// ��OpenStream֮���״ε���Playʱָ����hWndΪNULL��������ֻ���н��룬����
	// ʾͼ�񣬴�ʱ�ͻ��������ͨ�����ûص����д���ͼ����ʾ���֡�
	virtual long Play( HWND hWnd ) = 0;
	// ֹͣ����
	virtual long Stop() = 0;
	// ��ͣ����
	virtual long Pause( long lPause ) = 0;

	// ���ò����ٶȣ�lSpeedȡֵ����Ϊ-16��+16֮�䣬lSpeedΪ��ֵʱ����ʾ��������
	// �ȵ�lSpeed���ٲ��ţ�lSpeedΪ��ֵʱ����ʾ�������ٶȵ�lSpeed�ľ���ֵ��֮һ
	// ���ٲ���
	virtual long SetSpeed( long lSpeed ) = 0;
	// ��ȡ�����ٶ�
	virtual long GetSpeed( long * lSpeed ) = 0;

	// �����������������Թ���ʽʵ��
	virtual long PlayAudio() = 0;
	virtual long StopAudio() = 0;

	// ����������С��ȡֵ��Χ0-0XFFFF
	virtual long SetVolume( long lVolume ) = 0;

	// ץͼ
	virtual long Snapshot( const char * sFileName ) = 0;

	// ¼��
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



