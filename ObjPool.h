// ObjPool.h: interface for the CObjPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJPOOL_H__0373B6F6_4C1A_4D08_B88E_EC8BF227D24F__INCLUDED_)
#define AFX_OBJPOOL_H__0373B6F6_4C1A_4D08_B88E_EC8BF227D24F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//线程安全的对象池模板类
template<class TYPE, class ARG_TYPE>
class CObjPool
{
public:
	int GetLstMemCount();
	int GetLstFreeCount();

	TYPE GetFreeBuf();
    void ReleaseBuf(TYPE _pBuf);

	CObjPool(UINT _uiBufCount);
	virtual ~CObjPool();
	    
private:
	CList<TYPE,TYPE&>	m_lstFree;			//空闲内存队列
	CList<TYPE,TYPE&>	m_lstMem;			//当前所有内存队列
    UINT				m_uiBufCount;       //缓存总数
    CRITICAL_SECTION	m_criBufPool;       //缓存临界区
};

template<class TYPE, class ARG_TYPE>
CObjPool<TYPE,ARG_TYPE>::CObjPool(UINT _uiBufCount)
{
    InitializeCriticalSection(&m_criBufPool);
	m_uiBufCount=_uiBufCount;
	m_lstMem.RemoveAll();
	m_lstFree.RemoveAll();	
}

template<class TYPE, class ARG_TYPE>
CObjPool<TYPE,ARG_TYPE>::~CObjPool()
{
	TYPE	v_pBuff=NULL;
	POSITION	v_pos=NULL;
	//释放缓存
    EnterCriticalSection(&m_criBufPool);
	m_lstFree.RemoveAll();
	if (!m_lstMem.IsEmpty())
	{
		v_pos=m_lstMem.GetHeadPosition();
		while (v_pos)
		{
			v_pBuff=m_lstMem.GetNext(v_pos);
			if (v_pBuff!=NULL)
			{
				delete	v_pBuff;
				v_pBuff=NULL;
			}
		}
		m_lstMem.RemoveAll();
	}
    LeaveCriticalSection(&m_criBufPool);
	
    DeleteCriticalSection(&m_criBufPool);
}

//请求分配一个内存单元
//返回值：分配成功，返回指向分配的buf指针，否则返回NULL
template<class TYPE, class ARG_TYPE>
TYPE CObjPool<TYPE,ARG_TYPE>::GetFreeBuf()
{
    TYPE v_pRet = NULL;
    EnterCriticalSection(&m_criBufPool);
    if (!m_lstFree.IsEmpty())
    {
		v_pRet=m_lstFree.RemoveHead();
    }
	else
	{
		if ((UINT)m_lstMem.GetCount()<m_uiBufCount)
		{
			v_pRet=new ARG_TYPE();
			if (v_pRet!=NULL)
			{
				m_lstMem.AddTail(v_pRet);
			}
		}
	}	
    LeaveCriticalSection(&m_criBufPool);
    return	v_pRet;
}

//释放一个内存单元
template<class TYPE, class ARG_TYPE>
void CObjPool<TYPE,ARG_TYPE>::ReleaseBuf(TYPE _pBuf)
{
    EnterCriticalSection(&m_criBufPool);
	if (_pBuf!=NULL)//&&(m_lstMem.Find(_pBuf)!=NULL))
	{
		m_lstFree.AddTail(_pBuf);
	} 
    LeaveCriticalSection(&m_criBufPool);
}

template<class TYPE, class ARG_TYPE>
int CObjPool<TYPE,ARG_TYPE>::GetLstFreeCount()
{
	return m_lstFree.GetCount();
}


template<class TYPE, class ARG_TYPE>
int CObjPool<TYPE,ARG_TYPE>::GetLstMemCount()
{
	return m_lstMem.GetCount();
}
//////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_OBJPOOL_H__0373B6F6_4C1A_4D08_B88E_EC8BF227D24F__INCLUDED_)
