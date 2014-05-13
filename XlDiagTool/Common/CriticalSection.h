#pragma once

class CriticalSection
{
public:
	CriticalSection(void)
	{
		::InitializeCriticalSection(&m_cs);
	}
	~CriticalSection(void)
	{
		::DeleteCriticalSection(&m_cs);
	}

	void Enter()
	{
		::EnterCriticalSection(&m_cs);
	}
	void Leave()
	{
		::LeaveCriticalSection(&m_cs);
	}

	struct Lock; 

protected:
	CRITICAL_SECTION m_cs;

	//复制构造函数，其实就是默认复制构造函数，浅拷贝
	CriticalSection(const CriticalSection&);  
	CriticalSection& operator=(const CriticalSection&); 
};

struct CriticalSection::Lock {  
	CriticalSection& s;  
	Lock(CriticalSection& s) : s(s) 
	{ 
		s.Enter(); 
	}  
	~Lock()               
	{ 
		s.Leave(); 
	}  
};  
