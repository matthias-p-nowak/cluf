/*
 * @author Matthias P. Nowak
 * @copyright LGPL 3.0 https://opensource.org/licenses/lgpl-3.0.html
 */

#include "testing.h"


struct cluf_global _cluf;
jmp_buf jmpBuf;
char *exit_msg=NULL;

void cluf_exit(char *msg) {
  /**
   * redefining the exit function used in the test program 
   * preventing the tests to exit due to an exit call in the tested routines 
   * the tested routines need to be run inside a setjump call switch
   */
  exit_msg=msg;
  longjmp(jmpBuf,1);
}

// during setup, pointers to character arrays get stored, hence those arrays can't be on the stack
char buf1[PATH_MAX];
char buf2[PATH_MAX];


int dummy() {
  return 0;
}
// ###############################################

CUNIT_SUITE("setup tests",dummy,dummy)

void test_succeed() {
  /*
   * get started with testing - this one should always work
   */
  CU_ASSERT(true);
}
CUNIT_TEST("always succeed",test_succeed)


void test_forgotten_setup() {
  /*
   * no path set - must fail
   */
  memset(&_cluf,0,sizeof(_cluf));
  if(setjmp(jmpBuf)==0) {
    cluf_setup_1();
    // should never reach the next line
    CU_FAIL("require failing in setup_1 due to missing data");
  } else {
    // after an exit
    CU_ASSERT_STRING_EQUAL(exit_msg,"no source directory given");
  }
}
CUNIT_TEST("zero setup",test_forgotten_setup)

void test_source_length() {
  memset(&_cluf,0,sizeof(_cluf));
  /*
   * additional '/' are removed at the end
   */
  strcpy(buf1,"hello//");
  _cluf.sourceName=buf1;
  strcpy(buf2,"world");
  _cluf.targetName=buf2;
  cluf_setup_1();
  CU_ASSERT_EQUAL(_cluf.sourceLen,5);
  CU_ASSERT_EQUAL(_cluf.targetLen,5);
  /*
   * same with the other side
   */
  strcpy(buf1,"hello");
  _cluf.sourceName=buf1;
  strcpy(buf2,"world///");
  _cluf.targetName=buf2;
  cluf_setup_1();
  CU_ASSERT_EQUAL(_cluf.sourceLen,5);
  CU_ASSERT_EQUAL(_cluf.targetLen,5);
}
CUNIT_TEST("source/target length",test_source_length)

void test_wrong_shortened() {
  memset(&_cluf,0,sizeof(_cluf));
  /*
   * in case for a chrooted case, the source must reside inside the target, such that symlinks can work
   * it means that the target is a prefix to the source
   */
  _cluf.sourceName="commonmine";
  _cluf.targetName="commonother";
  _cluf.shortenLinks=true;
  if(setjmp(jmpBuf)==0) {
    cluf_setup_1(); // should exit
    // should never arrive at the next line
    CU_FAIL("required failing in setup_1 due to wrong shortening");
  } else {
    // came here due to an exit
    CU_ASSERT_STRING_EQUAL(exit_msg,"shortening of symlinks not possible due to setup");
  }
}
CUNIT_TEST("wrong shortened setup",test_wrong_shortened)


// ###############################################

int init_suite() {
  /*
   * create a bit random source directory and test on correct target creation
   */
  memset(&_cluf,0,sizeof(_cluf));
  snprintf(buf1,PATH_MAX,"/tmps/t1/biggest/h%ld",random());
  _cluf.sourceName=buf1;
  strcpy(buf2,"/tmps/t2");
  _cluf.targetName=buf2;
  _cluf.shortenLinks=false;
  // _cluf.debug=1000;
  cluf_setup_1();
  return 0;
}
CUNIT_SUITE("Test Suite for translation",init_suite,dummy)


void test_basic() {
  /*
   * any source identical to the source should be translated into the target
   */
  char buf2[PATH_MAX];
  buf2[0]='\0';
  int destLen;
  if(cluf_source2target(_cluf.sourceName,buf2,&destLen))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf2,_cluf.targetName);
}
CUNIT_TEST("basic test",test_basic)

void test_hello1() {
  /*
   * the "hello world" equivalent
   */
  char buf1[PATH_MAX];
  char buf2[PATH_MAX];
  buf2[0]='\0';
  int destLen;
  snprintf(buf1,PATH_MAX,"%s/%s",_cluf.sourceName,"hello world");
  if(cluf_source2target(buf1,buf2,&destLen))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf2,"/tmps/t2/hello world");
}
CUNIT_TEST("simple src->target",test_hello1)

void test_shortened3(){
  /*
   * for symlinks, the shortened function is called, but should work like the regular source2target function
   */
  char buf2[PATH_MAX];
  buf2[0]='\0';
  if(cluf_source2shortened(_cluf.sourceName,buf2))
    CU_FAIL("source to shortened(not) failed");
  CU_ASSERT_STRING_EQUAL(buf2,"/tmps/t2");
}
CUNIT_TEST("transform source to target in non-shortened case",test_shortened3)

void test_shortened2() {
  /*
   * the same, but with a "hello world" ending
   */
  char buf1[PATH_MAX];
  char buf2[PATH_MAX];
  buf2[0]='\0';
  snprintf(buf1,PATH_MAX,"%s/%s",_cluf.sourceName,"hello world");
  if(cluf_source2shortened(buf1,buf2))
    CU_FAIL("source to shortened(not) failed");
  CU_ASSERT_STRING_EQUAL(buf2,"/tmps/t2/hello world");
}
CUNIT_TEST("shortened in non-shortened case",test_shortened2)


// ###############################################
int init_suite2() {
  /*
   * setup with a shortened case, where source is inside target so a chrooted environment will work
   */
  memset(&_cluf,0,sizeof(_cluf));
  strcpy(buf1,"/tmps/t2/big");
  _cluf.sourceName=buf1;
  strcpy(buf2,"/tmps/t2");
  _cluf.targetName=buf2;
  _cluf.shortenLinks=true;
  cluf_setup_1();
  return 0;
}
CUNIT_SUITE("Test Suite for shorted translation",init_suite2,dummy)

void test_shortened1() {
  /*
   * source name should be translated into a short name that makes sense inside the target
   */
  char buf3[PATH_MAX];
  buf3[0]='\0';
  if(cluf_source2shortened(_cluf.sourceName,buf3))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf3,"/big");
  if(cluf_source2shortened(_cluf.targetName,buf3))
    CU_FAIL("source 2 target failed");
  CU_ASSERT_STRING_EQUAL(buf3,"/");
  int r=cluf_source2shortened("/tmps/t",buf3);
  CU_ASSERT_EQUAL(r,1);

}
CUNIT_TEST("basic shortened test",test_shortened1)
