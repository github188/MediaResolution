#ifndef QUEUE_H
#define QUEUE_H
#include "Global.h"
namespace ty
{
	template <class T>
	class  Queue
	{
	private:
		std::list<T> *_lists;					//容器
		int _max;								//支持的最长队列
		int _number;							//当前入列数
		Poco::Mutex   _mutex;			//入，出队列互斥锁
		Poco::Event	_ready;				//信号量
	public:
		Queue();
		Queue(int max);
		~Queue();
		void Destory()
		{
			_ready.set();
			
		}
		void Push(T& cnt);				//入队
		bool Pop(T& cnt,bool bWait=true);					//出队，队列为空时，阻塞操作
		int used()const {return _number;}
	};

	template<class T>
	Queue<T>::Queue()
	{
		_lists = new std::list<T>;
		_max=100;
		_number=0;
	}

	template<class T>
	Queue<T>::Queue(int max)
	{
		_lists = new std::list<T>;
		_number=0;
		_max=max;
	}

	template<class T>
	Queue<T>::~Queue()
	{
		//Destory();
		delete _lists;
	}

	template<class T>
	void Queue<T>::Push(T& cnt)
	{
		_mutex.lock();
		if(_number>=_max)
		{
			_mutex.unlock();
			return;
		}
		try
		{
			
			_lists->push_back(cnt);
			_number++;
			_ready.set();
			_mutex.unlock();
		}
		catch (...)
		{
			_mutex.unlock();
		}
		return;
	}

	template<class T>
	bool Queue<T>::Pop(T& cnt,bool bWait)
	{
		_mutex.lock();
		if(_number==0)
		{
			_mutex.unlock();
			if (bWait==false)
			{
				return false;
			}
			_ready.wait();
			_mutex.lock();
		}

		try
		{
			if(_lists->begin()==_lists->end())
			{
				_mutex.unlock();
				return false;
				
			}

			cnt = _lists->front();

			_lists->pop_front();

			_number--;
			
			_mutex.unlock();

			return true;
		}
		catch(...)
		{
			_mutex.unlock();

			return false;
		}
		
	}
}
#endif
