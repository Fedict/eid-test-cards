//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Stephen Makumbi
//

#include "UnitTest++.h"
#include "xml.h"
#include <string.h>
#include <stdio.h>
#include "util.h"
#include <time.h>
#include "testlog.h"

using namespace EidInfra;

char VIRTUAL_CARD_PATH[1024];
char RELATIVE_PATH_PREFIX[32];

int RunAllTests(void);

int main(int argc, char* argv[])
{
	printf("************************************************************************\n");
	printf("NOTE: make sure the visibility in your control file are set to HIDE_REAL\n");
	printf("************************************************************************\n");

	FILE *f = NULL;
	if (fopen_s(&f, "../_DocsInternal/BeidTestCtrl.xml", "r") == 0)
		fclose(f);
	else
	{
		printf("\nERR: can't run the unit tests: ../_DocsInternal doesn't exist\n\n");
		return -1;
	}

	strcpy_s(RELATIVE_PATH_PREFIX, 31, "../");

	int result = 0;
	RunAllTests();

	return result;
}

// Help function
bool writeFile(const char *filename, const void *contents, size_t contentsLen)
{
	FILE *f = fopen(filename, "wb");
	if (f == NULL) {
		printf("ERR: couldn't open file \"%s\" to write to\n", filename);
		return false;
	}

	fwrite(contents, 1, contentsLen, f);
	fclose(f);

	return true;
}

typedef	struct {
	int result;
	int expectresult;
	char* testDescription;
	void (*test_function_ptr)(void);
} eIDTest;

int RunAllTests()
{
	clock_t startTime = clock();
	clock_t duration = 0;
	int msecDuration = 0;

	int i = 0;
	int nrofTests = 0;
	errno_t error;
	eIDTest eIDTests[] = { \

	{0,0,"TEST(Transmit) description",&Transmit},
	{0,0,"TEST(GetVirtualFilePath1) description",&GetVirtualFilePath1},
	{0,0,"TEST(GetShowFile) description",&GetShowFile},
	{0,0,"TEST(GetVirtualFilePaths) description",&GetVirtualFilePaths},
	{0,0,"TEST(GetVirtualFilePathsNoCrLf) description",&GetVirtualFilePathsNoCrLf},
	{0,0,"TEST(BadControlFiles) description",&BadControlFiles},
	{0,0,"TEST(addAll) description",&addAll},
	{0,0,"TEST(selectByPath) description",&selectByPath},
	{0,0,"TEST(selectByFID) description",&selectByFID},
	{0,0,"TEST(selectByAID) description",&selectByAID},
	{0,0,"TEST(loadFromFile) description",&loadFromFile},
	{0,0,"TEST(Readers) description",&Readers},
	{0,0,"TEST(Transactions) description",&Transactions},
	{0,0,"TEST(Properties) description",&Properties},
	{0,0,"TEST(GetNextElement) description",&GetNextElement},
	{0,0,"TEST(FindElement) description",&FindElement},
	{0,0,"TEST(seekToContentElement) description",&seekToContentElement},
	{0,0,"TEST(BigData) description",&BigData},
	};

	error = INITLOG();
	if(error != 0)
	{
		printf("Can't open logfile");
		return -1;
	}
	nrofTests = (sizeof(eIDTests)/sizeof(eIDTest));

	while (i < nrofTests )
	{
		//eIDTests[i].result = //for now, no testresults yet, only logs
		eIDTests[i].test_function_ptr();
		i++;
	}

	for (i = 0; i < nrofTests; i++)
	{
		if (eIDTests[i].testDescription != NULL)
		{
			testlog(LVL_NOLEVEL,"_______________________________________________\n");
			testlog(LVL_NOLEVEL,"Test %d %s \n", i, eIDTests[i].testDescription);
		}
//		if(eIDTests[i].result != eIDTests[i].expectresult)
//		{
//			testlog(LVL_NOLEVEL,"FAILED : Result = 0x%.8x Expected Result\n", eIDTests[i].result, eIDTests[i].expectresult );
//		}
	}

	duration = clock() - startTime;

	msecDuration = (duration * 1000) / CLOCKS_PER_SEC;
	printf("Duration: %d,%d seconds", msecDuration/1000, msecDuration%1000);

	testlog(LVL_NOLEVEL,"\n===============================================\n");

	ENDLOG();
	testlog(LVL_NOLEVEL,"press a key to end the test\n");
	// Wait for user to end this test
#ifdef WIN32
	getchar();
#endif
	return 0;
}