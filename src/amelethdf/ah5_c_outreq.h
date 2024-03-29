#ifndef AH5_C_OUTREQ_H
#define AH5_C_OUTREQ_H

#include "ah5_general.h"
#include "ah5_c_label.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AH5_ort_instance_t
{
  char            *path;
  AH5_opt_attrs_t opt_attrs;
  char            *subject;
  char            *object;
  char            *subject_name;
  char            *output;
  hsize_t         nb_elements;  // solver specific: number of elements
  unsigned int    *nb_cpes;  // solver specific: number of computational elements per point
  unsigned int    **cpes;  // solver specific: tetrahedron IDs
  double          **ccpes;  // solver specific: computational coefficients
  hsize_t         output_dims[3];  // solver specific: dimensions of the output data
  int             rank;  // solver specific: dimensionality of the output data
  float           *data;  // solver specific: output data
} AH5_ort_instance_t;

typedef struct _AH5_ort_group_t
{
  char            *path;
  AH5_opt_attrs_t opt_attrs;
  hsize_t         nb_instances;
  AH5_ort_instance_t *instances;
} AH5_ort_group_t;

typedef struct _AH5_outputrequest_t
{
  hsize_t         nb_groups;
  AH5_ort_group_t *groups;
} AH5_outputrequest_t;

AH5_PUBLIC void AH5_init_ort_instance (AH5_ort_instance_t *ort_instance);
AH5_PUBLIC void AH5_init_ort_group (AH5_ort_group_t *ort_group);
AH5_PUBLIC void AH5_init_outputrequest(AH5_outputrequest_t *outputrequest);

AH5_PUBLIC char AH5_read_ort_instance (hid_t file_id, const char *path,
                                       AH5_ort_instance_t *ort_instance);
AH5_PUBLIC char AH5_read_ort_group (hid_t file_id, const char *path, AH5_ort_group_t *ort_group);
AH5_PUBLIC char AH5_read_outputrequest (hid_t file_id, AH5_outputrequest_t *outputrequest);

AH5_PUBLIC void AH5_print_ort_instance (const AH5_ort_instance_t *ort_instance, int space);
AH5_PUBLIC void AH5_print_ort_group (const AH5_ort_group_t *ort_group, int space);
AH5_PUBLIC void AH5_print_outputrequest (const AH5_outputrequest_t *outputrequest);

AH5_PUBLIC void AH5_free_ort_instance (AH5_ort_instance_t *ort_instance);
AH5_PUBLIC void AH5_free_ort_group (AH5_ort_group_t *ort_group);
AH5_PUBLIC void AH5_free_outputrequest (AH5_outputrequest_t *outputrequest);

#ifdef __cplusplus
}
#endif

#endif // AH5_C_OUTREQ_H
