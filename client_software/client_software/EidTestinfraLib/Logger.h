#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <winscard.h>
#include <string>

namespace EidInfra
{
/**
 * Log info.
 * E.g. logInfo("SCardConnect(%s) called\n", readerName);
 */
void logInfo(const char *format, ...);

/**
 * Log an error.
 * E.g. logError("Can't open config file: err = 0x%0x (%d)\n", err, err);
 */
void logError(const char *format, ...);

/**
 * Log the SCardTransmit() command.
 */
void logTransmitSend(bool bSr, SCARDHANDLE hCard,
	const BYTE *pbSendBuffer, DWORD cbSendLength);

/**
 * Log the SCardTransmit() response.
 */
void logTransmitRecv(bool bSr, SCARDHANDLE hCard,
	LONG result, const BYTE *pbRecvBuffer, DWORD cbRecvLength);

/**
 * Return the current date/time as a string.
 * E.g. logGetTimeStr("%Y-%m-%d %H:%M:%S")
 */
std::string logGetTimeStr(const char *format);
}

#endif
