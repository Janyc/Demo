#include <windows.h>

//һ�γ��������Ҫ��ĳ����Դ����ͬ��������������һ�ιؼ��δ���
//�ڽ���ùؼ��δ���ǰ����EnterCriticalSection�����������������̶߳�����ִ�иöδ��룬��������ͼִ�оͻᱻ������
//��ɹؼ��ε�ִ��֮�󣬵���LeaveCriticalSection�������������߳̾Ϳ��Լ���ִ�иöδ��롣
//����ú����������ã��������߳̽������ڵĵȴ���

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
