//实现包序号生成

#include "CmdHead.h"

SeqNumGenerator SeqNumGenerator::instance_;

SeqNumGenerator& SeqNumGenerator::Instance()
{
	return instance_;
}

int SeqNumGenerator::Generate()
{
	FastMutex::ScopedLock lock(num_mtx_);
	num_ = (num_ ++) % 100000;
	return num_;
}
