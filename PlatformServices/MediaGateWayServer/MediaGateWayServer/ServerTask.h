#include "BussinessServer.h"
#include "BussinessEvent.h"


class CMsgTask:public BussinessEvent
{
public:
	
	 //void OnRecvMsg(const MegaeyesRequestHead& head,const unsigned char* pBuffer,int nLength);

    //����ֵ����-1�������������Ҫ�����ͷſͻ����׽���
    int onReadable(Operator &op);

};