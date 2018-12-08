#include <windows.h>

//一段程序代码需要对某个资源进行同步保护，则这是一段关键段代码
//在进入该关键段代码前调用EnterCriticalSection函数，这样，其他线程都不能执行该段代码，若它们试图执行就会被阻塞。
//完成关键段的执行之后，调用LeaveCriticalSection函数，其他的线程就可以继续执行该段代码。
//如果该函数不被调用，则其他线程将无限期的等待。

class CSynch
{
public:
	CSynch(){::InitializeCriticalSection( &m_csLock );}
	~CSynch(){::DeleteCriticalSection( &m_csLock );}

	int EnterCS(void)
	{
		::EnterCriticalSection( &m_csLock );
		return 0;
	}
	int LeaveCS(void)
	{
		::LeaveCriticalSection( &m_csLock );
		return 0;
	}

private:
	CRITICAL_SECTION	m_lock;
}


class CAutoCS
{
public:
	CAutoCS(){};
	CAutoCS( CRITICAL_SECTION *v_CS )
	{
		m_pCS = v_CS;
		::EnterCriticalSection(m_pCS);
	}
	~CAutoCS()
	{
		::LeaveCriticalSection(m_pCS);
	}

private:
	CRITICAL_SECTION *m_pCS;
};
