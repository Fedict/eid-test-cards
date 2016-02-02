#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "testlog.h"

#define TEST(function) void function(void)


#define CHECK(bool) if((bool) == false)  {testlog(LVL_ERROR, "returned False");};
#define	CHECK_EQUAL(a, b) 	if (strcmp((a), (b)) != 0) {testlog(LVL_ERROR,(a));testlog(LVL_ERROR,(b));};
#define	CHECK_NUM_EQUAL(a, b) 	if ((a) != (b)) {testlog(LVL_ERROR, "%l doesn't equal %l",(a),(b));};
#define	CHECK_BYTE_ARRAY_EQUAL(a,b,len) if (memcmp((a), (b),(len)) != 0){testlogbytes(LVL_ERROR,(a),len);testlogbytes(LVL_ERROR,(b),len);};

TEST(Transmit);
TEST(GetVirtualFilePath1);
TEST(GetShowFile);
TEST(GetVirtualFilePaths);
TEST(GetVirtualFilePathsNoCrLf);
TEST(BadControlFiles);
TEST(addAll);
TEST(selectByPath);
TEST(selectByFID);
TEST(selectByAID);
TEST(loadFromFile);
TEST(Readers);
TEST(Transactions);
TEST(Properties);
TEST(GetNextElement);
TEST(FindElement);
TEST(seekToContentElement);
TEST(BigData);

