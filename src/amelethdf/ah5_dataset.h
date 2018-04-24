#ifndef AH5_DATASET_H
#define AH5_DATASET_H

#include "ah5_general.h"

#ifdef __cplusplus
extern "C" {
#endif

AH5_PUBLIC char AH5_read_int_dataset(hid_t file_id, const char *path, const hsize_t mn,
                                     int **rdata);
AH5_PUBLIC char AH5_read_flt_dataset(hid_t file_id, const char *path, const hsize_t mn,
                                     float **rdata);
AH5_PUBLIC char AH5_read_cpx_dataset(hid_t file_id, const char *path, const hsize_t mn,
                                     AH5_complex_t **rdata);
AH5_PUBLIC char AH5_read_str_dataset(hid_t file_id, const char *path, const hsize_t mn,
                                     size_t length, char ***rdata);

AH5_PUBLIC char AH5_write_char_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                       const char *wdata);
AH5_PUBLIC char AH5_write_flat_str_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                           const size_t slen, const char *buf);
AH5_PUBLIC char AH5_write_int_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                      const int *wdata);
AH5_PUBLIC char AH5_write_long_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                       const long *wdata);
AH5_PUBLIC char AH5_write_flt_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                      const float *wdata);
AH5_PUBLIC char AH5_write_cpx_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                      const AH5_complex_t *wdata);

/**
 * Write a dataset made of c-strings of same length.
 *
 * @param loc_id id of the location
 * @param dset_name name of the dataset
 * @param len number of strings in the dataset
 * @param slen Length (returned by strlen) of the strings
 * @param wdata data to be stored.
 *
 * @return status of writing: AH5_TRUE if success.
 */
AH5_PUBLIC char AH5_write_str_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                      const size_t slen, char **wdata);


/**
 * Write a dataset made of c-strings of same length written one after the other.
 *
 * @param loc_id id of the location
 * @param dset_name name of the dataset
 * @param len number of strings in the dataset
 * @param slen Length (returned by strlen) of the strings
 * @param wdata data to be stored.
 *
 * @return status of writing: AH5_TRUE if success.
 */
AH5_PUBLIC char AH5_write_flatstr_dataset(hid_t loc_id, const char *dset_name, const hsize_t len,
                                          const size_t slen, char *wdata);

AH5_PUBLIC char AH5_write_int_array(hid_t loc_id, const char *dset_name, const int rank,
                                    const hsize_t dims[], const int *wdata);
AH5_PUBLIC char AH5_write_flt_array(hid_t loc_id, const char *dset_name, const int rank,
                                    const hsize_t dims[], const float *wdata);
AH5_PUBLIC char AH5_write_cpx_array(hid_t loc_id, const char *dset_name, const int rank,
                                    const hsize_t dims[], const AH5_complex_t *wdata);
AH5_PUBLIC char AH5_write_str_array(hid_t loc_id, const char *dset_name, const int rank,
                                    const hsize_t dims[], const char *wdata);

#ifdef __cplusplus
}
#endif

#endif // AH5_DATASET_H
