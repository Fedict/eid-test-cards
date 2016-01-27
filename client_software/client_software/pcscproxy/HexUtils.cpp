#include "HexUtils.h"
#include <memory.h>

using namespace zetes::pcscproxy;

#define ISHEXCHAR(c)    ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

static int hexcharacter2hex(char h)
{
	if (h >= '0' && h <= '9')
		return h - '0';
	if (h >= 'a' && h <= 'f')
		return h - 'a' + 10;
	return h - 'A' + 10;
}

int HexUtils::hexstring2hex(const char *hex, unsigned char *buf, size_t *len)
{
	size_t count = 0;

	while (*hex != '\0' && count < *len)
	{
		// skip all that is not a hex char
		while (!ISHEXCHAR(*hex) && *hex != '\0')
			hex++;
		if (*hex == '\0')
			break;

		// The nex char should be a hex char too
		if (ISHEXCHAR(hex[1]))
		{
			if (count >= *len)
			{
				return -1;
			}
			buf[count++] = (char) (16 * hexcharacter2hex(*hex) + hexcharacter2hex(hex[1]));
		}
		else
		{
			return -1;
		}

		hex += 2;
	}

	*len = count;

	return 0;
}

//int HexUtils::compareAPDUS(const char *hexApdu, const unsigned char *buf, size_t bufLen)
//{
//	unsigned char buf1[280];
//	size_t buf1Len = sizeof(buf1);
//
//	if (0 != hex2bin(hexApdu, buf1, &buf1Len))
//		return -1;
//
//	if (memcmp(buf1, buf, buf1Len*sizeof(char)) != 0) {
//		return -1;
//	}
//
//	return 0;
//}
