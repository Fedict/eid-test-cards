#include <stdio.h>
#include <stdlib.h>
#include <string>

#define TEST(function) int function(void)


#define CHECK(bool) if((bool) == FALSE) return 1;
#define	CHECK_EQUAL(a, b) 	if (strcmp((a), (b)) != 0) return 2;
