#include "ah5_c_label.h"

// Init label dataset
void AH5_init_lbl_dataset(AH5_lbl_dataset_t *lbl_dataset)
{
  lbl_dataset->path = NULL;
  lbl_dataset->nb_items = 0;
  lbl_dataset->items = NULL;
}


// Init label category (all datasets)
void AH5_init_label(AH5_label_t *label)
{
  label->nb_datasets = 0;
  label->datasets = NULL;
}




// Read label dataset
char AH5_read_lbl_dataset(hid_t file_id, const char *path, AH5_lbl_dataset_t *lbl_dataset)
{
  H5T_class_t type_class;
  char rdata = AH5_FALSE;
  size_t length;
  int nb_dims;

  AH5_init_lbl_dataset(lbl_dataset);
  lbl_dataset->path = strdup(path);

  lbl_dataset->nb_items = 1;  // in case of single value
  if (AH5_path_valid(file_id, path))
    if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
      if (nb_dims <= 1)
        if (H5LTget_dataset_info(file_id, path, &(lbl_dataset->nb_items), &type_class, &length) >= 0)
          if (type_class == H5T_STRING)
            if(AH5_read_str_dataset(file_id, path, lbl_dataset->nb_items, length, &(lbl_dataset->items)))
              rdata = AH5_TRUE;
  if (!rdata)
  {
    AH5_print_err_dset(AH5_C_LABEL, path);
    lbl_dataset->nb_items = 0;
  }
  return rdata;
}


// Read label category (all datasets)
char AH5_read_label(hid_t file_id, AH5_label_t *label)
{
  char path[AH5_ABSOLUTE_PATH_LENGTH], rdata = AH5_TRUE;
  AH5_children_t children;
  hsize_t i;

  AH5_init_label(label);

  if (AH5_path_valid(file_id, AH5_C_LABEL))
  {
    children = AH5_read_children_name(file_id, AH5_C_LABEL);
    label->nb_datasets = children.nb_children;
    if (children.nb_children > 0)
    {
      label->datasets = (AH5_lbl_dataset_t *) malloc((size_t) children.nb_children * sizeof(
                          AH5_lbl_dataset_t));
      for (i = 0; i < children.nb_children; i++)
      {
        strcpy(path, AH5_C_LABEL);
        strcat(path, children.childnames[i]);
        if(!AH5_read_lbl_dataset(file_id, path, label->datasets + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_LABEL, AH5_C_LABEL);
    rdata = AH5_FALSE;
  }
  return rdata;
}




// Print label dataset
void AH5_print_lbl_dataset (const AH5_lbl_dataset_t *lbl_dataset, int space)
{
  hsize_t i;

  printf("%*sName: %s\n", space, "", AH5_get_name_from_path(lbl_dataset->path));
  for (i = 0; i < lbl_dataset->nb_items; i++)
    printf("%*s%s\n", space + 3, "", lbl_dataset->items[i]);
  printf("\n");
}


// Print label category (all datasets)
void AH5_print_label(const AH5_label_t *label)
{
  hsize_t i;

  printf("##################################  Label  ###################################\n\n");
  for (i = 0; i < label->nb_datasets; i++)
    AH5_print_lbl_dataset(&(label->datasets[i]), 0);
  printf("\n");
}




// Free memory used by structure lbl_dataset
void AH5_free_lbl_dataset (AH5_lbl_dataset_t *lbl_dataset)
{
  free(lbl_dataset->path);
  if (lbl_dataset->items != NULL)
  {
    free(lbl_dataset->items[0]);
    free(lbl_dataset->items);
  }
  AH5_init_lbl_dataset(lbl_dataset);
}


// Free memory used by structure label (including datasets)
void AH5_free_label (AH5_label_t *label)
{
  hsize_t i;

  if (label->datasets != NULL)
  {
    for (i = 0; i < label->nb_datasets; i++)
      AH5_free_lbl_dataset(label->datasets + i);
    free(label->datasets);
  }
  AH5_init_label(label);
}
