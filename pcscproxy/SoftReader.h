/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2016 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */
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

namespace fedict { namespace pcscproxy {
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
} //fedict

#endif
