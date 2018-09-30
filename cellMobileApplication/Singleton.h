#pragma once

#define DECLARE_SINGLETON_MEMBER(_Ty)\
	template<>\
	_Ty* CSingleton<_Ty>::m_instance=NULL;


template<class T>
class CSingleton
{
public:
	static inline T* Instance();
	static inline T& InstanceEx();
	static void CloseInstance();
	CSingleton();
	virtual ~CSingleton();

private:
	static T* m_instance;
};

template<class T>
CSingleton<T>::CSingleton()
{
//	ASSERT(NULL == m_instance);
	m_instance = (T*)((int)this + ((int)(T*)1 - (int)(CSingleton<T>*)(T*)1));
}

template<class T>
CSingleton<T>::~CSingleton()
{
}

template<class T>
inline T* CSingleton<T>::Instance()
{
	if(!m_instance)
		m_instance = new T;
	return m_instance;
}
template<class T>
inline T& CSingleton<T>::InstanceEx()
{
	if(!m_instance)
		m_instance = new T;
	return *m_instance;
}

template<class T>
void CSingleton<T>::CloseInstance()
{
	if(m_instance)
		delete m_instance;
	m_instance=(void*)0;
}