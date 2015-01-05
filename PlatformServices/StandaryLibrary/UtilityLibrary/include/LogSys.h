//this class encapsulate poco logger for conveniency to use
//mlcai 2012-07-31

#ifndef LogSys_H_INCLUDED
#define LogSys_H_INCLUDED

#include "Poco/Format.h"
#include "Poco/Logger.h"
#include "Poco/SharedPtr.h"
#include "Poco/Mutex.h"
#include <map>

using namespace std;
using namespace Poco;

class LogSys
{
public:
	LogSys(const string& name);
    LogSys();
    ~LogSys();

    int initialize(bool is_queue);
    bool isInitialed();

    void log(Message::Priority prio, const std::string& msg);
    void log(Message::Priority prio, const std::string& fmt, const Any& value);
    void log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2);
    void log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3);
    void log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4);
    void log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5);
    void log(Message::Priority prio, const std::string& fmt, const Any& value1, const Any& value2, const Any& value3, const Any& value4, const Any& value5, const Any& value6);

    static LogSys& getLogSys(const string& logName, bool is_queue =false);
	
	void setLogLevel(const string& level);

    Logger& getLogger();

private:
	void initPriorities();
    
	std::string m_loggerName;
	Poco::Mutex name_mutex_;
    bool m_isInitialed;
    map<string, Message::Priority> m_levelMap;
    //static LogSys g_logSys;
	static map<string, SharedPtr<LogSys> > _logSystems;
	static Poco::Mutex _logSysMutex;
};

#endif // LogSys_H_INCLUDED
