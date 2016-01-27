//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef TRANSACTION_H
#define TRANSACTION_H

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#include <sys/types.h>
#endif

namespace zetes { namespace pcscproxy {
				  class Transaction {
public:
				  Transaction(unsigned long scardHandle);
				  void aquire();
				  void release();
				  bool justStarted();
				  ~Transaction();
private:
				  size_t cnt;
				  Transaction();                              //do not implement
				  Transaction(const Transaction&);            //do not implement
				  Transaction& operator=(const Transaction&); //do not implement
				  int hash(const char* str);
				  bool            isJustStarted;
		#ifdef _WIN32
				  HANDLE          hMutex;
		#else
				  pthread_mutex_t *hMutex;
				  int             shmid;
		#endif
				  };
				  } //pcscproxy
}                   //zetes

#endif
