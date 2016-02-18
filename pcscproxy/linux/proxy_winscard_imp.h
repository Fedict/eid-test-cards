#ifndef __ZVLZIXMZUJGKBQIQKPSGMMPUKULEKVZE__
#define __ZVLZIXMZUJGKBQIQKPSGMMPUKULEKVZE__

#include "proxy_winscard.h"

extern const char *REAL_LIBS[];

void proxy_init();
void proxy_exit();

// Use #undef to let the function go to the lib to be proxy-ed,
// use #define to let the function go to the proxy lib (= this lib))
// For example:
//     #undef USE_PROXY_LIB_FOR_SCardEstablishContext  -> calls to SCardEstablishContext will go to pSCardEstablishContext
//     #define USE_PROXY_LIB_FOR_SCardEstablishContext  -> calls to SCardEstablishContext will go to imp_SCardEstablishContext

#undef USE_PROXY_LIB_FOR_SCardEstablishContext
#undef USE_PROXY_LIB_FOR_SCardReleaseContext
#undef USE_PROXY_LIB_FOR_SCardIsValidContext
#define USE_PROXY_LIB_FOR_SCardConnect
#define USE_PROXY_LIB_FOR_SCardReconnect
#define USE_PROXY_LIB_FOR_SCardDisconnect
#define USE_PROXY_LIB_FOR_SCardBeginTransaction
#define USE_PROXY_LIB_FOR_SCardEndTransaction
#define USE_PROXY_LIB_FOR_SCardStatus
#define USE_PROXY_LIB_FOR_SCardGetStatusChange
#define USE_PROXY_LIB_FOR_SCardControl
#define USE_PROXY_LIB_FOR_SCardTransmit
#undef USE_PROXY_LIB_FOR_SCardListReaderGroups
#define USE_PROXY_LIB_FOR_SCardListReaders
#undef USE_PROXY_LIB_FOR_SCardFreeMemory
#undef USE_PROXY_LIB_FOR_SCardCancel
#define USE_PROXY_LIB_FOR_SCardGetAttrib
#undef USE_PROXY_LIB_FOR_SCardSetAttrib

#endif  // __ZVLZIXMZUJGKBQIQKPSGMMPUKULEKVZE__
