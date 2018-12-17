#include <ah5.h>
#include "utest.h"

#define FILE XSTR(AH5_TEST_DATA_DIR) "/tst_utf-8.h5"
#define NODE_PATH "nœud principal contenant des caractères accentués"
#define UNODE_PATH "sous-nœud avec attribut accentué"
#define DSET_PATH "un dataset qui contient des caractères accentués"

//! Test suite counter.
int tests_run = 0;

char* test_utf_8() {
  hid_t file, node, unode, dset;
  size_t len;
  char res, *path, *attr, **dset_val;

  file = H5Fopen(FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
  mu_assert("H5Fopen() open file...", file >= 0);

  mu_assert("Valid node path...", AH5_path_valid(file, NODE_PATH));
  node = H5Oopen(file, NODE_PATH, H5P_DEFAULT);
  mu_assert("H5Oopen() open node...", node >= 0);

  mu_assert("Valid unode path...", AH5_path_valid(node, UNODE_PATH));
  unode = H5Oopen(node, UNODE_PATH, H5P_DEFAULT);
  mu_assert("H5Oopen() open unode...", unode >= 0);

  H5Oclose(unode);

  path = (char*) malloc(sizeof(char) * (1 + strlen(NODE_PATH) + 1 + strlen(UNODE_PATH) + 1));
  path[0] = '/';
  path[1] = '\0';
  strcat(path, NODE_PATH);
  strcat(path, "/");
  strcat(path, UNODE_PATH);
  mu_assert("Valid unode absolute path...", AH5_path_valid(file, path));
  unode = H5Oopen(file, path, H5P_DEFAULT);
  mu_assert("H5Oopen() open unode...", unode >= 0);
  free(path);

  len = AH5_read_str_attr_len(unode, ".", "attribut du sous-nœud accentué");
  mu_assert("AH5_read_str_attr_len() read utf-8 attr len...", len > 0);

  attr = (char*) malloc(sizeof(char) * (len + 1));
  res = AH5_read_str_attr(unode, ".", "attribut du sous-nœud accentué", &attr);
  mu_assert("AH5_read_str_attr() status...", res == AH5_TRUE);
  mu_assert("AH5_read_str_attr() str length...", strlen(attr) == strlen("é è à œ"));
  mu_assert("AH5_read_str_attr() str value...", strcmp(attr, "é è à œ") == 0);
  free(attr);

  mu_assert("Valid dset path...", AH5_path_valid(unode, DSET_PATH));
  dset = H5Oopen(unode, DSET_PATH, H5P_DEFAULT);
  mu_assert("H5Oopen() open dset...", dset >= 0);
  len = 11;
  dset_val = (char**) malloc(sizeof(char*));
  dset_val[0] = (char*) malloc(sizeof(char) * (len + 1));
  res = AH5_read_str_dataset(dset, ".", 1, len, &dset_val);
  mu_assert("AH5_read_str_dataset() status...", res == AH5_TRUE);
  mu_assert("AH5_read_str_dataset() str length...", strlen(dset_val[0]) == strlen("é è à œ"));
  mu_assert("AH5_read_str_dataset() str value...", strcmp(dset_val[0], "é è à œ") == 0);
  free(dset_val[0]);
  free(dset_val);

  H5Oclose(dset);
  H5Oclose(unode);
  H5Oclose(node);
  H5Fclose(file);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char* all_tests() {
  mu_run_test(test_utf_8);

  return MU_FINISHED_WITHOUT_ERRORS;
}


int main(/* int argc, char** argv */) {
  char *result;
  tests_run = 0;
  result = all_tests();

  if (result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL TESTS PASSED\n");
  }

  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
