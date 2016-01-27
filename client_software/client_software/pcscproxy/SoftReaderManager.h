//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef SOFTREADERMANAGER_H
#define SOFTREADERMANAGER_H

#include <util.h>
#include <vector>
#include "SoftCard.h"
#include "APDURequest.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#define IN
#define OUT
#endif

namespace zetes { namespace pcscproxy {
				  enum T_HANDLE { H_SOFT=1, H_HARD=2, H_SOFT_OR_HARD=3 };
				  enum T_NAME { N_SOFT=1, N_HARD=2, N_SOFT_OR_HARD=3 };
				  static const std::string SOFTREADER_NAME_PREFIX("!Virtual ");

				  class SoftReader;
				  class Control;
				  class SoftReaderManager {
public:

				  SoftReaderManager();

				  ~SoftReaderManager();

				  size_t createSoftReaders(LPTSTR mszReaders, LPDWORD pcchReaders);

				  SoftReader* const getSoftReaderByName(LPCTSTR szReader, T_NAME = N_SOFT) const;

				  SoftCard* const getSoftCard(SCARDHANDLE hCard) const;

				  const Control* const getControl() const;

				  static DWORD calcListReadersLength(LPCTSTR mszReaders, DWORD pcchReaders);

private:

				  static size_t takeFromMultiString(LPCTSTR mszReaders, DWORD len, std::vector<std::string>&);
				  static bool cmpVirtualNameFirst(const std::string&, const std::string&);
				  static bool cmpVirtualNameLast(const std::string&, const std::string&);

				  Control *itsControl;
				  SoftReaderManager(const SoftReaderManager&);            //do not implement
				  SoftReaderManager& operator=(const SoftReaderManager&); //do not implement

				  std::vector<SoftReader*> readers;
				  };
				  } //pcscproxy
}                   //zetes

#endif
