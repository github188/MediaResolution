#include "stdafx.h"
#include "ServerTask.h"
#include "NetUtility.h"
#include "MegaeyesHead.h"
#include "VDeviceManager.h"
using ty::MsgHander;

int CMsgTask::onReadable(Operator &op)
{
    CmdHead head;

    if(!NetUtility::Receive(op.hand->_socket,(char*)&head, sizeof(head)))
    {   
        return -1;
    }

    char pBuffer[8*1024];

    if(!NetUtility::Receive(op.hand->_socket,(char*)pBuffer,head.length))
    {   
        return -1;
    }

    return VDeviceManager::Instance().OnRecvRequest(head, (const unsigned char *)pBuffer, op.hand->_socket);
}
