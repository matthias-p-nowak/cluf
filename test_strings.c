
#include "testing.h"
#include <setjmp.h>

struct cluf_global _cluf;
jmp_buf jmpBuf;
char *exit_msg=NULL;

void cluf_exit(char *msg) {
  exit_msg=msg;
  longjmp(jmpBuf,1);
}

// ###############################################

int dummy() {
  return 0;
}
CUNIT_SUITE("setup tests",dummy,dummy)

void test_forgotten_setup() {
  memset(&_cluf,0,sizeof(_cluf));
  if(setjmp(jmpBuf)==0) {
    cluf_setup_1();
    CU_FAIL("not failing when required in setup_1");
  } else{
    CU_ASSERT_STRING_EQUAL(exit_msg,"no source directory given");
  }
}
CUNIT_TEST("zero setup",test_forgotten_setup)

void test_source_length(){
  memset(&_cluf,0,sizeof(_cluf));
  _cluf.sourceName="hello";
  _cluf.targetName="world";
  cluf_setup_1();
  CU_ASSERT_EQUAL(_cluf.sourceLen,5);
  CU_ASSERT_EQUAL(_cluf.targetLen,5);
}
CUNIT_TEST("source/target length",test_source_length)

// ###############################################
int init_suite(){
	memset(&_cluf,0,sizeof(_cluf));
  _cluf.sourceName="/tmps/t2/big";
  _cluf.targetName="/tmps/t2";
  _cluf.shortenLinks=false;
  // _cluf.debug=1000;
  cluf_setup_1();
	return 0;
	}
CUNIT_SUITE("Test Suite for translation",init_suite,dummy)


// ###############################################
void test_succeed() {
	CU_ASSERT(true);
}
CUNIT_TEST("always succeed",test_succeed)

void test_basic(){
  char buf2[PATH_MAX];
  buf2[0]='\0';
  if(cluf_source2target(_cluf.sourceName,buf2))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf2,_cluf.targetName);
}
CUNIT_TEST("basic test",test_basic)

void test_hello1(){
	char buf1[PATH_MAX];
  char buf2[PATH_MAX];
  buf2[0]='\0';
  snprintf(buf1,PATH_MAX,"%s/%s",_cluf.sourceName,"hello world");
  if(cluf_source2target(buf1,buf2))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf2,"/tmps/t2/hello world");
}
CUNIT_TEST("simple src->target",test_hello1)


// ###############################################
int init_suite2() {
  memset(&_cluf,0,sizeof(_cluf));
  _cluf.sourceName="/tmps/t2/big";
  _cluf.targetName="/tmps/t2";
  _cluf.shortenLinks=true;
  cluf_setup_1();
  return 0;
}
CUNIT_SUITE("Test Suite for shorted translation",init_suite2,dummy)

void test_shortened1() {
  char buf2[PATH_MAX];
  buf2[0]='\0';
  if(cluf_source2shortened(_cluf.sourceName,buf2))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf2,"/");
}
CUNIT_TEST("basic shortened test",test_shortened1)
