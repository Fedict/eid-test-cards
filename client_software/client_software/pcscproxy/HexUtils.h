//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef HEXUTILS_H
#define HEXUTILS_H

#ifndef _WIN32
#include <stddef.h>
#endif

namespace zetes { namespace pcscproxy {
				  class HexUtils {
public:
				  static int hexstring2hex(const char *hex, unsigned char *buf, size_t *len);
private:
				  HexUtils();                           //do not implement
				  ~HexUtils();                          //do not implement
				  HexUtils(const HexUtils&);            //do not implement
				  HexUtils& operator=(const HexUtils&); //do not implement
				  };
				  } //pcscproxy
} //zetes

#endif
