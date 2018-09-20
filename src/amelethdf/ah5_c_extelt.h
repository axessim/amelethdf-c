#ifndef AH5_C_EXTELT_H
#define AH5_C_EXTELT_H

#include "ah5_general.h"

#ifndef _MSC_VER
#include <unistd.h>
#else
#define R_OK 4
#define F_OK 0
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** A set of external element. */
typedef struct _AH5_eet_dataset_t
{
  char            *principle_file_path;  /** Path of the principle file. */
  char            *path;         /** Path of the externalElement in the principle file. */
  hid_t           *file_id;      /** The ids of the files. */
  hsize_t         nb_eed_items;  /** Number of external elements. */
  /** External content: (internal path, external file path, external path). */
  char            **eed_items;
} AH5_eet_dataset_t;

/** The external elements in a file. */
typedef struct _AH5_external_element_t
{
  hsize_t         nb_datasets;  /** Number of sets of external elements. */
  AH5_eet_dataset_t *datasets;  /** The sets of external elements. */
} AH5_external_element_t;

AH5_PUBLIC char AH5_read_eet_dataset (hid_t file_id, const char *path,
                                      AH5_eet_dataset_t *eet_dataset);
AH5_PUBLIC char AH5_read_external_element (hid_t file_id, AH5_external_element_t *external_element);

AH5_PUBLIC void AH5_print_eet_dataset (const AH5_eet_dataset_t *eet_dataset, int space);
AH5_PUBLIC void AH5_print_external_element (const AH5_external_element_t *external_element);

AH5_PUBLIC void AH5_free_eet_dataset (AH5_eet_dataset_t *eet_dataset);
AH5_PUBLIC void AH5_free_external_element (AH5_external_element_t *external_element);

AH5_PUBLIC char AH5_open_external_files (AH5_eet_dataset_t *eet_dataset);
AH5_PUBLIC char AH5_close_external_files (AH5_eet_dataset_t *eet_dataset);

/**
 * Test if the provided path is an external element.
 *
 *  @param externals The external elements.
 *  @param path The tested path.
 *  @param file_id The id of the external file, if external.
 *  @param external_path The path of the external element, if external.
 *
 *  @return AH5_TRUE if external else AH5_FALSE.
 */
AH5_PUBLIC char AH5_is_external_element(
    const AH5_external_element_t* externals, const char* path,
    hid_t* file_id, char const** external_path);


/**
 * The path of a file in the same folder than the provided one.
 *
 *  @param fpath1 A file path in the folder where is the other one.
 *  @param fname2 The file name.
 *  @param fpath2 The path of the file, if NULL only the required size is returned.
 *  @param size The maximal size of the fpath2.
 *
 *  @return The required size of the path.
 */
AH5_PUBLIC size_t AH5_file_path_next_to(const char* fpath1, const char* fname2, char* fpath2, size_t size);

#ifdef __cplusplus
}
#endif

#endif // AH5_C_EXTELT_H
