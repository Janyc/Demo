#include "stdafx.h"
#include "IOCP.h"

namespace nsIOCP
{
	CIOCP::CIOCP()
	{
		m_hIOCP = NULL;
		m_hExitEvent = NULL;
		::memset(m_hWorkThread,0x00,sizeof(m_hWorkThread));
		::memset(m_dwThreadId,0x00,sizeof(m_dwThreadId));

		// ��ȡCPU����
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		m_dwWorkThreadSize = sysinfo.dwNumberOfProcessors*4;

		if( m_dwWorkThreadSize > MAX_WORKTHREAD_SIZE )
		{// �����߳�����Ϊ���ΪMAX_WORKTHREAD_SIZE
			m_dwWorkThreadSize = MAX_WORKTHREAD_SIZE;
		}
	}

	CIOCP::~CIOCP()
	{
		Release();
	}

	BOOL CIOCP::Start()
	{
		/// δ��������ʼ��ʼ��������ɶ˿�
		//<<< ������ɶ˿�
		if( !CreatNewCompletionPort() )
		{
			return FALSE;
		}
		// �����˳��¼�
		m_hExitEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		if( m_hExitEvent == NULL )
		{
			return FALSE;
		}
		///<<< ����I/O��ɴ������߳�
		for( int i=0; i<(int)m_dwWorkThreadSize;i++ )
		{
			m_hWorkThread[i] = ::CreateThread(NULL, 0, _WorkerThreadProc, this, 0, &m_dwThreadId[i]);
		}

		return TRUE;
	}

	void CIOCP::Release()
	{
		for( int i = 0; i < (int)m_dwWorkThreadSize; i++ )
		{
			::PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
		}

		if( WAIT_TIMEOUT == WaitForSingleObject(m_hExitEvent,1000) )
		{
			for( int i = 0; i < (int)m_dwWorkThreadSize; i++ )
			{
				if( m_hWorkThread[i]!= NULL )
				{
					::TerminateThread(m_hWorkThread[i],0);;
					::CloseHandle(m_hWorkThread[i]);
					m_hWorkThread[i] = NULL;
				}
			}

			if( m_hExitEvent != NULL )
			{
				CloseHandle(m_hExitEvent);
				m_hExitEvent = NULL;
			}
		}
	}

	BOOL CIOCP::CreatNewCompletionPort()
	{
		m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, m_dwWorkThreadSize);
		if( m_hIOCP != NULL )
		{
			return TRUE;
		}

		return FALSE;
	}

	void CIOCP::AddWorkFunc(CB_WorkFuc _lpfnWorkFuc)
	{
		::PostQueuedCompletionStatus( m_hIOCP, 65536, (DWORD)_lpfnWorkFuc, NULL );
	}

	DWORD WINAPI CIOCP::_WorkerThreadProc(LPVOID lpParam)
	{
		CIOCP* pThis = (CIOCP*)lpParam;

		if( pThis != NULL )
		{
			pThis->_WorkerThread();
		}

		::ExitThread(0);

		return 0;
	}

	void CIOCP::_WorkerThread()
	{
		DWORD dwKey = 0;
		DWORD dwSize = 0;
		DWORD dwResult = 0;
		LPOVERLAPPED lpol;

		DWORD threadid= GetCurrentThreadId();

		while ( TRUE )
		{
			// �ڹ���������ɶ˿ڵ������׽����ϵȴ�I/O���
			BOOL bOK = ::GetQueuedCompletionStatus( m_hIOCP, &dwSize, (LPDWORD)&dwKey, (LPOVERLAPPED*)&lpol, INFINITE );

			if ( 0 == dwSize ) //�����ͷ���Դ
			{
				break;
			}
			else if( 65536 == dwSize )
			{
				CB_WorkFuc _fpWorFunc = (CB_WorkFuc)dwKey;
				if(_fpWorFunc)
				{
					_fpWorFunc(NULL);
				}
			}
		}

		// �����߳�
		for( DWORD i=0; i<m_dwWorkThreadSize;i++)
		{
			if( m_dwThreadId[i] == threadid && m_hWorkThread[i] != NULL )
			{
				::CloseHandle(m_hWorkThread[i]);
				m_hWorkThread[i] = NULL;
			}
		}
		if( ::InterlockedDecrement((LONG*)&m_dwWorkThreadSize) <= 0 )
		{// ���й����̶߳��Ƴ���ż����˳��¼�
			::SetEvent(m_hExitEvent);
		}
	}
};