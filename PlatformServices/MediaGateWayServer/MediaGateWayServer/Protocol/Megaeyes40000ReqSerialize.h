#ifndef MEGAEYES_40000REQ_SERIALIZE_H
#define MEGAEYES_40000REQ_SERIALIZE_H

#include "MegaeyesRequestSerialize.h"

#define PROTOCOL_ID_40000  40000
/**********************心跳40000************************************/
/*
<Message>
<DevWorkID>0000000000200000000000000350001</DevWorkID>
...
<DevWorkID>0000000000200000000000000350002</DevWorkID>
</Message>
*/
/******************************************************************/


class Megaeyes40000ReqSerialize:public MegaeyesRequestSerialize
{
public:
    Megaeyes40000ReqSerialize()
    {

    }
    ~Megaeyes40000ReqSerialize()
    {

    }
    virtual std::string Serialize()
    {
        m_xml.SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>");
        m_xml.AddElem("Message");
        m_xml.IntoElem();
        for (int i=0;i<(int)m_OnlineVec.size();i++)
        {
            m_xml.AddElem("DevWorkID",m_OnlineVec[i].workID);
        }
        m_xml.OutOfElem();

        m_cmdHead.cmdnum = PROTOCOL_ID_40000;
        m_cmdHead.length = m_xml.GetDoc().length() + 1;

        char *buffer = new char[sizeof(m_cmdHead) + m_cmdHead.length];
        memcpy(buffer, &m_cmdHead, sizeof(MegaeyesRequestHead));
        memcpy(buffer + sizeof(MegaeyesRequestHead), m_xml.GetDoc().c_str(), m_xml.GetDoc().length() + 1);
        std::string xml;
        xml.append(buffer, sizeof(MegaeyesRequestHead) + m_xml.GetDoc().length() + 1);
		delete[] buffer;
        return xml;
    }
    //反序列化
    virtual void UnSerialize(const std::string& strMessage)
    {

    }
public:
    typedef std::vector<VDeviceConfig> DevVec;
    DevVec m_OnlineVec;
};
#endif
