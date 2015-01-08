#ifndef _H_CONST_H_
#define _H_CONST_H_

#include "define.h"

typedef struct CmdHead
{ 
	int version;
	int length;
	int cmdnum;
	int cmdseq;
	int exten;
	int success;
	char src[20];
	char dst[20];
	CmdHead():version(2056),length(0),cmdnum(0),cmdseq(0),
		exten(0),success(0)
	{
		memset(src, 0, sizeof(src));
		memset(dst, 0, sizeof(dst));
	}
} CmdHead;

#endif
