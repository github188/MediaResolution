//报警控制器实现
//mlcai 2014-03-06

#include "AlarmController.h"
#include "LogInfo.h"
#include "LogSys.h"


using namespace Poco;

AlarmController::AlarmController():_alarm_queue(1000)
{
	is_stop_=false;
}

AlarmController::~AlarmController()
{
	is_stop_=true;
	_post_thr.join();
}

void AlarmController::Start(const std::string& access_ip, int acess_port)
{
	_access_ip = access_ip;
	_access_port = acess_port;
	_post_thr.start(*this);
}

void AlarmController::printFailedAlarmInfo(string fileName,string failedInfo)
{
	FILE* f=fopen(fileName.c_str(),"a+");
	if(f!=NULL)
	{
		char LTime[256]={0};
		Poco::LocalDateTime time;
		sprintf(LTime,"%d-%02d-%02d-%02d-%02d-%02d",time.year(),time.month(),time.day(),time.hour(),time.minute(),time.second());
		fprintf(f,"%s :%s\n",LTime,failedInfo.c_str());
	}
	fclose(f);
}

void AlarmController::run()
{
	char buffer[1024];
	MegaeyesResponseHead *pResponseHead;
	while (!is_stop_)
	{
		Megaeyes5214ReqSerialize alarm_message;

		if(!_alarm_queue.Pop(alarm_message,false))
		{
			Thread::sleep(50);
			continue;
		}

		Net::SocketAddress address(_access_ip, _access_port);

		Net::StreamSocket socket;

		try
		{
			StreamSocket  socket_imp(address);	
			socket = socket_imp;
		}
		catch (...)
		{
			break;
		}
		string content =  alarm_message.Serialize();
		if(content.length()!=NetUtility::SafeSendData(socket,(char*)content.c_str(),content.length()))
		{
			//add by sunminping  记录发送5214报警失败的信息
			if(alarm_in==GetAlarmType())
			{
				LogSys& alarm_log=LogSys::getLogSys(LogInfo::Instance().AlarmIn(), true);
				alarm_log.log(Message::PRIO_NOTICE,"%s 向接入发送5214消息失败!",alarm_message.m_strDevNaming.c_str());
			}
			else
			{
				LogSys& alarm_log=LogSys::getLogSys(LogInfo::Instance().AlarmDetect(),true);
				alarm_log.log(Message::PRIO_NOTICE,"%s 向接入发送5214消息失败!",alarm_message.m_strDevNaming.c_str());
			}
			continue;
			//end add
		}
		pResponseHead = (MegaeyesResponseHead*)buffer;
		if(!NetUtility::Receive(socket,(char *)buffer,sizeof(MegaeyesResponseHead)))
		{
			//add by sunminping
			//alarm_log.log(Message::PRIO_NOTICE,"从接入接收5215命令协议头信息失败!");
			//end add
		}
		if(pResponseHead->success != 0)
		{	
			//char buf[100]={0};
			//alarm_log.Log(Message::PRIO_NOTICE,"从接入接收的5215命令协议头的success=%d",pResponseHead->success);
			//printFailedAlarmInfo(GetFileName(),buf);
			continue;
		}     
		if(!NetUtility::Receive(socket,(char *)buffer,pResponseHead->length))
		{
			//g_log<<"MediaGateWayServer receive heart(new) failed\n";
			//printFailedAlarmInfo(GetFileName(),"从接入接收5215命令对应的内容体失败!");
		}
	}
}
