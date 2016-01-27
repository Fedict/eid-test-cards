#include "Logger.h"
#include "Mutex.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

namespace EidInfra
{
// Logging is done only if this dir exists
#ifdef _WIN32
static const char logFileName[] = "C:\\Temp\\pcscproxy\\log.txt";
#else
static const char logFileName[] = "/tmp/pcscproxy/log.txt";
#endif

//////////////////////// CLOG class declaration ///////////////////////////////

class CLOG
{
public:
CLOG();
~CLOG();
bool init();
void log(const char *type, const char *format, va_list args);
void log2(const char *type, const char *format, ...);
private:
bool   initIsOK;
CMutex mutex;
};

CLOG logger;

/////////////////////// Exported functions ////////////////////////////////////

void logInfo(const char *format, ...)
{
	if (logger.init())
	{
		va_list args;
		va_start(args, format);
		try {
			logger.log("", format, args);
		}
		catch (...) {
			va_end(args);
		}
		va_end(args);
	}
}

void logError(const char *format, ...)
{
	if (logger.init())
	{
		va_list args;
		va_start(args, format);
		try {
			logger.log("ERR: ", format, args);
		}
		catch (...) {
			va_end(args);
		}
		va_end(args);
	}
}

void logTransmitSend(bool bSr, SCARDHANDLE hCard,
	const BYTE *pbSendBuffer, DWORD cbSendLength)
{
	if (logger.init())
	{
		try {
			DWORD maxlen = (cbSendLength < 258) ? cbSendLength : 258;
			// In case of PIN commands, don't log the PIN data
			if (cbSendLength > 2 && (0x20 == pbSendBuffer[1] || 0x24 == pbSendBuffer[1] || 0x2C == pbSendBuffer[1]))
				maxlen = (maxlen > 5) ? 5 : maxlen;
			const char *format = bSr ?
				"SCardTransmit(soft_hCard=%d, %s)\n" :
				"SCardTransmit(real_hCard=%d, %s)\n";
			char hex[3 * 258 + 2];
			hex[0] = '\0';
			for (DWORD i = 0; i < maxlen; i++)
				sprintf(hex + 3 * i, "%02x ", pbSendBuffer[i]);
			logger.log2("DEBUG: ", format, hCard, hex);
		}
		catch (...) {
		}
	}
}

void logTransmitRecv(bool bSr, SCARDHANDLE hCard,
	LONG result, const BYTE *pbRecvBuffer, DWORD cbRecvLength)
{
	if (logger.init())
	{
		try {
			DWORD maxlen = (cbRecvLength < 258) ? cbRecvLength : 258;
			const char *format = bSr ?
				"SCardTransmit(soft_hCard=%d): res = 0x%0x (%d), resp = %s\n" :
				"SCardTransmit(real_hCard=%d): res = 0x%0x (%d), resp = %s\n";
			char hex[3 * 258 + 2];
			if (0 == result) {
				for (DWORD i = 0; i < maxlen; i++)
					sprintf(hex + 3 * i, "%02x ", pbRecvBuffer[i]);
			}
			else
				hex[0] = '\0';
			logger.log2("DEBUG: ", format, hCard, result, result, hex);
		}
		catch (...) {
		}
	}
}

std::string logGetTimeStr(const char *format)
{
	time_t    rawtime;
	struct tm timeinfo;
	char      timeBuf [20];

	time(&rawtime);
#ifdef WIN32
	localtime_s(&timeinfo, &rawtime);
#else
	timeinfo = *(localtime(&rawtime));
#endif
	strftime(timeBuf, 20, format, &timeinfo);

	return timeBuf;
}

//////////////////////// CLOG class implementation ////////////////////////////

CLOG::CLOG()
{
	// Init is done in the init() function
	initIsOK = false;
}

CLOG::~CLOG()
{
	if (initIsOK)
	{
		CAutoMutex autoMutex(&mutex);
		initIsOK = false;

		// TODO: cleanup
	}
}

bool CLOG::init()
{
	if (!initIsOK)
	{
		CAutoMutex autoMutex(&mutex);

		// Check if we can open/create the log file for writing to it
		FILE *f  = NULL;
		int  err = fopen_s(&f, logFileName, "a");
		if (err == 0 && f != NULL)
		{
			fclose(f);

			// TODO

			initIsOK = true;
		}
	}

	return initIsOK;
}

void CLOG::log(const char *type, const char *format, va_list args)
{
	CAutoMutex autoMutex(&mutex);

	if (initIsOK)
	{
		FILE *f  = NULL;
		int  err = fopen_s(&f, logFileName, "a");
		if (err == 0 && f != NULL)
		{
			if (type != NULL && strlen(type) != 0)
				fprintf_s(f, "%s", type);
			vfprintf_s(f, format, args);

			fclose(f);
		}
	}
}

void CLOG::log2(const char *type, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	try {
		log(type, format, args);
	}
	catch (...) {
		va_end(args);
	}
	va_end(args);
}
}
