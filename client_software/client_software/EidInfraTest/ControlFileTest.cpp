#include <stdio.h>
#include <memory>
#include "xml.h"
#include "controlFileAccess.h"
#include "UnitTest++.h"
using namespace EidInfra;
using namespace std;

TEST(GetVirtualFilePath1)
{
	ControlFile ctl("ControlTest.xml", REAL_LAST);

	ctl.Add("hardchipnr 1", "C:/Documents And Settings/Stephen/virtual1.xml");
	ctl.Add("hardchipnr 2", "C:/Documents And Settings/Stephen/virtual2.xml");
	ctl.Add("hardchipnr 3", "C:/Documents And Settings/Stephen/virtual3.xml");
	ctl.Add("hardchipnr 4", "C:/Documents And Settings/Stephen/virtual4.xml");
	ctl.Add("hardchipnr 5", "");
	ctl.Add("hardchipnr 6", "C:/Documents And Settings/Stephen/virtual6.xml");
	ctl.Save("ControlTest.xml");

	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual1.xml", ctl.GetVirtualFilePath("hardchipnr 1").c_str());
	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual2.xml", ctl.GetVirtualFilePath("hardchipnr 2").c_str());
	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual6.xml", ctl.GetVirtualFilePath("hardchipnr 6").c_str());
	CHECK_EQUAL(ctl.GetVirtualFilePath("hardchipnr 20").c_str(), ""); // no such hard card!
	CHECK_EQUAL(ctl.GetVirtualFilePath("hardchipnr 5").c_str(), "");  //empty path
}

TEST(GetShowFile)
{
	ControlFile ctl1("ControlTest.xml", REAL_LAST);
	CHECK(ctl1.GetShowType() == REAL_LAST);
}
