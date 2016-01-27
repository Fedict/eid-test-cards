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

using namespace EidInfra;

char VIRTUAL_CARD_PATH[1024];
char RELATIVE_PATH_PREFIX[32];

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

	int result = UnitTest::RunAllTests();

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
