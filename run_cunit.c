/**
 * \file
 * \author Matthias P. Nowak
 * \date 2018-04-19
 * A general cunit file, containing a main function and can run the cunit tests in different modes.
 */

#include "run_cunit.h"


#undef CUNIT_SUITE
#define CUNIT_SUITE(name, init, clean) \
    int init(); \
    int clean();
#undef CUNIT_TEST
#define CUNIT_TEST(name, func) void func();
#include "running.inc"


#undef CUNIT_SUITE
#define CUNIT_SUITE(name, init, clean) run_cunit_suite(name,init,clean);
#undef CUNIT_TEST
#define CUNIT_TEST(name, func) reg_cunit_test(name,func);


  CU_pSuite pSuite = NULL;

void run_cunit_suite(char *name, int (*init)(),int(*clean)()) {
  if(!CU_get_registry()) {
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry ())
    {
      perror("no CUnit initalization");
      exit(EXIT_FAILURE);
    }
  }
  pSuite = CU_add_suite (name, init, clean);
  if (NULL == pSuite)
  {
    perror("Test suite registration failed");
  }
  return;
}

  void reg_cunit_test(char *name, void (*funct)()){
    if(!pSuite){
      fprintf(stderr,"no test suite registered\n");
      exit(EXIT_FAILURE);
    }
    if(!CU_add_test(pSuite,name, funct)){
      fprintf(stderr,"adding %s failed with %d\n",name,CU_get_error());
      perror("CU add test failed");
    }
}

/**
 *
 */
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
  // CU_list_tests_to_file ();
	 }
  int ret=0;
  fprintf(stderr,"numbers of failed tests: %d\n", ret=CU_get_number_of_tests_failed());
  CU_cleanup_registry ();
  return ret;
}
