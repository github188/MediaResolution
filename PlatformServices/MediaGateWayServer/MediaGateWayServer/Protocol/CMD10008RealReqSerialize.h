#ifndef CMD_REALREQUEST_H
#define CMD_REALREQUEST_H
#include "CMDRequestSerialize.h"

/**********************ʵʱ��Ƶ10008************************************/
/*
<Information>
	<ChannelNum>1-16</ChannelNum>
	<Token>20�ֽ�</Token>
	<StreamType>1/2/3/4</StreamType>
	<MediaServerIp><MediaServerIp> ��ѡNAT�豸
	<MediaServerPort>< MediaServerPort >��ѡNAT�豸
</Information>

*/
/************************************************************************/
class CMD10008RealReqSerialize:public CMDRequestSerialize
{
public:
	CMD10008RealReqSerialize()
	{
	}
	~CMD10008RealReqSerialize()
	{

	}
public:
    virtual std::string Serialize()
    {
        return NULL;
    }

	//�����л�
	virtual void UnSerialize(const std::string& strMessage)
	{
		StrUtility utility;
		if (!(m_xml.SetDoc(strMessage) && utility.ForeachXml(m_xml) && 
			utility.GetxmlValue("Information/ChannelNum", channel_) && utility.GetxmlValue("Information/Token", token_) 
			&& utility.GetxmlValue("Information/StreamType", stream_type_)))
            throw Poco::Exception("�����ʵʱ��Ƶ��Ϣ��", 10008);
		
		utility.GetxmlValue("Information/MediaServerIp", server_ip_);
		utility.GetxmlValue("Information/MediaServerPort", server_port_);
	}

public:
	int channel_;
	std::string token_;
	int stream_type_;
	std::string server_ip_;
	int server_port_;
};

#endif
