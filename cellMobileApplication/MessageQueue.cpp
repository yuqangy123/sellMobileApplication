#include "stdafx.h"
#include "MessageQueue.h"

DECLARE_SINGLETON_MEMBER(CYMessageQueue);
CYMessageQueue::CYMessageQueue()
{

}
CYMessageQueue::~CYMessageQueue()
{
	for (	std::map<int,CNotifyEventEx1*>::iterator Itr  = m_MsgList.begin(); Itr != m_MsgList.end(); ++Itr)
	{
		delete Itr->second;
	}
}

void CYMessageQueue::RegisterMsg(CUSTOM_MESSAGE_ID nID, const CNotifyEventEx1& NotifyEvent)
{
	if(m_MsgList.find(nID) != m_MsgList.end())
		delete m_MsgList[nID];

	CNotifyEventEx1* pNotifyEvent =new CNotifyEventEx1;
	if(pNotifyEvent)
	{
		*pNotifyEvent = NotifyEvent;
		m_MsgList[nID] = pNotifyEvent;
	}
};

void CYMessageQueue::UnRegisterMsg(CUSTOM_MESSAGE_ID nID, const CNotifyEventEx1& NotifyEvent)
{
	std::map<int,CNotifyEventEx1*>::iterator delItr = m_MsgList.find(nID);
	if(delItr != m_MsgList.end())
	{
		delete m_MsgList[nID];
		m_MsgList.erase(delItr);
	}
}

void CYMessageQueue::SendMsg(CUSTOM_MESSAGE_ID nID, void* param1)
{
	if(m_MsgList.find(nID) != m_MsgList.end())
	{
		CNotifyEventEx1* pNotifyEvent = m_MsgList[nID];
		pNotifyEvent->Notify(param1);
	}
}

void CYMessageQueue::Clear()
{
	m_MsgList.clear();
}