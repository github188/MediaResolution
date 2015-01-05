#pragma once
#include "Decoder.h"

typedef long ( CALLBACK * DecodeCallBack_t ) ( long hHandle, const char * pBuf, long  nSize, MegaClient::FRAME_INFO * pFrameInfo, long lParam );
typedef long( CALLBACK * DrawCallBack_t ) ( long hHandle, HDC hDC, long lParam );

namespace MegaClient
{
	class IVideoDecoder_M
	{
	public:
		virtual long Initial( long hWnd ) = 0;
		virtual long Free() = 0;
		//virtual long GetVersion( long * lVer ) = 0;
		//virtual long QueryFunction( const char * sFunctionName, long * lCanUse ) = 0;
		virtual long OpenFile( const char * sFileName, long * hHandle) = 0;
		virtual long CloseFile( long hHandle ) = 0;
		virtual long GetFileTime( long hHandle, long *lTimeLength ) = 0;
		virtual long OpenStream( const unsigned char * pStreamHead, long lSize, long lSpeedControl, long * hHandle) = 0;
		virtual long CloseStream( long hHandle ) = 0;
		virtual long InputData( long hHandle, const unsigned char * pStream, long lSize ) = 0;
		virtual long PlayAudio( long hHandle ) = 0;
		virtual long StopAudio( long hHandle ) = 0;
		virtual long SetVolume( long hHandle, long lVolume ) = 0;
		virtual long Play( long hHandle, long hWnd ) = 0;
		virtual long Stop( long hHandle ) = 0;
		virtual long Pause( long hHandle, long lPause ) = 0;
		virtual long SetPlayTime( long hHandle, long lSecond ) = 0;
		virtual long GetPlayTime( long hHandle, long * lSecond ) = 0;
		virtual long SetPlayPos( long hHandle, long lPos ) = 0;
		virtual long GetPlayPos( long hHandle, long * lPos ) = 0;
		virtual long SetSpeed( long hHandle, long lSpeed ) = 0;
		virtual long GetSpeed( long hHandle,long *lSpeed ) = 0;
		virtual long OneByOne( long hHandle ) = 0;
		virtual long OneByOneBack( long hHandle ) = 0;
		virtual long StartRecord( long hHandle, const char * sFileName ) = 0;
		virtual long StopRecord( long lHandle ) = 0;
		virtual long Snapshot( long hHandle, const char * sFileName ) = 0;
		virtual long Refresh( long hHandle ) = 0;
		//virtual long GetPictureSize( long hHandle, long * lWidth, long * lHeight ) = 0;
		//virtual long SetMessage( long hHandle, long idx, long hWnd, long lMsg ) = 0;
		virtual long SetDecodeCallBack( long hHandle, long ( CALLBACK * DecodeCallBack ) ( long hHandle, const char * pBuf, long  nSize, FRAME_INFO * pFrameInfo, long lParam ), long lParam ) = 0;
		//virtual long SetDrawCallBack( long hHandle, long( CALLBACK* DrawCallBack  ) ( long hHandle, HDC hDC, long lParam ), long lParam ) = 0;
		virtual long InputData2( long hHandle, const unsigned char * pStream, long lSize ){return E_NOTIMPL;}
	};
};