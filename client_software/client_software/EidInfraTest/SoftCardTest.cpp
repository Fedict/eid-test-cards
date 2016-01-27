#include "SoftCard.h"
#include "FileStructure.h"
#include "UnitTest++.h"

using namespace zetes::pcscproxy;

#ifdef _WIN32
static const char *VIRT_STEPHEN = "..\\_DocsInternal\\virtual_stephen.xml";
#else
static const char *VIRT_STEPHEN = "../_DocsInternal/virtual_stephen.xml";
#endif

TEST(loadFromFile)
{
	SoftReader sr("hardname", "softname");
	zetes::pcscproxy::SoftCard sc(&sr, 1234, 5678);
	BYTE                       hardNumber[16] =
	{ 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };

	sc.loadFromFile(VIRT_STEPHEN, hardNumber);

	CHECK_EQUAL(1234, sc.getHardHandle());
	CHECK_EQUAL(5678, sc.getSoftHandle());
}

