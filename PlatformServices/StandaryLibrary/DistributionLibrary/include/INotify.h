//抽象回调通知基类
#ifndef INotify_H
#define INotify_H

#include <string>
using namespace std;

class INotify
{
	public:
		virtual ~INotify() {}
		virtual string Notify(int type, char* buffer, int length) = 0;
};
#endif
