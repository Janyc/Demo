#pragma once

class CBufPool	//线程安全的内存池类
{
public:
	struct BufNode
	{
		BOOL bUsed;
		PCHAR pBuffer;
	};
	struct BufAddress
	{
		union
		{
			DWORD dwAddress;
			PCHAR pAddress;
		}Address;
	};
    CBufPool(UINT _uiBufCount,UINT _uiBufSize);
    ~CBufPool(void);
    PCHAR GetFreeBuf();
    void ReleaseBuf(PCHAR pBuf);
    
private:
	void Terminate();
	void Init();
	LPVOID					m_pVirtualAddress;	//虚拟内存地址
	BufNode*				m_pBufNodeHead;		//当前所有内存队列
    UINT					m_uiBufCount;       //缓存总数
	UINT					m_uiBufSize;		//缓存单元大小
    CRITICAL_SECTION		m_criBufPool;       //缓存临界区
	
};
