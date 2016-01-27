//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#include "SoftReader.h"
#include "SoftCard.h"
#include "Transaction.h"

using namespace zetes::pcscproxy;

SoftReader::SoftReader(const std::string& hardName, const std::string& softName):
	itsHardName(hardName), itsSoftName(softName)
{
	itsSoftCards.reserve(10);
}

SoftReader::~SoftReader()
{
	std::vector<SoftCard *>::iterator it;
	for (it = itsSoftCards.begin(); it != itsSoftCards.end(); it++)
		delete *it;
}

const std::string& SoftReader::getHardReaderName() const
{
	return itsHardName;
}

const std::string& SoftReader::getSoftReaderName() const
{
	return itsSoftName;
}

SoftCard* const SoftReader::getSoftCard(SCARDHANDLE softHandle) const
{
	std::vector<SoftCard *>::const_iterator it;
	for (it = itsSoftCards.begin(); it != itsSoftCards.end(); it++) {
		if (softHandle == (*it)->getSoftHandle())
			return *it;
	}
	return NULL;
}

SoftCard* SoftReader::createSoftCard(SCARDHANDLE hardHandle)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	// Let's hope hard SCARDHANDLEs are either more or less then 0xe1d000 apart from each other..
	SoftCard *sc = new SoftCard(this, hardHandle, hardHandle + 0xe1d000);
	itsSoftCards.push_back(sc);

	return sc;
}

LONG SoftReader::disconnect(SCARDHANDLE softHandle, DWORD dwDisposition)
{
	EidInfra::CAutoMutex autoMutex(&srMutex);

	std::vector<SoftCard *>::iterator it;
	for (it = itsSoftCards.begin(); it != itsSoftCards.end(); it++) {
		if (softHandle == (*it)->getSoftHandle()) {
			delete *it;
			itsSoftCards.erase(it);
			break;
		}
	}

	return SCARD_S_SUCCESS;
}

LONG SoftReader::reconnect(DWORD dwInitialization)
{
	/*
	lastCmdToCard = false;
	switch (dwInitialization) {
	case SCARD_LEAVE_CARD:
		//do nothing
		return SCARD_S_SUCCESS;
	case SCARD_RESET_CARD:
	case SCARD_UNPOWER_CARD:
		itsSoftCard->reset();
		return SCARD_S_SUCCESS;
	case SCARD_EJECT_CARD:
		if (itsSoftCard)
		{
			delete itsSoftCard;
			itsSoftCard = NULL;
		}
		return SCARD_S_SUCCESS;
	}
	*/
	return SCARD_E_INVALID_PARAMETER; // TODO
}

