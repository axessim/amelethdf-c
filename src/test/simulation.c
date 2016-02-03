// test path tools

#include <string.h>
#include <stdio.h>

#include <ah5.h>
#include "utest.h"

//! Test suite counter.
int tests_run = 0;


char *test_read_exemple_file_1_5_4_near_field_with_nec_simulation()
{
  hid_t file_id;
  AH5_simulation_t simu;
  AH5_em_source_t em_source;
  AH5_label_t label;
  AH5_link_t link;
  AH5_mesh_t mesh;
  AH5_outputrequest_t outputrequest;
  
  file_id = AH5_open_exemple_file("ah5_1_5_4_near_field_with_nec_simulation.h5");
  
  mu_assert("read simulation", AH5_read_simulation(file_id, &simu));
  AH5_print_simulation(&simu);
  AH5_free_simulation(&simu);

  mu_assert("read em source", AH5_read_electromagnetic_source(file_id, &em_source));
  AH5_print_electromagnetic_source(&em_source);
  AH5_free_electromagnetic_source(&em_source);

  mu_assert("read label", AH5_read_label(file_id, &label));
  AH5_print_label(&label);
  AH5_free_label(&label);

  mu_assert("read link", AH5_read_link(file_id, &link));
  AH5_print_link(&link);
  AH5_free_link(&link);

  mu_assert("read mesh", AH5_read_mesh(file_id, &mesh));
  AH5_print_mesh(&mesh);
  AH5_free_mesh(&mesh);

  mu_assert("read outputrequest", AH5_read_outputrequest(file_id, &outputrequest));
  AH5_print_outputrequest(&outputrequest);
  AH5_free_outputrequest(&outputrequest);
  
  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_read_exemple_file_1_5_4_generic_mesh()
{
  hid_t file_id;
  AH5_mesh_t mesh;
  
  file_id = AH5_open_exemple_file("ah5_1_5_4_generic_mesh.h5");

  mu_assert("read mesh", AH5_read_mesh(file_id, &mesh));
  AH5_print_mesh(&mesh);
  AH5_free_mesh(&mesh);
  
  return MU_FINISHED_WITHOUT_ERRORS;
}


// Run all tests
char *all_tests()
{
  mu_run_test(test_read_exemple_file_1_5_4_near_field_with_nec_simulation);
  mu_run_test(test_read_exemple_file_1_5_4_generic_mesh);
  
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
