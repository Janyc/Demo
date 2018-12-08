#include "stdafx.h"
#include "BufPool.h"
#include <assert.h>

//m_uiBufCount:�ڴ浥Ԫ����
//m_uiBufSize:�ڴ浥Ԫ��С
CBufPool::CBufPool(UINT _uiBufCount,UINT _uiBufSize) 
	: m_uiBufCount(_uiBufCount),m_uiBufSize(_uiBufSize)
{
    Init();
}

CBufPool::~CBufPool(void)
{
	Terminate();
}

//�������һ���ڴ浥Ԫ
//����ֵ������ɹ�������ָ������bufָ�룬���򷵻�NULL
PCHAR CBufPool::GetFreeBuf()
{
	BufNode* v_pBufNode;
    PCHAR v_pRet = NULL;
	UINT v_i;

    CAutoCS autoCS(&m_criBufPool);
	try
	{
		v_pBufNode=m_pBufNodeHead;
		for(v_i=0;v_i<m_uiBufCount;v_i++)
		{
			if(v_pBufNode[v_i].bUsed==FALSE)
			{
				v_pBufNode[v_i].bUsed=TRUE;
				return v_pBufNode[v_i].pBuffer;
			}
		}
	}
	catch(...)
	{

	}
    return NULL;
}

//�ͷ�һ���ڴ浥Ԫ
void CBufPool::ReleaseBuf(PCHAR pBuf)
{
	BufNode* v_pBufNode;
	BufAddress v_BufAddress1;
	BufAddress v_BufAddress2;
	UINT v_i;
    if (pBuf==NULL)
    {
        return;
    }

    CAutoCS autoCS(&m_criBufPool);
	try
	{
		v_pBufNode=m_pBufNodeHead;
		for(v_i=0;v_i<m_uiBufCount;v_i++)
		{
			if(v_pBufNode[v_i].bUsed==TRUE)
			{
				v_BufAddress1.Address.pAddress=v_pBufNode[v_i].pBuffer;
				v_BufAddress2.Address.pAddress=pBuf;
				if(v_BufAddress1.Address.dwAddress==v_BufAddress2.Address.dwAddress)
				{
					v_pBufNode[v_i].bUsed=FALSE;
					return;
				}
			}
		}
	}
	catch(...)
	{	
	}
}
//////////////////////////////////////////////////////////////////////////

void CBufPool::Init()
{
	InitializeCriticalSection(&m_criBufPool);
	//�жϲ����Ƿ�Ϸ�
    assert(m_uiBufCount);
	assert(m_uiBufSize);
	m_pVirtualAddress=(BufNode*)VirtualAlloc(NULL, m_uiBufCount*m_uiBufSize, MEM_COMMIT, PAGE_READWRITE);
	m_pBufNodeHead=(BufNode*)VirtualAlloc(NULL, m_uiBufCount*sizeof(BufNode), MEM_COMMIT, PAGE_READWRITE);
	for(UINT v_i=0;v_i<m_uiBufCount;v_i++)
	{
		m_pBufNodeHead[v_i].bUsed=FALSE;
		m_pBufNodeHead[v_i].pBuffer=((PCHAR)m_pVirtualAddress)+m_uiBufSize*v_i;
	}
}

void CBufPool::Terminate()
{
	PCHAR		v_pBuff=NULL;
	//�ͷŻ���
	{
		CAutoCS autoCS(&m_criBufPool);;
		try
		{
			VirtualFree(m_pBufNodeHead, 0, MEM_RELEASE);
			VirtualFree(m_pVirtualAddress, 0, MEM_RELEASE);
		}
		catch(...)
		{
			
		}
	}
    DeleteCriticalSection(&m_criBufPool);
}
