/**
 * @author Matthias P. Nowak
 * @date 2018-04-19
 * A general cunit file, containing a main function and can run the cunit tests in different modes.
 */

#include "run_cunit.h"

/**
 * we need declarations of functions that are defined in the test_*.c files
 * the macros are retrieved by the makefile into running.inc
 * here we use makros to make function definitions
 */
#undef CUNIT_SUITE
#define CUNIT_SUITE(name, init, clean) \
    int init(); \
    int clean();
#undef CUNIT_TEST
#define CUNIT_TEST(name, func) void func();
#include "running.inc"

/**
 * redefine the macros to involve function calls for registering suites and tests
 */
#undef CUNIT_SUITE
#define CUNIT_SUITE(name, init, clean) run_cunit_suite(name,init,clean);
#undef CUNIT_TEST
#define CUNIT_TEST(name, func) reg_cunit_test(name,func);

// the current suite
CU_pSuite pSuite = NULL;

void run_cunit_suite(char *name, int (*init)(),int(*clean)()) {
  /**
   * registers a new suite
   */
  pSuite = CU_add_suite (name, init, clean);
  if (NULL == pSuite)
  {
    perror("Test suite registration failed");
    fprintf(stderr,"suite %s not registered\n",name);
  }
  return;
}

void reg_cunit_test(char *name, void (*funct)()) {
  /**
   * registers a new test
   */
  if(!pSuite) {
    // make sure that at least one suite was registered
    perror("no test suite registered\n");
    fprintf(stderr,"test %s not registered\n",name);
    return;
  }
  if(!CU_add_test(pSuite,name, funct)) {
    fprintf(stderr,"adding %s failed with %d\n",name,CU_get_error());
    perror("CU add test failed");
  }
}

void
print_usage (char *progName) {
  printf ("use %s -i for interactive testing\n", progName);
  printf ("use %s -b for batch testing to console\n", progName);
  printf ("use %s -a for automated testing to XML file\n", progName);
}
/**
 * Testing the queue using CUNIT in a specified manner
 */
int
main (int argc, char **argv)
{
  // modes for running the tests
  bool imode = 0, amode = 0, bmode = 0;
  int c;
  while (-1 != (c = getopt (argc, argv, "iba")))
    switch (c) {
    case 'i':
      imode = 1;
      break;
    case 'a':
      amode = 1;
      break;
    case 'b':
      bmode = 1;
      break;
    default:
      print_usage (argv[0]);
      exit (2);
    }
  if (!(imode || amode || bmode))
  {
    print_usage (argv[0]);
    exit (2);
  }

  if (CUE_SUCCESS != CU_initialize_registry ())
  {
    perror("no CUnit initalization");
    exit(EXIT_FAILURE);
  }
/*
 * running.inc contains macros that are transformed into function calls for registering suites and tests
 */
#include "running.inc"

  if (imode)
  {
    // is an interactive mode, user will notice
    CU_curses_run_tests ();
  }
  if (bmode)
  {
    /* Run all tests using the basic interface, it is verbose */
    CU_basic_set_mode (CU_BRM_VERBOSE);
    CU_basic_run_tests ();
    printf ("\n");
  }
  if (amode)
  {
    /* Run all tests using the automated interface */
    printf ("automated test, results are in Test-Results.xml\n");
    CU_set_output_filename ("Test");
    CU_automated_run_tests ();
    CU_list_tests_to_file ();
  }
  int ret=0;
  printf("numbers of failed tests: %d\n", ret=CU_get_number_of_tests_failed());
  CU_cleanup_registry ();
  return ret;
}
