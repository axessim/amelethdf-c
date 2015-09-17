/**
 * @file   phmodel.c
 * @author Nathanaël MUOT <nathanael.muot@axessim.fr>
 * @date   Wed Sep 16 16:03:02 2015
 *
 * @brief
 *
 *
 */

#include <string.h>
#include <stdio.h>

#include <ah5.h>
#include "utest.h"

//! Test suite counter.
int tests_run = 0;

#define TOL 1e-5

char *test_read_phm_volume_instance()
{
  hid_t file_id;
  AH5_volume_instance_t vol;

  file_id = AH5_open_exemple_file("ah5_1_5_4.h5");

  mu_assert(
    "read bad math",
    !AH5_read_phm_volume_instance(file_id, "/physicalModel/volume/bad_name", &vol));

  /*Classical material*/
  mu_assert(
    "read classical_material",
    AH5_read_phm_volume_instance(file_id, "/physicalModel/volume/classical_material", &vol));

  mu_assert_eqf(
    "volumetric_mass_density",
    vol.volumetric_mass_density, 0.);
  mu_assert_eq(
    "electric_conductivity",
    vol.electric_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "electric_conductivity",
    vol.electric_conductivity.data.singlereal.value, 1.e-6, TOL);
  mu_assert_eq(
    "magnetic_conductivity",
    vol.magnetic_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "magnetic_conductivity",
    vol.magnetic_conductivity.data.singlereal.value, 0., TOL);
  mu_assert_eq(
    "relative_permeability",
    vol.relative_permeability.type, MP_SINGLE_COMPLEX);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.re, 1., TOL);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.im, 0., TOL);
  mu_assert_eq(
    "relative_permittivity",
    vol.relative_permittivity.type, MP_SINGLE_COMPLEX);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.singlecomplex.value.re, 1., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.singlecomplex.value.im, 0., TOL);

  AH5_print_phm_volume_instance(&vol, 0);
  AH5_free_phm_volume_instance(&vol);


  /*Debye material*/
  mu_assert(
    "read debye_material",
    AH5_read_phm_volume_instance(file_id, "/physicalModel/volume/debye_material", &vol));

  mu_assert_eqf(
    "volumetric_mass_density",
    vol.volumetric_mass_density, 1.);
  mu_assert_eq(
    "electric_conductivity",
    vol.electric_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "electric_conductivity",
    vol.electric_conductivity.data.singlereal.value, 1.e-6, TOL);
  mu_assert_eq(
    "magnetic_conductivity",
    vol.magnetic_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "magnetic_conductivity",
    vol.magnetic_conductivity.data.singlereal.value, 0., TOL);
  mu_assert_eq(
    "relative_permeability",
    vol.relative_permeability.type, MP_SINGLE_COMPLEX);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.re, 1., TOL);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.im, 0., TOL);
  mu_assert_eq(
    "relative_permittivity",
    vol.relative_permittivity.type, MP_DEBYE);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.debye.limit, 0., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.debye.stat, 1., TOL);
  mu_assert_eq(
    "relative_permittivity",
    vol.relative_permittivity.data.debye.nb_gtau, 1);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.debye.gtau[0], 10., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.debye.gtau[1], 1.1, TOL);

  AH5_print_phm_volume_instance(&vol, 0);
  AH5_free_phm_volume_instance(&vol);


  /*Lorentz material*/
  mu_assert(
    "read lorentz_material",
    AH5_read_phm_volume_instance(file_id, "/physicalModel/volume/lorentz_material", &vol));

  mu_assert_eqf(
    "volumetric_mass_density",
    vol.volumetric_mass_density, 5.);
  mu_assert_eq(
    "electric_conductivity",
    vol.electric_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "electric_conductivity",
    vol.electric_conductivity.data.singlereal.value, 3., TOL);
  mu_assert_eq(
    "magnetic_conductivity",
    vol.magnetic_conductivity.type, MP_SINGLE_REAL);
  mu_assert_close(
    "magnetic_conductivity",
    vol.magnetic_conductivity.data.singlereal.value, 4., TOL);
  mu_assert_eq(
    "relative_permeability",
    vol.relative_permeability.type, MP_SINGLE_COMPLEX);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.re, 1., TOL);
  mu_assert_close(
    "relative_permeability",
    vol.relative_permeability.data.singlecomplex.value.im, 0., TOL);
  mu_assert_eq(
    "relative_permittivity",
    vol.relative_permittivity.type, MP_LORENTZ);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.limit, 1., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.stat, 2., TOL);
  mu_assert_eq(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.nb_god, 1);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.god[0], 3., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.god[1], 4., TOL);
  mu_assert_close(
    "relative_permittivity",
    vol.relative_permittivity.data.lorentz.god[2], 5., TOL);

  AH5_print_phm_volume_instance(&vol, 0);
  AH5_free_phm_volume_instance(&vol);


  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}

// Run all tests
char *all_tests()
{
  mu_run_test(test_read_phm_volume_instance);

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
