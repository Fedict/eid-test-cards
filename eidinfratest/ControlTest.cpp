#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include "xml.h"
#include "virtualCardAccess.h"
#include "Control.h"
#include "UnitTest++.h"
using namespace EidInfra;
using namespace std;
using namespace fedict::pcscproxy;

extern char * createRelativePath(const char * fileName);
extern bool writeFile(const char *filename, const void *contents, size_t contentsLen);

static const BYTE StephensChipNr[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };
static const BYTE RogiersChipNr[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x27, 0x0D, 0xF4, 0x01, 0x13, 0x18 };
static const BYTE FloresChipNr[16] = { 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x26, 0x23, 0x68, 0x19, 0x2F, 0x16 };

#define DEFAULT_CONTROL_FILE_NAME      "_DocsInternal/BeidTestCtrl.xml"
#define ALTERNATE_CONTROL_FILE_NAME    "_DocsInternal/BeidTestCtrl2.xml"

TEST(GetVirtualFilePaths)
{
	Control ctl(createRelativePath(DEFAULT_CONTROL_FILE_NAME));

	CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());

	VirtualCard Stephen(ctl.getSoftCardFileLocation(StephensChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF232CF7131031", Stephen.GetChipnr());

	VirtualCard Rogier(ctl.getSoftCardFileLocation(RogiersChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF270DF4011319", Rogier.GetChipnr());

	VirtualCard Flores(ctl.getSoftCardFileLocation(FloresChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF262368192F17", Flores.GetChipnr());
}


TEST(GetVirtualFilePathsNoCrLf)
{
	Control ctl(createRelativePath(ALTERNATE_CONTROL_FILE_NAME));

	CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());

	VirtualCard Stephen(ctl.getSoftCardFileLocation(StephensChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF232CF7131031", Stephen.GetChipnr());

	VirtualCard Rogier(ctl.getSoftCardFileLocation(RogiersChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF270DF4011319", Rogier.GetChipnr());

	VirtualCard Flores(ctl.getSoftCardFileLocation(FloresChipNr).c_str());
	CHECK_EQUAL("534C494E336600296CFF262368192F17", Flores.GetChipnr());
}

TEST(BadControlFiles)
{
	{
		Control ctl("nosuchfile");
		CHECK_EQUAL("", ctl.getSoftCardFileLocation((const unsigned char *) "xxxxxxxxxxxxxxxx").c_str());
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
	}

	if (writeFile("empty", "", 0))
	{
		Control ctl("empty");
		CHECK_EQUAL("", ctl.getSoftCardFileLocation((const unsigned char *) "xxxxxxxxxxxxxxxx").c_str());
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
	}

	unsigned char *buf = new unsigned char [100000];
	for (int i = 0; i < 100000; i++) {
		buf[i] = (unsigned char) rand();
	}
	if (writeFile("bigrandom", buf, 100000))
	{
		Control ctl("bigrandom");
		CHECK_EQUAL("", ctl.getSoftCardFileLocation((const unsigned char *) "xxxxxxxxxxxxxxxx").c_str());
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
	}
	delete [] buf;

	buf = new unsigned char [100000];
	memset(buf, 0, 100000);
	if (writeFile("bigzeros", buf, 100000))
	{
		Control ctl("bigzeros");
		CHECK_EQUAL("", ctl.getSoftCardFileLocation((const unsigned char *) "xxxxxxxxxxxxxxxx").c_str());
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
	}
	delete [] buf;

	const char *conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<control>"
		"	<show>AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA</show>"
		"</control>";
	if (writeFile("bigvisibility", conts, strlen(conts)))
	{
		Control ctl("bigvisibility");
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
		CHECK_EQUAL("", ctl.getSoftCardFileLocation((const unsigned char *) "xxxxxxxxxxxxxxxx").c_str());
	}

	conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<control>"
		"	<show>HIDE_REAL</show>"
		"	<virtualcard>"
		"		<hardchipnr>534C494E336600296CFF2623660B0826</hardchipnr>"
		"		<file>AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA</file>"
		"	</virtualcard>"
		"</control>";
	if (writeFile("bigpath", conts, strlen(conts)))
	{
		Control ctl("bigpath");
		CHECK_NUM_EQUAL(HIDE_REAL, ctl.getListReaderOrder());
		unsigned char chipnr[] = {0x53,0x4C,0x49,0x4E,0x33,0x66,0x00,0x29,0x6C,0xFF,0x26,0x23,0x66,0x0B,0x08,0x26};
		std::string locStr = ctl.getSoftCardFileLocation(chipnr);
		const char *loc = locStr.c_str();
		CHECK(strlen(loc) > 100 && loc[0] == 'A');
	}

	conts =
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
		"<control>"
		"	<show>HIDE_REA";
	if (writeFile("badxml", conts, strlen(conts)))
	{
		Control ctl("badxml");
		CHECK_NUM_EQUAL(REAL_LAST, ctl.getListReaderOrder());
		unsigned char chipnr[] = {0x53,0x4C,0x49,0x4E,0x33,0x66,0x00,0x29,0x6C,0xFF,0x26,0x23,0x66,0x0B,0x08,0x26};
		CHECK_EQUAL("", ctl.getSoftCardFileLocation(chipnr).c_str());
	}
}
