#ifndef QUEUE_H
#define QUEUE_H
#include "Global.h"
namespace ty
{
	template <class T>
	class  Queue
	{
	private:
		std::list<T> *_lists;					//����
		int _max;								//֧�ֵ������
		int _number;							//��ǰ������
		Poco::Mutex   _mutex;			//�룬�����л�����
		Poco::Event	_ready;				//�ź���
	public:
		Queue();
		Queue(int max);
		~Queue();
		void Destory()
		{
			_ready.set();
			
		}
		void Push(T& cnt);				//���
		bool Pop(T& cnt,bool bWait=true);					//���ӣ�����Ϊ��ʱ����������
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
