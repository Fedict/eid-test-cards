//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef SOFTREADER_H
#define SOFTREADER_H

#include "APDURequest.h"
#include "Mutex.h"
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#define IN
#define OUT
#endif

namespace zetes { namespace pcscproxy {
				  class Transaction;
				  class SoftCard;
				  class SoftReader {
public:

				  SoftReader(const std::string& hardname, const std::string& softname);

				  ~SoftReader();

				  SoftCard* createSoftCard(SCARDHANDLE hardHandle);

				  const std::string& getHardReaderName() const;
				  const std::string& getSoftReaderName() const;
				  SoftCard* const getSoftCard(SCARDHANDLE softHandle) const;

				  LONG disconnect(SCARDHANDLE softHandle, DWORD dwDisposition);
				  LONG reconnect(DWORD dwInitialization);

private:
				  SoftReader();                           //do not implement
				  SoftReader(const SoftCard&);            //do not implement
				  SoftReader& operator=(const SoftCard&); //do not implement

				  std::vector<SoftCard *> itsSoftCards;

				  const std::string itsHardName, itsSoftName;

				  EidInfra::CMutex  srMutex;
				  }; // SoftReader
				  } //pcscproxy
} //zetes

#endif
