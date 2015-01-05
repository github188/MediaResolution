#include "BitUtility.h"
using namespace TY::Common;

const map<int,int> BitUtility::data =  BitUtility::CreateMap();

int BitUtility::GetOneBit(int num)
{
	return val>>(((sizeof(val)*8)+1)-num-1)&1;
}

int BitUtility::GetMultiBit(int startnum, int endnum)
{
	if(endnum<startnum)
	{
		return -1;
	}

	map<int,int>::const_iterator iter = BitUtility::data.find(endnum-startnum+1);
	if(iter==BitUtility::data.end())
	{
		return -1;
	}

	return val>>(((sizeof(val)*8)+1)-endnum-1)&iter->second;
}
