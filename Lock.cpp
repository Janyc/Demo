// Lock.cpp: implementation of the CLock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLock::CLock( ILockableObject * pLockable ) : m_pLock(pLockable)
{
    m_pLock->Lock();
}


CLock:: ~CLock()
{
    m_pLock->UnLock();
}


CLockGuard::CLockGuard( ILockableObject * pLockable ) : m_pLock(pLockable), m_owner(false)
{
    acquire();
}

CLockGuard:: ~CLockGuard()
{
    release();
}
