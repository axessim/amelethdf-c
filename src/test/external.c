// test path tools

#include <string.h>
#include <stdio.h>

#include "utest.h"

#include <ah5.h>

#define EXTERNAL_TEST_FILE  (XSTR(AH5_TEST_DATA_DIR) "/test_external_principle.h5")

#define mu_assert_equal2(x, y) mu_assert("Test of equality failed: " #x " != " #y "." , x == y)
#define mu_assert_str_equal2(x, y) mu_assert("Test of equality failed: " #x " != " #y "." , AH5_strcmp(x, y) == 0)


//! Test suite counter.
int tests_run = 0;


char *test_external()
{
  hid_t file = -1;
  AH5_external_element_t externals;
  char success = AH5_TRUE;
  AH5_eet_dataset_t* external = NULL;
  const char* path_ext = NULL;
  hid_t file_ext = 0;

  file = H5Fopen(EXTERNAL_TEST_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
  success = AH5_read_external_element(file, &externals);

  mu_assert("Fail to read external element.", success == AH5_TRUE);

  mu_assert_equal2(externals.nb_datasets, 1);
  external = externals.datasets;
  mu_assert_equal2(external->nb_eed_items, 1);
  mu_assert_str_equal2(
      AH5_get_name_from_path(external->principle_file_path), "test_external_principle.h5");
  printf("path: %s\n", external->path);
  mu_assert_str_equal2(external->path, "/externalElement/external_elements");
  mu_assert_str_equal2(external->eed_items[0], "/totö/tutu/titî");
  mu_assert_str_equal2(
      AH5_get_name_from_path(external->eed_items[1]), "test_external_external.h5");
  mu_assert_str_equal2(external->eed_items[2], "/totö2/tutu2/titî2");

  success = AH5_is_external_element(&externals, "/totö/tutu/titî", &file_ext, &path_ext);
  mu_assert("External not found.", success == AH5_TRUE);
  mu_assert_equal2(file_ext, external->file_id[0]);
  mu_assert_str_equal2(path_ext, "/totö2/tutu2/titî2");
  mu_assert_equal2(path_ext, external->eed_items[2]);

  AH5_free_external_element(&externals);
  H5Fclose(file);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_file_path_next_to()
{

  const char* fpath = "/x/y/z";
  const char* fname = "toto";
  char* fpath2 = NULL;

  // Get the size of the resulting path.
  size_t size = AH5_file_path_next_to(fpath, fname, NULL, 0);
  mu_assert("Wrong size.", size == 10);
  fpath2 = malloc(size);
  AH5_file_path_next_to(fpath, fname, fpath2, size);

  mu_assert("file_path_next_to has failed.", AH5_strcmp(fpath2, "/x/y/toto") == 0);
  free(fpath2);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *all_tests()
{
  mu_run_test(test_external);
  mu_run_test(test_file_path_next_to);

  return 0;
}

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
