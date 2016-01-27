/* ****************************************************************************
$$$_ZETES_LICENSE_MUST_BE_HERE_BEFORE_DISTRIBUTING_SOURCES_$$$
**************************************************************************** */
#include "Mutex.h"

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

namespace EidInfra
{

CMutex::CMutex()
{
#ifdef WIN32
	InitializeCriticalSection(&m_Mutex);
#else
	pthread_mutex_init(&m_Mutex, NULL);
	m_MutexLockcount = 0;
	m_MutexOwner = 0;
#endif
}

CMutex::~CMutex()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
	LeaveCriticalSection(&m_Mutex);
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
#endif
}

void CMutex::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&m_Mutex);
#else
	if (pthread_mutex_trylock(&m_Mutex))
	{
		if (! pthread_equal( m_MutexOwner, pthread_self()  ) )
		{
			pthread_mutex_lock(&m_Mutex);
			m_MutexOwner = pthread_self();
			m_MutexLockcount++;
		}
		else 
		{
			m_MutexLockcount++;
		}
	}
	else
	{	
		m_MutexOwner = pthread_self();
		m_MutexLockcount++;
	}
#endif
}

void CMutex::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&m_Mutex);
#else
	if ( pthread_equal( m_MutexOwner, pthread_self()  ) )
	{
		if( m_MutexLockcount > 1 )
		{
			m_MutexLockcount--;
		}
		else
		{
			m_MutexOwner = 0;
			m_MutexLockcount--;
			pthread_mutex_unlock(&m_Mutex);
		}
	}
#endif
}

}
