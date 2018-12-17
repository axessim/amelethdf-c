// test path tools

#include <string.h>
#include <stdio.h>

#include <ah5.h>
#include "utest.h"

//! Test suite counter.
int tests_run = 0;


char *test_init_datasetx()
{
  AH5_datasetx_t data;

  mu_assert("bad input", !AH5_init_datasetx(NULL, 0, H5T_FLOAT));
  mu_assert("bad input", !AH5_init_datasetx(&data, 1, H5T_NATIVE_LDOUBLE));
  mu_assert("empty", AH5_init_datasetx(&data, 0, H5T_FLOAT));
  mu_assert_eq_ptr("empty", data.f, NULL);

  mu_assert("valid float", AH5_init_datasetx(&data, 5, H5T_FLOAT));
  mu_assert_ne("valid float", data.f, NULL);
  free(data.f);

  mu_assert("valid int", AH5_init_datasetx(&data, 5, H5T_INTEGER));
  mu_assert_ne("valid int", data.i, NULL);
  free(data.i);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_init_vector()
{
  AH5_vector_t vec;

  mu_assert("bad input", !AH5_init_ft_vector(NULL, NULL, 0, H5T_FLOAT));

  mu_assert("set path", AH5_init_ft_vector(&vec, "name", 0, H5T_FLOAT));
  mu_assert_str_equal("set path", vec.path, "name");
  mu_assert_eq("empty vec", vec.nb_values, 0);
  mu_assert_eq_ptr("empty vec", vec.values.f, NULL);
  mu_assert_eq_ptr("empty vec", vec.values.i, NULL);
  free(vec.path);

  vec.path = NULL;
  mu_assert("init without name", AH5_init_ft_vector(&vec, NULL, 10, H5T_FLOAT));
  mu_assert_eq_ptr("vec name", vec.path, NULL);
  mu_assert_eq("vec size", vec.nb_values, 10);
  mu_assert_ne("vec value", vec.values.f, NULL);
  free(vec.values.f);

  mu_assert("init", AH5_init_ft_vector(&vec, "name", 10, H5T_FLOAT));
  AH5_free_ft_vector(&vec);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_init_dataset()
{
  AH5_dataset_t flt;
  hsize_t dims1d[] = {5};
  hsize_t dims2d[] = {3, 2};

  mu_assert("bad input", !AH5_init_ft_dataset(NULL, NULL, 0, NULL, H5T_FLOAT));
  mu_assert("empty", AH5_init_ft_dataset(&flt, NULL, 0, NULL, H5T_FLOAT));
  mu_assert_eq("empty", flt.nb_dims, 0);
  mu_assert_eq_ptr("empty", flt.path, NULL);
  mu_assert_eq_ptr("empty", flt.dims, NULL);

  mu_assert("1d float", AH5_init_ft_dataset(&flt, "name", 1, dims1d, H5T_FLOAT));
  mu_assert_str_equal("1d float", flt.path, "name");
  mu_assert_eq("1d float", flt.nb_dims, 1);
  mu_assert_eq("1d float", flt.dims[0], 5);
  mu_assert_ne("1d float", flt.values.f, NULL);
  AH5_free_ft_dataset(&flt);

  mu_assert("2d float", AH5_init_ft_dataset(&flt, "name", 2, dims2d, H5T_INTEGER));
  mu_assert_str_equal("2d float", flt.path, "name");
  mu_assert_eq("2d float", flt.nb_dims, 2);
  mu_assert_eq("2d float", flt.dims[0], 3);
  mu_assert_eq("2d float", flt.dims[1], 2);
  mu_assert_ne("2d float", flt.values.i, NULL);
  AH5_free_ft_dataset(&flt);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_init_arrayset()
{
  AH5_arrayset_t flt;
  hsize_t dims1d[] = {5};
  hsize_t dims2d[] = {3, 2};

  mu_assert("bad input", !AH5_init_ft_arrayset(NULL, NULL, 0, NULL, H5T_FLOAT));
  mu_assert("empty", AH5_init_ft_arrayset(&flt, NULL, 0, NULL, H5T_FLOAT));
  mu_assert_eq("empty", flt.nb_dims, 0);
  mu_assert_eq_ptr("empty", flt.path, NULL);
  mu_assert_eq_ptr("empty", flt.dims, NULL);

  mu_assert("1d float", AH5_init_ft_arrayset(&flt, "name", 1, dims1d, H5T_FLOAT));
  mu_assert_str_equal("1d float", flt.path, "name");
  mu_assert_eq("1d float", flt.nb_dims, 1);
  mu_assert_ne("1d float", flt.dims, NULL);
  mu_assert_str_equal("1d float", flt.dims[0].path, "dim1");
  mu_assert_eq("1d float", flt.dims[0].nb_values, 5);
  mu_assert_eq_ptr("1d float", flt.dims[0].values.f, NULL);
  mu_assert_str_equal("1d float", flt.data.path, "data");
  mu_assert_eq("1d float", flt.data.nb_dims, 1);
  mu_assert_eq("1d float", flt.data.dims[0], 5);
  mu_assert_ne("1d float", flt.data.values.f, NULL);

  AH5_ft_arrayset_set_meshdim(flt.dims, "/mesh/path");
  mu_assert_str_equal("data on mesh", flt.dims->values.s[0], "/mesh/path");
  mu_assert_eq("data on mesh", flt.dims->nb_values, 1);
  mu_assert_eq("data on mesh", flt.dims->type_class, AH5_TYPE_STRING);

  AH5_free_ft_arrayset(&flt);

  mu_assert("2d float", AH5_init_ft_arrayset(&flt, "name", 2, dims2d, H5T_INTEGER));
  mu_assert_str_equal("2d float", flt.path, "name");
  mu_assert_eq("2d float", flt.nb_dims, 2);
  mu_assert_ne("2d float", flt.dims, NULL);
  mu_assert_str_equal("2d float", flt.dims[0].path, "dim1");
  mu_assert_eq("2d float", flt.dims[0].nb_values, 2);
  mu_assert_eq_ptr("2d float", flt.dims[0].values.f, NULL);
  mu_assert_str_equal("2d float", flt.dims[1].path, "dim2");
  mu_assert_eq("2d float", flt.dims[1].nb_values, 3);
  mu_assert_eq_ptr("2d float", flt.dims[1].values.f, NULL);
  mu_assert_str_equal("2d float", flt.data.path, "data");
  mu_assert_eq("2d float", flt.nb_dims, 2);
  mu_assert_ne("2d float", flt.dims, NULL);
  mu_assert_eq("2d float", flt.data.nb_dims, 2);
  mu_assert_eq("2d float", flt.data.dims[0], 3);
  mu_assert_eq("2d float", flt.data.dims[1], 2);
  mu_assert_ne("2d float", flt.data.values.i, NULL);
  AH5_free_ft_arrayset(&flt);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_write_single_flotingtype()
{
  hid_t file_id;

  AH5_singleinteger_t flti;
  AH5_singlereal_t fltr;

  int idata;
  float rdata;

  // Write a simple mesh test.
  file_id = AH5_auto_test_file();

  // single integer
  flti.path = (char *)"/floatingType/flti";
  flti.value = 10;
  flti.opt_attrs.nb_instances = 0;
  mu_assert("Write single integer.",
            AH5_write_ft_singleinteger(file_id, &flti));

  AH5_read_int_attr(file_id, "/floatingType/flti", "value", &idata);
  mu_assert_eq("Test the written int value.", idata, 10);

  // single real
  fltr.path = (char *)"/floatingType/fltr";
  fltr.value = 0.5;
  fltr.opt_attrs.nb_instances = 0;
  mu_assert("Write single real.",
            AH5_write_ft_singlereal(file_id, &fltr));

  AH5_read_flt_attr(file_id, "/floatingType/fltr", "value", &rdata);
  mu_assert_eq("Test the written real value.", rdata, 0.5);

  // Close file.
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}

char *test_write_ft_vector()
{
  hid_t file_id;
  hsize_t i;
  const hsize_t nb_values = 10;
  float *fvalues;

  AH5_vector_t vector;

  // Write a simple mesh test.
  file_id = AH5_auto_test_file();

  vector.path = "/floatingType/vector";
  vector.opt_attrs.nb_instances = 0;
  vector.type_class = H5T_FLOAT;
  vector.nb_values = nb_values;
  vector.values.f = (float *)malloc(vector.nb_values*sizeof(float));
  for (i = 0; i < vector.nb_values; ++i)
    vector.values.f[i] = 0.3 * i;

  mu_assert("Write vector.",
            AH5_write_ft_vector(file_id, &vector));

  mu_assert("Read float dataset for check.",
            AH5_read_flt_dataset(file_id, "/floatingType/vector", nb_values, &fvalues));
  for (i = 0; i < nb_values; ++i)
    mu_assert_close("Test the written real values.",
                    fvalues[i], 0.3 * i, 1.e-4);

  free(fvalues);

  // Close file.
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}

char *test_write_ft_dataset()
{
  hid_t file_id;
  hsize_t i;
  float *fvalues;

  AH5_dataset_t ds;

  // Write a simple mesh test.
  file_id = AH5_auto_test_file();

  ds.path = "/floatingType/dataset";
  ds.opt_attrs.nb_instances = 0;
  ds.type_class = H5T_FLOAT;
  ds.nb_dims = 2;
  ds.dims = (hsize_t *)malloc(ds.nb_dims*sizeof(hsize_t));
  ds.dims[0] = 10;
  ds.dims[1] = 2;
  ds.values.f = (float *)malloc(ds.dims[0]*ds.dims[1]*sizeof(float));
  for (i = 0; i < ds.dims[0]*ds.dims[1]; ++i)
    ds.values.f[i] = (float)0.3 * i;

  mu_assert("Write ds.",
            AH5_write_ft_dataset(file_id, &ds));

  AH5_read_flt_dataset(file_id, "/floatingType/dataset", ds.dims[0]*ds.dims[1], &fvalues);
  for (i = 0; i < ds.dims[0]*ds.dims[1]; ++i)
    mu_assert_close("Test the written real values.",
                    fvalues[i], 0.3 * (float)i, 1.e-4);

  free(fvalues);

  // Close file.
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}

char *test_write_ft_arrayset()
{
  hid_t file_id;
  hsize_t i;
  int dim = 0;
  hsize_t nb_values = 1;
  float *fvalues;
  char **svalues;

  AH5_arrayset_t array, dataonmesh;
  char *meshpath = "/mesh/gmesh/umesh/group/groupname";

  // Write a simple mesh test.
  file_id = AH5_auto_test_file();

  /* simple array set*/
  array.path = "/floatingType/array";
  array.opt_attrs.nb_instances = 0;
  array.nb_dims = 2;
  array.dims = (AH5_vector_t *)malloc(array.nb_dims*sizeof(AH5_vector_t));

  array.dims[0].nb_values = 10;
  array.dims[0].opt_attrs.nb_instances = 0;
  array.dims[0].type_class = H5T_FLOAT;
  array.dims[0].values.f = (float *)malloc(array.dims[0].nb_values*sizeof(float));
  for (i = 0; i < array.dims[0].nb_values; ++i)
    array.dims[0].values.f[i] = 0.3 * i;

  array.dims[1].nb_values = 2;
  array.dims[1].opt_attrs.nb_instances = 0;
  array.dims[1].type_class = H5T_INTEGER;
  array.dims[1].values.i = (int *)malloc(array.dims[1].nb_values*sizeof(int));
  for (i = 0; i < array.dims[1].nb_values; ++i)
    array.dims[1].values.i[i] = i * 3;

  array.data.type_class = H5T_FLOAT;
  array.data.opt_attrs.nb_instances = 0;
  array.data.nb_dims = array.nb_dims;
  array.data.dims = (hsize_t *)malloc(array.data.nb_dims*sizeof(hsize_t));
  for (dim = 0; dim < array.data.nb_dims; ++dim) {
    array.data.dims[dim] = array.dims[dim].nb_values;
    nb_values *= array.dims[dim].nb_values;
  }
  array.data.values.f = (float *)malloc(nb_values*sizeof(float));
  for (i = 0; i < nb_values; ++i)
    array.data.values.f[i] = 0.3 * i;

  mu_assert("Write array set.",
            AH5_write_ft_arrayset(file_id, &array));

  AH5_read_flt_dataset(file_id, "/floatingType/array/data", nb_values, &fvalues);
  for (i = 0; i < nb_values; ++i)
    mu_assert_close("Test the written real values.",
                    fvalues[i], 0.3 * i, 1.e-4);

  free(fvalues);

  /* a data on mesh */
  dataonmesh.path = "/floatingType/dataonmesh";
  dataonmesh.opt_attrs.nb_instances = 0;
  dataonmesh.nb_dims = 2;
  dataonmesh.dims = (AH5_vector_t *)malloc(dataonmesh.nb_dims*sizeof(AH5_vector_t));

  dataonmesh.dims[0].nb_values = 1;
  dataonmesh.dims[0].opt_attrs.nb_instances = 0;
  dataonmesh.dims[0].type_class = H5T_STRING;
  dataonmesh.dims[0].values.s = &meshpath;

  dataonmesh.dims[1].nb_values = 2;
  dataonmesh.dims[1].opt_attrs.nb_instances = 0;
  dataonmesh.dims[1].type_class = H5T_INTEGER;
  dataonmesh.dims[1].values.i = (int *)malloc(dataonmesh.dims[1].nb_values*sizeof(int));
  for (i = 0; i < dataonmesh.dims[1].nb_values; ++i)
    dataonmesh.dims[1].values.i[i] = i * 3;

  dataonmesh.data.type_class = H5T_FLOAT;
  dataonmesh.data.opt_attrs.nb_instances = 0;
  dataonmesh.data.nb_dims = dataonmesh.nb_dims;
  dataonmesh.data.dims = (hsize_t *)malloc(dataonmesh.data.nb_dims*sizeof(hsize_t));
  dataonmesh.data.dims[0] = 10;
  dataonmesh.data.dims[1] = dataonmesh.dims[1].nb_values;
  nb_values = 1;
  for (dim = 0; dim < dataonmesh.data.nb_dims; ++dim) nb_values *= dataonmesh.data.dims[dim];
  dataonmesh.data.values.f = (float *)malloc(nb_values*sizeof(float));
  for (i = 0; i < nb_values; ++i)
    dataonmesh.data.values.f[i] = 0.3 * i;

  mu_assert("Write dataonmesh.",
            AH5_write_ft_arrayset(file_id, &dataonmesh));

  AH5_read_flt_dataset(file_id, "/floatingType/dataonmesh/data", nb_values, &fvalues);
  for (i = 0; i < nb_values; ++i)
    mu_assert_close("Test the written real values.",
                    fvalues[i], 0.3 * i, 1.e-4);

  AH5_read_str_dataset(file_id, "/floatingType/dataonmesh/ds/dim1", 1, strlen(meshpath), &svalues);
  mu_assert_str_equal("Test the written string dims.",
                      *svalues, meshpath);

  free(fvalues);

  // Close file.
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


// Run all tests
char *all_tests()
{
  mu_run_test(test_write_single_flotingtype);
  mu_run_test(test_write_ft_vector);
  mu_run_test(test_write_ft_dataset);
  mu_run_test(test_write_ft_arrayset);
  mu_run_test(test_init_datasetx);
  mu_run_test(test_init_vector);
  mu_run_test(test_init_dataset);
  mu_run_test(test_init_arrayset);

  return MU_FINISHED_WITHOUT_ERRORS;
}


AH5_UTEST_MAIN(all_tests, tests_run);
