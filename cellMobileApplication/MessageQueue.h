#pragma once
#include <map>
#include "Singleton.h"
#include "Node.h"


typedef enum{
	MSG_ID_MESSAGENAME = 0,

	SHOW_TIPS_DIALOG,

	MSG_ID_MAX
}CUSTOM_MESSAGE_ID;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//可以扩展到多个参数
#define CNotifyEventEx1 CNotifyEvent<CNode, void*>

#define CreateEventEx1(NotifyFunc) CNotifyEventEx1(this, static_cast<CNotifyEventEx1::pNotifyFunc>(NotifyFunc))

template<class classType, class pParam>
class CNotifyEvent
{
public:
	typedef int (classType::*pNotifyFunc)(pParam lParam);
	CNotifyEvent(classType* Owner=NULL, const pNotifyFunc pFun=NULL);
	void Notify(pParam nParam);
	void operator=(const CNotifyEvent &pEvent);

private:
	pNotifyFunc m_pFunc;
	classType* m_pOwner;
};

#define MessageQueue CYMessageQueue::InstanceEx()
class CYMessageQueue
	:public CSingleton<CYMessageQueue>
{
public:
	CYMessageQueue();
	~CYMessageQueue();

	void RegisterMsg(CUSTOM_MESSAGE_ID nID, const CNotifyEventEx1& NotifyEvent);
	void UnRegisterMsg(CUSTOM_MESSAGE_ID nID, const CNotifyEventEx1& NotifyEvent);
	void SendMsg(CUSTOM_MESSAGE_ID nID, void* param1);
	void Clear();

private:
	std::map<int,CNotifyEventEx1*>	m_MsgList;
};


template<class classType, class param>
CNotifyEvent<classType, param>::CNotifyEvent(classType *Owner/*=NULL*/, const pNotifyFunc pFun/*=NULL*/)
{
	m_pOwner = Owner;
	m_pFunc = pFun;
}

template<class classType, class param>
void CNotifyEvent<classType, param>::Notify(param nParam)
{
	if(m_pOwner && m_pFunc)
		(m_pOwner->*m_pFunc)(nParam);
}

template<class classType, class param>
void CNotifyEvent<classType, param>::operator=(const CNotifyEvent &pEvent)
{
	this->m_pOwner = pEvent.m_pOwner;
	this->m_pFunc = pEvent.m_pFunc;
}
