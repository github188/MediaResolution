//
// SocketReactor.cpp
//
// $Id: //poco/1.4/Net/src/SocketReactor.cpp#1 $
//
// Library: Net
// Package: Reactor
// Module:  SocketReactor
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/SocketNotifier.h"
#include "Poco/ErrorHandler.h"
#include "Poco/Thread.h"
#include "Poco/Exception.h"


using Poco::FastMutex;
using Poco::Exception;
using Poco::ErrorHandler;


namespace Poco {
namespace Net {


SocketReactor::SocketReactor():
	_stop(false),
	_timeout(DEFAULT_TIMEOUT),
	_pReadableNotification(new ReadableNotification(this)),
	_pWritableNotification(new WritableNotification(this)),
	_pErrorNotification(new ErrorNotification(this)),
	_pTimeoutNotification(new TimeoutNotification(this)),
	_pIdleNotification(new IdleNotification(this)),
	_pShutdownNotification(new ShutdownNotification(this))
{
}


SocketReactor::SocketReactor(const Poco::Timespan& timeout):
	_stop(false),
	_timeout(timeout),
	_pReadableNotification(new ReadableNotification(this)),
	_pWritableNotification(new WritableNotification(this)),
	_pErrorNotification(new ErrorNotification(this)),
	_pTimeoutNotification(new TimeoutNotification(this)),
	_pIdleNotification(new IdleNotification(this)),
	_pShutdownNotification(new ShutdownNotification(this))
{
}


SocketReactor::~SocketReactor()
{
}


void SocketReactor::run()
{
	Socket::SocketList readable;
	Socket::SocketList writable;
	Socket::SocketList except;
	
	while (!_stop)
	{
		try
		{
			readable.clear();
			writable.clear();
			except.clear();
			int nSockets = 0;
			{
				FastMutex::ScopedLock lock(_mutex);
				for (EventHandlerMap::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
				{
					if (it->second->accepts(_pReadableNotification))
					{
						readable.push_back(it->first);
						nSockets++;
					}
					if (it->second->accepts(_pWritableNotification))
					{
						writable.push_back(it->first);
						nSockets++;
					}
					if (it->second->accepts(_pErrorNotification))
					{
						except.push_back(it->first);
						nSockets++;
					}
				}
			}
			if (nSockets == 0)
			{
				onIdle();
			}
			else if (Socket::select(readable, writable, except, _timeout))
			{
				onBusy();

				printf("===============>current on_readable sockets %d<==================\n", readable.size());

				for (Socket::SocketList::iterator it = readable.begin(); it != readable.end(); ++it)
					dispatch(*it, _pReadableNotification);
				for (Socket::SocketList::iterator it = writable.begin(); it != writable.end(); ++it)
					dispatch(*it, _pWritableNotification);
				for (Socket::SocketList::iterator it = except.begin(); it != except.end(); ++it)
					dispatch(*it, _pErrorNotification);
			}
			else onTimeout();
		}
		catch (Exception& exc)
		{
			ErrorHandler::handle(exc);
		}
		catch (std::exception& exc)
		{
			ErrorHandler::handle(exc);
		}
		catch (...)
		{
			ErrorHandler::handle();
		}
	}
	onShutdown();
}

	
void SocketReactor::stop()
{
	_stop = true;
}


void SocketReactor::setTimeout(const Poco::Timespan& timeout)
{
	_timeout = timeout;
}

	
const Poco::Timespan& SocketReactor::getTimeout() const
{
	return _timeout;
}


void SocketReactor::addEventHandler(const Socket& socket, const Poco::AbstractObserver& observer)
{
	NotifierPtr pNotifier;
	{
		FastMutex::ScopedLock lock(_mutex);
		
		EventHandlerMap::iterator it = _handlers.find(socket);
		if (it == _handlers.end())
		{
			pNotifier = new SocketNotifier(socket);
			_handlers[socket] = pNotifier;
		}
		else pNotifier = it->second;
	}
	pNotifier->addObserver(this, observer);
}


void SocketReactor::removeEventHandler(const Socket& socket, const Poco::AbstractObserver& observer)
{
	NotifierPtr pNotifier;
	{
		FastMutex::ScopedLock lock(_mutex);
		
		EventHandlerMap::iterator it = _handlers.find(socket);
		if (it != _handlers.end())
		{
			pNotifier = it->second;
			if (pNotifier->countObservers() == 1)
			{
				_handlers.erase(it);
			}
		}
	}
	if (pNotifier)
	{
		pNotifier->removeObserver(this, observer);
	}
}


void SocketReactor::onTimeout()
{
	dispatch(_pTimeoutNotification);
}


void SocketReactor::onIdle()
{
	dispatch(_pIdleNotification);
}


void SocketReactor::onShutdown()
{
	dispatch(_pShutdownNotification);
}


void SocketReactor::onBusy()
{
}


void SocketReactor::dispatch(const Socket& socket, SocketNotification* pNotification)
{
	NotifierPtr pNotifier;
	{
		FastMutex::ScopedLock lock(_mutex);
		EventHandlerMap::iterator it = _handlers.find(socket);
		if (it != _handlers.end())
			pNotifier = it->second;
		else
			return;
	}
	dispatch(pNotifier, pNotification);
}


void SocketReactor::dispatch(SocketNotification* pNotification)
{
	std::vector<NotifierPtr> delegates;
	delegates.reserve(_handlers.size());
	{
		FastMutex::ScopedLock lock(_mutex);
		for (EventHandlerMap::iterator it = _handlers.begin(); it != _handlers.end(); ++it)
			delegates.push_back(it->second);
	}
	for (std::vector<NotifierPtr>::iterator it = delegates.begin(); it != delegates.end(); ++it)
	{
		dispatch(*it, pNotification);
	}
}


void SocketReactor::dispatch(NotifierPtr& pNotifier, SocketNotification* pNotification)
{
	try
	{
		pNotifier->dispatch(pNotification);
	}
	catch (Exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (std::exception& exc)
	{
		ErrorHandler::handle(exc);
	}
	catch (...)
	{
		ErrorHandler::handle();
	}
}


} } // namespace Poco::Net
