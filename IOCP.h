#pragma once

#define MAX_WORKTHREAD_SIZE 8

typedef DWORD (CALLBACK *CB_WorkFuc)(LPVOID lpParameter);

namespace nsIOCP
{
	class CIOCP
	{
	public:
		CIOCP();
		~CIOCP();
		
	public:
		BOOL Start();
		void Release();
		void AddWorkFunc(CB_WorkFuc _lpfnWorkFuc);
		
	public:
		// 完成端口工作线程池
		static DWORD WINAPI _WorkerThreadProc(LPVOID lpParam);
		void _WorkerThread();
		
	private:
		BOOL CreatNewCompletionPort();
		
	private:
		HANDLE m_hWorkThread[MAX_WORKTHREAD_SIZE];
		DWORD  m_dwThreadId[MAX_WORKTHREAD_SIZE];
		HANDLE m_hIOCP;
		DWORD  m_dwWorkThreadSize;
		HANDLE m_hExitEvent;
	};
};

using namespace nsIOCP;