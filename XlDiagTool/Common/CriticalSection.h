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

	//���ƹ��캯������ʵ����Ĭ�ϸ��ƹ��캯����ǳ����
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
