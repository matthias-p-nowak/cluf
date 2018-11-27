#include "cluf.h"
#include "run_cunit.h"

struct cluf_global _cluf;

CUNIT_SUITE("Test Suite for translation",init_suite,clean_suite)
int init_suite(){
	memset(&_cluf,0,sizeof(_cluf));
	return 0;
	}
int clean_suite(){
	return 0;
	}

// ###############################################
void test_succeed() {
	CU_ASSERT(true);
}
CUNIT_TEST("always succeed",test_succeed)
