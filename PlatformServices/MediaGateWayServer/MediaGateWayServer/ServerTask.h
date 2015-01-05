#include "BussinessServer.h"
#include "BussinessEvent.h"


class CMsgTask:public BussinessEvent
{
public:
	
	 //void OnRecvMsg(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int nLength);

    //返回值等于-1，代表服务器需要主动释放客户端套接字
    int onReadable(Operator &op);

};