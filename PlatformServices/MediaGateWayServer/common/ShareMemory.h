/***********************************************************************
Author:Sunminping
Date:2014-8-29
Function:为了实现license限制点位数，需要用到共享内存
***********************************************************************/

#ifndef _H_SHARE_MEMORY
#define _H_SHARE_MEMORY

namespace smp
{
	typedef struct sShareMemory
	{
		int nCameraCount;            //点位个数
	}ShareMemory,*PShareMemory;

}

#endif //_H_SHARE_MEMORY