//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Stephen Makumbi
//

#ifndef __control_file_access_h__
#define __control_file_access_h__

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "xml.h"
#include "util.h"

using namespace std;

#define CONTROL_FILE_PATH    "C:\\Documents and Settings\\All Users\\BeidTestCtrl.xml"

enum showType {
	REAL_FIRST,
	REAL_LAST,
	HIDEREAL,
	HIDE_VIRTUAL,
	NON_SHOW_TYPE  //gaurd
};

namespace EidInfra
{
class ControlFile : public Xml
{
public:
std:: vector<string> m_readers;
ControlFile(const char * path, showType type);
virtual ~ControlFile();
void Add(const char *hardchipnr, const char *file);
void Show();
void SetPath(const char *path)
{
	strcpy_s((char *) filePath, 2048, path);
}

void Save(const char * fileName);
string GetVirtualFilePath(const char *hardchipnr);
showType GetShowType();         //{return m_type;}
private:
char     filePath[2048];
showType m_type;
};
}

#endif
