#ifndef _CHANDLER_H
#define _CHANDLER_H
#include "Global.h"
#include "Device.h"
#include "Const.h"
#include "Operator.h"

using namespace ty;

class CHandler
{
public:
	virtual ~CHandler();

	virtual int Run(Operator& op, CmdHead& head, char* body) = 0;

	bool OperateLongSockEvent(StreamSocket& sock, Device* dev, char* send_buffer, int send_len, char* recv_buffer);

	virtual int AnswerCorrectRes(StreamSocket& sock, CmdHead& head, string body = "");

	virtual int AnswerErrorRes(StreamSocket& sock, CmdHead& head, int nRet, string body);

protected:
	CHandler();
};

#endif