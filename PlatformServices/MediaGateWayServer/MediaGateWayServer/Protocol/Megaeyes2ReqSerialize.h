#ifndef MEGAEYES_2REQ_SERIALIZE_H
#define MEGAEYES_2REQ_SERIALIZE_H

#include "MegaeyesRequestSerialize.h"
/**********************ÀÏµÄÐÄÌø2************************************/
/*
<Message>
<DevType>2</DevType> 
</Message>
*/
/******************************************************************/


class Megaeyes2ReqSerialize:public MegaeyesRequestSerialize
{
public:
    Megaeyes2ReqSerialize()
    {
        onlineDeviceWorkID = " ";
    }
    ~Megaeyes2ReqSerialize()
    {

    }
    virtual std::string Serialize()
    {
        m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
        m_xml.AddElem("Message");
        m_xml.IntoElem();
        m_xml.AddElem("DevType","2");
        m_xml.OutOfElem();

		memcpy(m_cmdHead.src,onlineDeviceWorkID.c_str(),31);
        m_cmdHead.cmdnum = MSG_HEARTBEAT;
        m_cmdHead.length = m_xml.GetDoc().length() + 1;

        char buffer[512] = {0};
        memcpy(buffer, &m_cmdHead, sizeof(MegaeyesRequestHead));
        memcpy(buffer + sizeof(MegaeyesRequestHead), m_xml.GetDoc().c_str(), m_xml.GetDoc().length() + 1);
        std::string xml;
        xml.append(buffer, sizeof(MegaeyesRequestHead) + m_xml.GetDoc().length() + 1);
        return xml;
    }

    virtual void UnSerialize(const std::string& strMessage){}

public:
    std::string onlineDeviceWorkID;
};
#endif