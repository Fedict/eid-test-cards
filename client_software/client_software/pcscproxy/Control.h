//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef CONTROL_H
#define CONTROL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#endif
#include <string>
#include "xml.h"
#include "Mutex.h"

using namespace std;
using namespace EidInfra;
namespace zetes { namespace pcscproxy {
#define MAX_FILE_PATH        500
#define CONTROL_FILE_SIZE    4096

				  enum ListReaderOrder { REAL_FIRST, REAL_LAST, HIDE_REAL, HIDE_VIRTUAL };

				  class Control : public Xml {
public:
				  Control();
				  Control(std::string file);
				  ~Control();

				  std::string getSoftCardFileLocation(const BYTE* const chipNr) const;
				  ListReaderOrder getListReaderOrder();
				  string GetVirtualFilePath(const char *hardchipnr);
				  char * GetFilePath()
				  {
					  return filePath;
				  }
				  void GetFileSize(const char * filePath);
private:
				  EidInfra::CMutex controlMutex;
				  char             filePath[MAX_FILE_PATH];
				  size_t           fileSize;
				  char             * fileBuffer;
				  ListReaderOrder  itsListReaderOrder;
				  Control(const Control&);            //do not implement
				  Control& operator=(const Control&); //do not implement
				  void getVisibility();
				  };
				  } //pcscproxy
}                   //zetes

#endif
