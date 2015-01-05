#ifndef CMD10009REALRESSERIALIZE_H
#define  CMD10009REALRESSERIALIZE_H

#include "CMDRequestSerialize.h"
/********************** µ ± ”∆µ5011************************************/
/*
<Message>
<TermIP>25.30.5.18</TermIP>
<SvrIP>25.30.5.18</SvrIP>
<SvrType>2</SvrType>
<VideoPort>4601</VideoPort>
<Ticket>Vm1VPxyIsHTZ3SO8Yru8tl8svZsCHG2</Ticket>
<StreamType></StreamType>
<Protocol>HIKMP4.2.5</Protocol>
</Message>
*/
/************************************************************************/
class CMD10009RealResSerialize : public CMDRequestSerialize
{
public:
    CMD10009RealResSerialize()
    {
    }
    ~CMD10009RealResSerialize()
    {

    }

    virtual std::string Serialize()
    {
		if (m_cmdHead.success != 0)
		{
			m_xml.AddElem("Information");
			m_xml.IntoElem();
			m_xml.AddElem("ErrDesctription", err_description);
		}

        m_cmdHead.cmdnum = CMD_DEVREALTIME_RESP;
        m_cmdHead.length = m_cmdHead.success != 0 ? m_xml.GetDoc().length() + 1 : 0;

        char buffer[1024] = {'\0'};
        memcpy(buffer, &m_cmdHead, sizeof(m_cmdHead));
		if (m_cmdHead.length)
			memcpy(buffer + sizeof(m_cmdHead), m_xml.GetDoc().c_str(), m_xml.GetDoc().length() + 1);
        std::string xml;
        xml.append(buffer, sizeof(m_cmdHead) + m_xml.GetDoc().length() + 1);
        return xml;
    }

    virtual void UnSerialize(const std::string& strMessage){}
public:
	std::string err_description;
};

#endif