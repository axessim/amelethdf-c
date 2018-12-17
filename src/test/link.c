// Test of the link.

#include <string.h>
#include <stdio.h>
#include <ah5.h>
#include "utest.h"

#define TEST_FILE "_ah5_test.h5"

hid_t CreateTestFile()
{
  hid_t fid;
  herr_t status;
  char ah5_status;
  hid_t label_grp;
  hid_t link_grp;
  hid_t links;
  hid_t link;
  char *labels[2] = {"label subject", "label object"};

  fid = H5Fcreate(TEST_FILE, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
  if (fid < 0) return fid;
  // Create nodes
  label_grp = H5Gcreate(fid, "label", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (label_grp < 0) return label_grp;
  link_grp = H5Gcreate(fid, "link", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (link_grp < 0) return link_grp;
  links = H5Gcreate(link_grp, "link", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (links < 0) return links;
  link = H5Gcreate(links, "link_instance_0", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (link < 0) return link;

  // Label array.
  ah5_status = AH5_write_str_dataset(label_grp, "link", 2, 14, (char** const)labels);
  if (ah5_status != AH5_TRUE) return -1;

  // link attribute.s
  status = AH5_write_str_attr(link, ".", "object", "/label/link");
  if (status < 0) return status;
  status = AH5_write_int_attr(link, ".", "object_id", 1);
  if (status < 0) return status;
  status = AH5_write_str_attr(link, ".", "subject", "/label/link");
  if (status < 0) return status;
  status = AH5_write_int_attr(link, ".", "subject_id", 0);
  if (status < 0) return status;
  status = AH5_write_flt_attr(link, ".", "my attr", 1.1);
  if (status < 0) return status;

  // Release nodes
  status = H5Gclose(label_grp);
  if (status < 0) return status;
  status = H5Gclose(link);
  if (status < 0) return status;
  status = H5Gclose(links);
  if (status < 0) return status;
  status = H5Gclose(link_grp);
  if (status < 0) return status;
  return fid;
}

void Clean(const hid_t file_id)
{
  if (file_id >= 0)
  {
    if (H5Fclose(file_id) < 0)
    {
      printf("Fail to close temporary file: %s\n", TEST_FILE);
    }
  }

  if (remove(TEST_FILE) != 0)
  {
    printf("Fail to remove temporary file: %s\n", TEST_FILE);
  }
}

#define CheckFreeNReturn(test, links, message) if (!(test)) {AH5_free_link(&links); return message;}

static char *TestRead(const hid_t file_id)
{
  AH5_link_t links;
  AH5_lnk_group_t *link_grp;
  AH5_lnk_instance_t *link;
  char status;
  hsize_t iattr;

  AH5_init_link(&links);
  status = AH5_read_link(file_id, &links);
  CheckFreeNReturn(status == AH5_TRUE, links, "Fail to read links.\n");
  CheckFreeNReturn(links.nb_groups == 1, links, "Wrong number of link group.\n");
  CheckFreeNReturn(links.groups != NULL, links, "Link groups not allocated.\n");

  link_grp = &(links.groups[0]);
  CheckFreeNReturn(strcmp(link_grp->path, "/link/link") == 0, links, "Wrong link group path.\n");
  CheckFreeNReturn(link_grp->nb_instances == 1, links, "Link groups not allocated.\n");
  CheckFreeNReturn(link_grp->instances != NULL, links, "Link instances not allocated.\n");
  CheckFreeNReturn(link_grp->opt_attrs.nb_instances == 0, links,
                   "Link group must not have attributes.\n");

  link = &(link_grp->instances[0]);
  CheckFreeNReturn(strcmp(link->path, "/link/link/link_instance_0") == 0,
                   links, "Wrong link path.\n");
  CheckFreeNReturn(strcmp(link->subject, "/label/link") == 0, links, "Wrong link subject.\n");
  CheckFreeNReturn(strcmp(link->subject_name, "label subject") == 0, links,
                   "Wrong link subject label.\n");
  CheckFreeNReturn(strcmp(link->object, "/label/link") == 0, links, "Wrong link object.\n");
  CheckFreeNReturn(strcmp(link->object_name, "label object") == 0, links,
                   "Wrong link object label.\n");
  CheckFreeNReturn(link->opt_attrs.nb_instances == 3, links, "Wrong number of link attrs.\n");
  for (iattr = 0; iattr < link->opt_attrs.nb_instances; iattr++)
  {
    if (strcmp(link->opt_attrs.instances[iattr].name, "object_id") == 0)
    {
      CheckFreeNReturn(link->opt_attrs.instances[iattr].type == H5T_INTEGER, links,
                       "Wrong object_id type.\n");
      CheckFreeNReturn(link->opt_attrs.instances[iattr].value.i == 1, links,
                       "Wrong object_id value.\n");
    }
    else if (strcmp(link->opt_attrs.instances[iattr].name, "subject_id") == 0)
    {
      CheckFreeNReturn(link->opt_attrs.instances[iattr].type == H5T_INTEGER, links,
                       "Wrong subject_id type.\n");
      CheckFreeNReturn(link->opt_attrs.instances[iattr].value.i == 0, links,
                       "Wrong dubject_id value.\n");
    }
    else if (strcmp(link->opt_attrs.instances[iattr].name, "my attr") == 0)
    {
      CheckFreeNReturn(link->opt_attrs.instances[iattr].type == H5T_FLOAT, links,
                       "Wrong attribute type.\n");
      CheckFreeNReturn(link->opt_attrs.instances[iattr].value.f == 1.1f, links,
                       "Wrong attirubute value.\n");
    }
    else
    {
      CheckFreeNReturn(0, links, "Unexpected attribute.\n");
    }
  }

  return NULL;
}

typedef struct _Result
{
  int status;
  int nb_success;
  int nb_total;
} Result;

void InitResult(Result *result)
{
  result->status = EXIT_SUCCESS;
  result->nb_success = 0;
  result->nb_total = 0;
}

void CheckTestResult(const char *status, Result *result)
{
  result->nb_total++;
  if (status == NULL)
  {
    result->nb_success++;
  }
  else
  {
    printf("Test fails with message:\n  %s\n", status);
    result->status = EXIT_FAILURE;
  }
}

void DisplayResult(const Result *result)
{
  printf("Test passed: %d / %d.\n", result->nb_success, result->nb_total);
  if (result->nb_success == result->nb_total)
  {
    printf("*** Test passed successfully.\n");
  }
  else
  {
    printf("*** Test failed.\n");
  }
}

// All files must have the main function.
int main(int UNUSED(argc), char **UNUSED(argv))
{
  Result result;
  hid_t file_id;

  InitResult(&result);
  file_id = CreateTestFile();

  if (file_id < 0)
  {
    printf("Fail to open temporary test file: %s\n", TEST_FILE);
    Clean(file_id);
    return EXIT_FAILURE;
  }

  CheckTestResult(TestRead(file_id), &result);

  Clean(file_id);
  DisplayResult(&result);

  return result.status;
}
