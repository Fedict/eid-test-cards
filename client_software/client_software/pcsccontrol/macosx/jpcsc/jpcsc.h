#ifndef _jpcsc_h
#define _jpcsc_h

#include <stdio.h>
#include <assert.h>
#include <jni.h>

#ifdef WIN32
#include <winscard.h>
#else
#include <wintypes.h>
#include <winscard.h>
#endif

#define GEN_FUNCNAME(FUNCNAME) Java_com_linuxnet_jpcsc_##FUNCNAME
#define PCSC_EX_CLASSNAME "com/linuxnet/jpcsc/PCSCException"

/*
 * Length of ATR.
 */
#define JPCSC_ATR_SIZE SCARD_ATR_LENGTH 

/*
 * Name of NullPointerException
 */
#define NP_EX_CLASSNAME "java/lang/NullPointerException"


#ifndef WIN32
#if defined(SCARD_ATTR_VALUE)
#define HAVE_SCARD_ATTRIBUTES
#endif /* SCARD_ATTR_VALUE */
#endif /* WIN32 */

#endif /* _jpcsc_h */
