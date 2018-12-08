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

		// 获取CPU核数
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		m_dwWorkThreadSize = sysinfo.dwNumberOfProcessors*4;

		if( m_dwWorkThreadSize > MAX_WORKTHREAD_SIZE )
		{// 工作线程上限为最大为MAX_WORKTHREAD_SIZE
			m_dwWorkThreadSize = MAX_WORKTHREAD_SIZE;
		}
	}

	CIOCP::~CIOCP()
	{
		Release();
	}

	BOOL CIOCP::Start()
	{
		/// 未启动，开始初始化创建完成端口
		//<<< 创建完成端口
		if( !CreatNewCompletionPort() )
		{
			return FALSE;
		}
		// 创建退出事件
		m_hExitEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
		if( m_hExitEvent == NULL )
		{
			return FALSE;
		}
		///<<< 创建I/O完成处理工作线程
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
			// 在关联到此完成端口的所有套节字上等待I/O完成
			BOOL bOK = ::GetQueuedCompletionStatus( m_hIOCP, &dwSize, (LPDWORD)&dwKey, (LPOVERLAPPED*)&lpol, INFINITE );

			if ( 0 == dwSize ) //请求释放资源
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

		// 清理线程
		for( DWORD i=0; i<m_dwWorkThreadSize;i++)
		{
			if( m_dwThreadId[i] == threadid && m_hWorkThread[i] != NULL )
			{
				::CloseHandle(m_hWorkThread[i]);
				m_hWorkThread[i] = NULL;
			}
		}
		if( ::InterlockedDecrement((LONG*)&m_dwWorkThreadSize) <= 0 )
		{// 所有工作线程都推出后才激活退出事件
			::SetEvent(m_hExitEvent);
		}
	}
};