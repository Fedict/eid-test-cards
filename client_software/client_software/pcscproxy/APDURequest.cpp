#include "APDURequest.h"
#include "Carddata.h"

using namespace zetes::pcscproxy;

APDURequest::APDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength) : t(OTHER), buffer(pbSendBuffer), length(cbSendLength)
{
	using namespace zetes::pcscproxy::carddata;

	if (length < 2)
		t = OTHER;
	else if (PROP_CLASS == buffer[0] && INS_GET_CARD_DATA == buffer[1])
		t = GET_CARD_DATA;
	else if (DEF_CLASS == buffer[0] && INS_READ_BINARY == buffer[1])
		t = READ_BINARY;
	else if (DEF_CLASS == buffer[0] && INS_SELECT_FILE == buffer[1])
		t = SELECT_FILE;
	else if (DEF_CLASS == buffer[0] && INS_GET_RESPONSE == buffer[1])
		t = GET_RESPONSE;

	else t = OTHER;
}

APDURequest::~APDURequest()
{
}

APDURequest APDURequest::createAPDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength)
{
	return APDURequest(pbSendBuffer, cbSendLength);
}

LPCBYTE APDURequest::getBuffer() const
{
	return buffer;
}

DWORD APDURequest::size() const
{
	return length;
}

APDURequest::TYPE APDURequest::getType() const
{
	return t;
}

bool APDURequest::isGetCardData() const
{
	return GET_CARD_DATA == t;
}

bool APDURequest::isSelectFile() const
{
	return SELECT_FILE == t;
}

bool APDURequest::isReadBinary() const
{
	return READ_BINARY == t;
}

bool APDURequest::isGetResponse() const
{
	return GET_RESPONSE == t;
}
