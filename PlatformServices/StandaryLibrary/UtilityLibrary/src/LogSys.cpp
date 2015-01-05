//implement LogSys
//mlcai 2012-08-01

#include "Poco/AutoPtr.h"
#include "Poco/FileChannel.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/LocalDateTime.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "LogSys.h"
#include "QueueChannel.h"

//LogSys LogSys::g_logSys;
map<string, SharedPtr<LogSys> > LogSys::_logSystems;
Poco::Mutex LogSys::_logSysMutex;

LogSys::LogSys(const string& name) : m_loggerName(name)
{
	initPriorities();
}

LogSys::LogSys() : m_loggerName("log")
{
    initPriorities();
}

LogSys::~LogSys()
{
}

int LogSys::initialize(bool is_queue)
{
    AutoPtr<ConsoleChannel> pConsole = new ConsoleChannel;

	string logPath = Path::current();
	logPath += "log";

	File logDir(Path(logPath, Path::PATH_UNIX));
	if (!logDir.exists())
		logDir.createDirectory();


	string logFileName("log/");
	logFileName += m_loggerName;
	logFileName += ".log";
	AutoPtr<FileChannel> pFileChannel = new FileChannel(logFileName);
	pFileChannel->setProperty(FileChannel::PROP_ROTATION, "daily");
	pFileChannel->setProperty(FileChannel::PROP_ARCHIVE, "timestamp");
	pFileChannel->setProperty(FileChannel::PROP_PURGECOUNT, "10");

	AutoPtr<Formatter> pFormatter = new PatternFormatter("%Y-%m-%d %H:%M:%S [%p]: %t");
	AutoPtr<FormattingChannel> pFormatConsole = new FormattingChannel(pFormatter.get(), pConsole.get());
	AutoPtr<FormattingChannel> pFormatFileChannel = new FormattingChannel(pFormatter.get(), pFileChannel.get());

	AutoPtr<SplitterChannel> pSplitterChannel = new SplitterChannel;
	pSplitterChannel->addChannel(pFormatConsole.get());
	pSplitterChannel->addChannel(pFormatFileChannel.get());

	Logger& logger = Logger::get(m_loggerName);
	logger.setLevel(m_levelMap["information"]);

	if (is_queue)
	{
		AutoPtr<QueueChannel> queue_channel = new QueueChannel(pSplitterChannel.get());
		logger.setChannel(queue_channel);
	}
	else
		logger.setChannel(pSplitterChannel);

    m_isInitialed = true;
    return 0;
}

bool LogSys::isInitialed()
{
    return m_isInitialed;
}

void LogSys::log(Message::Priority prio, const std::string& msg)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}

		Message logMsg(logger_name, msg, prio);
		logMsg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(logMsg);
	}
	catch (...)
	{

	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value1, value2), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value1, value2, value3), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value1, value2, value3, value4), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value1, value2, value3, value4, value5), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

void LogSys::log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6)
{
	try
	{
		std::string logger_name;
		{
			Poco::Mutex::ScopedLock lock(name_mutex_);
			logger_name = m_loggerName;
		}
		Message msg(logger_name, format(fmt, value1, value2, value3, value4, value5, value6), prio);
		msg.setTime(LocalDateTime().timestamp());
		Logger& logger = Logger::get(logger_name);
		logger.log(msg);
	}
	catch (...)
	{
	}
}

LogSys& LogSys::getLogSys(const string& logName, bool is_queue)
{
	Poco::Mutex::ScopedLock locker(_logSysMutex);
	map<string, SharedPtr<LogSys> >::iterator it;
	if ((it = _logSystems.find(logName)) != _logSystems.end())
		return (*(it->second.get()));
	LogSys* logSys = new LogSys(logName);
	logSys->initialize(is_queue);
	_logSystems.insert(map<string, SharedPtr<LogSys> >::value_type(logName, SharedPtr<LogSys>(logSys)));
    return *logSys;
}

void LogSys::setLogLevel(const string& level)
{
	Logger& logger = getLogger();
	logger.setLevel(m_levelMap[level]);
}

Logger& LogSys::getLogger()
{
    return Logger::get(m_loggerName);
}

void LogSys::initPriorities()
{
	m_levelMap.insert(map<string, Message::Priority>::value_type("fatal", Message::PRIO_FATAL));
	m_levelMap.insert(map<string, Message::Priority>::value_type("critical", Message::PRIO_CRITICAL));
	m_levelMap.insert(map<string, Message::Priority>::value_type("error", Message::PRIO_ERROR));
	m_levelMap.insert(map<string, Message::Priority>::value_type("warning", Message::PRIO_WARNING));
	m_levelMap.insert(map<string, Message::Priority>::value_type("notice", Message::PRIO_NOTICE));
	m_levelMap.insert(map<string, Message::Priority>::value_type("information", Message::PRIO_INFORMATION));
	m_levelMap.insert(map<string, Message::Priority>::value_type("debug", Message::PRIO_DEBUG));
	m_levelMap.insert(map<string, Message::Priority>::value_type("trace", Message::PRIO_TRACE));
}
