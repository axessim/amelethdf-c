#include <string.h>
#include <stdio.h>

#include <ah5.h>
#include "utest.h"

//! Test suite counter.
int tests_run = 0;


char *test_creat_open_close()
{
  hid_t file_id;

  const char *filename = "test_creat_open_close.h5";
  const char *entrypoint = "my entry point";
  char *buf = NULL;

  // clean test space
  if (file_exists(filename))
    remove(filename);

  // create
  file_id = AH5_create(filename, H5F_ACC_TRUNC, NULL);
  mu_assert("create", HDF5_SUCCESS(file_id));
  mu_assert("file has been created", file_exists(filename));
  mu_assert("close file", HDF5_SUCCESS(AH5_close(file_id)));
  remove(filename);

  // create with entry point
  file_id = AH5_create(filename, H5F_ACC_TRUNC, entrypoint);
  mu_assert("create", HDF5_SUCCESS(file_id));
  mu_assert("file has been created", file_exists(filename));
  mu_assert("close file", HDF5_SUCCESS(AH5_close(file_id)));

  // open
  file_id = AH5_open(filename, H5F_ACC_RDONLY);
  mu_assert("open", HDF5_SUCCESS(file_id));

  // read entry point
  mu_assert_eq(
      "Check entry point strlen",
      AH5_read_entrypoint_strlen(file_id),
      strlen(entrypoint));

  buf = (char*)malloc(sizeof(char*) * (AH5_read_entrypoint_strlen(file_id) + 1));
  mu_assert(
      "read entry point",
      HDF5_SUCCESS(AH5_read_entrypoint(file_id, buf)));
  mu_assert_str_equal("read entry point", buf, entrypoint);

  mu_assert("close file", HDF5_SUCCESS(AH5_close(file_id)));
  remove(filename);

  return MU_FINISHED_WITHOUT_ERRORS;
}



// Run all tests
char *all_tests()
{
  mu_run_test(test_creat_open_close);

  return MU_FINISHED_WITHOUT_ERRORS;
}

// Main function, run tests and print results.
int main(int argc, char **argv)
{
  char *result;
  tests_run = 0;
  result = all_tests();

  if (result != 0)
  {
    printf("%s\n", result);
  }
  else
  {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
