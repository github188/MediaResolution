#ifndef MSG_HANDER_H
#define MSG_HANDER_H
#include "Global.h"
#include "Operator.h"
#include "StaticVar.h"
#include "StandaryLibrary.h"

namespace ty
{
	class StandaryLibrary_API MsgHander
	{
	private:
		SocketReactor& _reactor;
		
	public:
		MsgHander(StreamSocket& socket, SocketReactor& reactor):
		_socket(socket), _reactor(reactor), is_delete_by_self(0), is_process_by_self(0)
		{
			Start();
		}
		virtual ~MsgHander()
		{

		}
		virtual void Start();
		
		virtual void Stop();

		virtual void Release();
		
		virtual void onReadable(ReadableNotification* pNf);

		StreamSocket   _socket;

		int is_delete_by_self;			//0Ϊ��������ͷ�MsgHander������Ϊҵ���ͷ�

		int is_process_by_self;			//0Ϊ���������ɶ��¼��� ����Ϊҵ����

		Poco::Mutex _sockMutex;
	};
}
#endif
