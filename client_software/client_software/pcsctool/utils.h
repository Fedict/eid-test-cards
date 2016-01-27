#include <stdio.h>
#include <stdlib.h>
#include <string>

int hex2bin(const char *hex, unsigned char *buf, size_t *len);
void dumphex(const char *msg, const unsigned char *buf, size_t len);

void delayMS(int x);

void StoreAPDUs(const char *chipNr, int idx,
	const unsigned char *sendBuf, size_t sendBufLen,
	const unsigned char *recvBuf, size_t recvBufLen);
int compareAPDUS(const char *cmdApdu, const unsigned char *respApdu, size_t respApduLen,
	const char *chipnrForCompare, int idx);

#define CHECK_PCSC_RET(func, ret) \
	if (SCARD_S_SUCCESS != ret)	  \
		printf("ERR line %d: %s returned 0x%0x (=%d) (%s)\n", __LINE__, func, (unsigned int) ret, (unsigned int) ret, pcscerr2string(ret));

#define CHECK_PCSC_RET_PASS(toReturn) \
	if (SCARD_S_SUCCESS != ret)		  \
		returnValue = toReturn;

#define CHECK_PCSC_RET_FAIL(toReturn) \
	if (SCARD_S_SUCCESS == ret)		  \
		returnValue = toReturn;

