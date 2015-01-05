#ifndef MegaeyesHead_H
#define MegaeyesHead_H

#include "define.h"

typedef struct MegaeyesHead
{ 
	int version;
	int length;
	int cmdnum;
	int cmdseq;
	MegaeyesHead():version(257),length(0),cmdnum(0),cmdseq(0)
	{
	
	}
} MegaeyesHead; 

typedef struct MegaeyesRequestHead : MegaeyesHead
{ 
	char src[32];
	char dst[32];
    MegaeyesRequestHead()
    {
        memset(src,0,32);
        memset(dst,0,32);
    }
} MegaeyesRequestHead; 


typedef struct MegaeyesResponseHead : MegaeyesHead
{ 
	int success;
	MegaeyesResponseHead():success(0)
	{

	}
} MegaeyesResponseHead; 

#endif