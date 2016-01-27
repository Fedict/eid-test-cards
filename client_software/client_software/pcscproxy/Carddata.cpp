#include "Carddata.h"

#ifndef _WIN32
#include <winscard.h>
#include <cstring>
#endif

using namespace zetes::pcscproxy::carddata;

LONG Carddata::cmdNotAvailable(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, CMD_NOT_AVAILABLE, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::wrongParamP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, WRONG_PARAM_P1P2, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::leTooLong(IN size_t expected, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	pbRecvBuffer[0] = 0x6C;            //le too long, only xx bytes available (hexadecimal value)
	pbRecvBuffer[1] = (BYTE) expected; //bytes available
	*pcbRecvLength  = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::bytesRemaining(IN size_t remaining, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, IN size_t offset)
{
	if (*pcbRecvLength < 2 + offset)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	pbRecvBuffer[offset]     = 0x61;             //xx remaining bytes to retrieve
	pbRecvBuffer[offset + 1] = (BYTE) remaining; //remaining bytes
	*pcbRecvLength           = (BYTE) offset + 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::normalEndingOfCmd(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, IN size_t offset)
{
	if (*pcbRecvLength < 2 + offset)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer + offset, carddata::NORMAL_ENDING_OF_CMD, 2);
	*pcbRecvLength = (BYTE) offset + 2;;
	return SCARD_S_SUCCESS;
}

LONG Carddata::fileLcInconsistentWithP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, carddata::LC_INCONSISTENT_WITH_P1P2, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::fileNotFound(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, carddata::FILE_NOT_FOUND, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::fileWrongParamP1P2(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, carddata::FILE_WRONG_PARAM_P1P2, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}

LONG Carddata::cmdNotAllowedFileNoEF(OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	if (*pcbRecvLength < 2)
	{
		return SCARD_E_INSUFFICIENT_BUFFER;
	}
	memcpy(pbRecvBuffer, carddata::CMD_NOT_ALLOWED_FILE_NO_EF, 2);
	*pcbRecvLength = 2;
	return SCARD_S_SUCCESS;
}


