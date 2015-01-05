//implement MsgHandler and MsgHandlerCollector

#include "MsgHandler.h"

int io_count = 0;

void MsgHander::Start()
{
	_reactor.addEventHandler(_socket, Observer<MsgHander, ReadableNotification>(*this, &MsgHander::onReadable));
}

void MsgHander::Stop()
{
	_reactor.removeEventHandler(_socket, Observer<MsgHander, ReadableNotification>(*this, &MsgHander::onReadable));
}

void MsgHander::Release()
{
	Stop();
	_socket.close();
	delete this;
}

void MsgHander::onReadable(ReadableNotification* pNf)
{
	pNf->release();

	Stop();

	Operator op;
	op.hand=this;
	StaticVar::g_MsgQueue.Push(op);
}
