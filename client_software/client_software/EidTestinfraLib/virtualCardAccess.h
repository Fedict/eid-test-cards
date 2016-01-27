
//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Stephen Makumbi
//

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#include "xml.h"

using namespace std;

namespace EidInfra
{
class VirtualCard : public Xml
{
public:
VirtualCard(void);
VirtualCard(const char *path);
virtual ~VirtualCard(void);

std::vector<string> GetAllData(const char *path);
std::vector<string> GetAllFiles2(const char *path);

char *GetType()
{
	return m_type;
}
char *GetTestCardVersion()
{
	return m_testCardVersion;
}
char *GetFriendlyName()
{
	return m_friendlyName;
}
char *GetChipnr()
{
	return m_chipNr;
}
char *GetDevCardChipNr()
{
	return m_devCardChipNr;
}
char *GetVersionnumbers()
{
	return m_versionNumbers;
}
char *GetAtr()
{
	return m_atr;
}
void GetData(const char *id, char * result, size_t maxLength);

private:
FILE                * m_file;
char                m_temp[8192];
char                m_type[32];
char                m_testCardVersion[16];
char                m_friendlyName[32];
char                m_chipNr[64];
char                m_devCardChipNr[64];
char                m_versionNumbers[32];
char                m_atr[32];
std::vector<string> m_contents;
};
}
