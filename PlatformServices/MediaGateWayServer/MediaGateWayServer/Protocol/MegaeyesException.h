#ifndef MEGAEYES_EXCEPTION_H
#define MEGAEYES_EXCEPTION_H

#include <stdexcept>
#include <string>
class CMegaEyesException:public std::exception
{
public:
	CMegaEyesException(const std::string& msg, int code = 0):m_strMsg(msg),m_nCode(code)
	{

	}
	~CMegaEyesException() throw()
	{

	}
	const std::string& message() const
	{
		return m_strMsg;
	}
	const char* what() const throw()
	{
		return m_strMsg.c_str();
	}
public:
	std::string m_strMsg;
	int m_nCode;
};
#endif