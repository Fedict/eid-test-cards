#include "Transaction.h"
#ifndef _WIN32
#include <cstring>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#endif
#include <time.h>
#include <string.h>
#include <stdio.h>

using namespace zetes::pcscproxy;

Transaction::Transaction(unsigned long scardHandle) : cnt(0), isJustStarted(false)
{
	char name[70];
	sprintf(name, "%u%u", scardHandle, (unsigned long) clock());

	#ifdef _WIN32
	hMutex = CreateMutex(NULL, false, name);
	#else
	key_t key = hash(name);
	shmid = shmget(key, sizeof(pthread_mutex_t), 0644 | IPC_CREAT);
	int   *shm_ptr = (int*) shmat(shmid, (void*) 0, 0);
	hMutex = (pthread_mutex_t*) shm_ptr;
	if (0 == pthread_mutex_trylock(hMutex))
	{
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(hMutex, &attr);
		pthread_mutex_unlock(hMutex);
	}
	#endif
}

Transaction::~Transaction()
{
	#ifdef _WIN32
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
	#else
	shmdt(hMutex);
	shmctl(shmid, IPC_RMID, NULL);
	#endif
}

void Transaction::aquire()
{
	if (0 == cnt++)
	{
		#ifdef _WIN32
		WaitForSingleObject(hMutex, INFINITE);
		#else
		pthread_mutex_lock(hMutex);
		#endif
		isJustStarted = true;
	}
}

void Transaction::release()
{
	if (cnt > 0)
		cnt--;
	if (0 == cnt)
	{
		isJustStarted = false;
		#ifdef _WIN32
		ReleaseMutex(hMutex);
		#else
		pthread_mutex_unlock(hMutex);
		#endif
	}
}

int Transaction::hash(const char* str)
{
	unsigned long hash = 5381;
	char          c;
	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c; // hash*33 + c
	}
	return hash;
}

/**
 * Returns true the first time this method is called after an aquire(),
 * false the next times this method is called.
 */
bool Transaction::justStarted()
{
	bool ret = isJustStarted;
	isJustStarted = false;
	return ret;
}


