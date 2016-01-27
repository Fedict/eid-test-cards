//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef APDUREQUEST_H
#define APDUREQUEST_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#endif

namespace zetes { namespace pcscproxy {
				  class APDURequest {
public:
				  static APDURequest createAPDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength);

				  APDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength);
				  ~APDURequest();

				  enum TYPE { GET_CARD_DATA, SELECT_FILE, READ_BINARY, GET_RESPONSE, OTHER };

				  LPCBYTE getBuffer() const;
				  DWORD size() const;

				  bool isGetCardData() const;
				  bool isSelectFile() const;
				  bool isReadBinary() const;
				  bool isGetResponse() const;

				  TYPE getType() const;

private:
				  APDURequest(); //Do not implement

				  TYPE    t;

				  LPCBYTE buffer;
				  DWORD   length;
				  };
				  } //pcscproxy
}                   //zetes

#endif
