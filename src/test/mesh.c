// test path tools

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <ah5.h>
#include <hdf5.h>
#include "utest.h"

//! Test suite counter.
int tests_run = 0;


// Build a misformed umesh with elementNodes and elementTypes of wrong type.
char build_misformed_umesh(const char* fpath)
{
  char success = AH5_FALSE;
  const int etypes[1] = {1};
  const long enodes[2] = {0, 1};
  hsize_t i;
  hid_t file_id;
  hid_t cmesh_id;
  hid_t gmesh_id;
  hid_t mesh_id;
  AH5_mesh_t mesh;
  AH5_umesh_t* umesh = NULL;

  if (&mesh == AH5_init_mesh(&mesh, 1)) {
    if (mesh.groups == AH5_init_msh_group(mesh.groups, "/mesh/mesh", 1, 0)) {
      if (mesh.groups->msh_instances == AH5_init_msh_instance(
              mesh.groups->msh_instances, "/mesh/mesh/mesh", MSH_UNSTRUCTURED)) {
        umesh = &mesh.groups->msh_instances->data.unstructured;

        // init nodes
        if (umesh == AH5_init_umesh(umesh, 0, 0, 2, 0, 0, 0)) {
          for (i = 0; i < umesh->nb_nodes[0]*umesh->nb_nodes[1]; i++)
            umesh->nodes[i] = i;

          file_id = AH5_create(fpath, H5F_ACC_TRUNC, NULL);
          if (file_id >= 0) {
            if (AH5_TRUE == AH5_write_mesh(file_id, &mesh)) {
              cmesh_id = H5Gopen(file_id, "mesh", H5P_DEFAULT);
              if (cmesh_id >= 0) {
                gmesh_id = H5Gopen(cmesh_id, "mesh", H5P_DEFAULT);
                if (gmesh_id >= 0) {
                  mesh_id = H5Gopen(gmesh_id, "mesh", H5P_DEFAULT);
                  if (mesh_id >= 0) {
                    if (H5Ldelete(mesh_id, "elementTypes", H5P_DEFAULT) >= 0 &&
                        H5Ldelete(mesh_id, "elementNodes", H5P_DEFAULT) >= 0) {
                      if (AH5_write_int_dataset(mesh_id, "elementTypes", 1, etypes) == AH5_TRUE &&
                          AH5_write_long_dataset(mesh_id, "elementNodes", 2, enodes) == AH5_TRUE) {
                        success = AH5_TRUE;
                      }
                    }
                    H5Gclose(mesh_id);
                  }
                  H5Gclose(gmesh_id);
                }
                H5Gclose(cmesh_id);
              }
            }
            AH5_close(file_id);
          }
        }
      }
    }
    AH5_free_mesh(&mesh);
  }
  return success;
}


//! Build a simple unstructured mesh two tetra,
void build_umesh_1(AH5_umesh_t *umesh)
{
  hsize_t i;
  AH5_usom_table_t *som;

  // init nodes
  AH5_init_umesh(umesh, 11, 3, 5, 1, 0, 3);
  /* umesh->nb_nodes[0] = 5; */
  /* umesh->nb_nodes[1] = 3; */
  /* umesh->nodes = (float *)malloc(umesh->nb_nodes[0]*umesh->nb_nodes[1]*sizeof(float)); */
  for (i = 0; i < umesh->nb_nodes[0]*umesh->nb_nodes[1]; i++)
    umesh->nodes[i] = i;
  // init element
  /* umesh->nb_elementtypes = 3; */
  /* umesh->elementtypes = (char *)malloc(umesh->nb_elementtypes*sizeof(char)); */
  umesh->elementtypes[0] = AH5_UELE_TETRA4;
  umesh->elementtypes[1] = AH5_UELE_TETRA4;
  umesh->elementtypes[2] = AH5_UELE_TRI3;

  /* umesh->nb_elementnodes = 4+4+3; */
  /* umesh->elementnodes = (int *)malloc(umesh->nb_elementnodes*sizeof(int)); */
  umesh->elementnodes[0] = 0;
  umesh->elementnodes[1] = 1;
  umesh->elementnodes[2] = 2;
  umesh->elementnodes[3] = 3;

  umesh->elementnodes[4] = 1;
  umesh->elementnodes[5] = 2;
  umesh->elementnodes[6] = 3;
  umesh->elementnodes[7] = 4;

  umesh->elementnodes[8] = 1;
  umesh->elementnodes[9] = 2;
  umesh->elementnodes[10] = 3;

  // build group
  /* umesh->nb_groups = 1; */
  /* umesh->groups = (AH5_ugroup_t *) malloc(umesh->nb_groups * sizeof(AH5_ugroup_t)); */
  umesh->groups[0].path = (char *) malloc(5 * sizeof(char));
  strcpy(umesh->groups[0].path, "name");
  umesh->groups[0].entitytype = AH5_GROUP_FACE;
  umesh->groups[0].nb_groupelts = 1;
  umesh->groups[0].groupelts = (int *) malloc(umesh->groups[0].nb_groupelts * sizeof(int));
  umesh->groups[0].groupelts[0] = 2;

  // group of group
  /* umesh->nb_groupgroups = 0; */
  /* umesh->groupgroups = NULL; */

  // selector on mesh
  /* umesh->nb_som_tables = 0; */
  /* umesh->som_tables = NULL; */

  som = umesh->som_tables;
  AH5_init_usom_table(som, "pie", 2, SOM_POINT_IN_ELEMENT);
  som->data.pie.indices[0] = 0;
  som->data.pie.indices[1] = 1;
  som->data.pie.vectors[0][0] = 2;
  som->data.pie.vectors[0][1] = 3;
  som->data.pie.vectors[0][2] = 4;
  som->data.pie.vectors[1][0] = 5;
  som->data.pie.vectors[1][1] = 6;
  som->data.pie.vectors[1][2] = 7;

  som = umesh->som_tables + 1;
  AH5_init_usom_table(som, "edge", 3, SOM_EDGE);
  som->data.ef.items[0] = 10;
  som->data.ef.items[1] = 11;
  som->data.ef.items[2] = 12;
  som->data.ef.items[3] = 13;
  som->data.ef.items[4] = 14;
  som->data.ef.items[5] = 15;

  som = umesh->som_tables + 2;
  AH5_init_usom_table(som, "face", 4, SOM_FACE);
  som->data.ef.items[0] = 100;
  som->data.ef.items[1] = 101;
  som->data.ef.items[2] = 102;
  som->data.ef.items[3] = 103;
  som->data.ef.items[4] = 104;
  som->data.ef.items[5] = 105;
  som->data.ef.items[6] = 106;
  som->data.ef.items[7] = 107;
}


//! Test copy mesh
char *test_copy_umesh()
{
  AH5_umesh_t msh1, msh2;

  build_umesh_1(&msh1);

  mu_assert("copy umesh", AH5_copy_umesh(&msh2, &msh1) != NULL);

  return MU_FINISHED_WITHOUT_ERRORS;
}

//! Test init functions function.
char *test_init_functions()
{
  AH5_mesh_t mesh;
  AH5_msh_group_t msh_group;
  AH5_umesh_t umesh;
  AH5_smesh_t smesh;
  AH5_msh_instance_t msh;

  /*Check mesh*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_mesh(NULL, 1), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_mesh(&mesh, 0), &mesh);
  mu_assert_eq("check mesh field", mesh.nb_groups, 0);
  mu_assert_eq_ptr("check mesh field", mesh.groups, NULL);

  mu_assert_eq_ptr(
    "init mesh", AH5_init_mesh(&mesh, 5), &mesh);
  mu_assert_eq("check mesh field", mesh.nb_groups, 5);
  mu_assert_ne("check mesh field", mesh.groups, NULL);

  AH5_free_mesh(&mesh);
  mu_assert_eq("check mesh field", mesh.nb_groups, 0);
  mu_assert_eq_ptr("check mesh field", mesh.groups, NULL);

  /*Check mesh group*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_msh_group(NULL, NULL, 1, 1), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_msh_group(&msh_group, NULL, 0, 0), &msh_group);
  mu_assert_eq_ptr("check mesh field", msh_group.path, NULL);
  mu_assert_eq("check mesh field", msh_group.nb_msh_instances, 0);
  mu_assert_eq_ptr("check mesh field", msh_group.msh_instances, NULL);
  mu_assert_eq("check mesh field", msh_group.nb_mlk_instances, 0);
  mu_assert_eq_ptr("check mesh field", msh_group.mlk_instances, NULL);

  mu_assert_eq_ptr(
    "init mesh", AH5_init_msh_group(&msh_group, "group name", 3, 2), &msh_group);
  mu_assert_str_equal("check mesh field", msh_group.path, "group name");
  mu_assert_eq("check mesh field", msh_group.nb_msh_instances, 3);
  mu_assert_ne("check mesh field", msh_group.msh_instances, NULL);
  mu_assert_eq("check mesh field", msh_group.nb_mlk_instances, 2);
  mu_assert_ne("check mesh field", msh_group.mlk_instances, NULL);

  AH5_free_msh_group(&msh_group);
  mu_assert_eq_ptr("check mesh field", msh_group.path, NULL);
  mu_assert_eq("check mesh field", msh_group.nb_msh_instances, 0);
  mu_assert_eq_ptr("check mesh field", msh_group.msh_instances, NULL);
  mu_assert_eq("check mesh field", msh_group.nb_mlk_instances, 0);
  mu_assert_eq_ptr("check mesh field", msh_group.mlk_instances, NULL);

  /*Check umesh*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_umesh(NULL, 1, 1, 1, 1, 1, 1), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_umesh(&umesh, 0, 0, 0, 0, 0, 0), &umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 0);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 0);
  mu_assert_eq("check mesh field", umesh.nb_som_tables, 0);
  mu_assert_eq_ptr("check mesh field", umesh.som_tables, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_umesh(&umesh, 1, 2, 3, 0, 0, 0), &umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 1);
  mu_assert_ne("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 2);
  mu_assert_ne("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 3);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 3);
  mu_assert_eq("check mesh field", umesh.nb_groups, 0);
  mu_assert_eq_ptr("check mesh field", umesh.groups, NULL);
  mu_assert_eq("check mesh field", umesh.nb_som_tables, 0);
  mu_assert_eq_ptr("check mesh field", umesh.som_tables, NULL);
  AH5_free_umesh(&umesh);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_umesh(&umesh, 0, 0, 3, 0, 0, 0), &umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 3);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 3);
  mu_assert_eq("check mesh field", umesh.nb_groups, 0);
  mu_assert_eq_ptr("check mesh field", umesh.groups, NULL);
  mu_assert_eq("check mesh field", umesh.nb_som_tables, 0);
  mu_assert_eq_ptr("check mesh field", umesh.som_tables, NULL);
  AH5_free_umesh(&umesh);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_umesh(&umesh, 0, 0, 3, 0, 0, 2), &umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 3);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 3);
  mu_assert_eq("check mesh field", umesh.nb_groups, 0);
  mu_assert_eq_ptr("check mesh field", umesh.groups, NULL);
  mu_assert_eq("check mesh field", umesh.nb_som_tables, 2);
  mu_assert_ne("check mesh field", umesh.som_tables, NULL);

  AH5_free_umesh(&umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 0);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 0);
  mu_assert_eq("check mesh field", umesh.nb_som_tables, 0);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_umesh(&umesh, 1, 2, 3, 4, 3, 2), &umesh);
  mu_assert_eq("check field", umesh.nb_elementnodes, 1);
  mu_assert_ne("check field", umesh.elementnodes, NULL);
  mu_assert_eq("check field", umesh.nb_elementtypes, 2);
  mu_assert_ne("check field", umesh.elementtypes, NULL);
  mu_assert_eq("check field", umesh.nb_nodes[0], 3);
  mu_assert_eq("check field", umesh.nb_nodes[1], 3);
  mu_assert_eq("check field", umesh.nb_groups, 4);
  mu_assert_ne("check field", umesh.groups, NULL);
  mu_assert_eq("check field", umesh.nb_som_tables, 2);
  mu_assert_ne("check field", umesh.som_tables, NULL);

  /*Check umesh group*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_ugroup(NULL, NULL, 1, AH5_GROUP_EDGE), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups, "group name", 1, AH5_GROUP_NODE),
    umesh.groups);
  mu_assert_str_equal("check field", umesh.groups->path, "group name");
  mu_assert_eq("check field", umesh.groups->entitytype, AH5_GROUP_NODE);
  mu_assert_eq("check field", umesh.groups->nb_groupelts, 1);
  mu_assert_ne("check field", umesh.groups->groupelts, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups+1, "group 1", 1, AH5_GROUP_EDGE),
    umesh.groups + 1);
  mu_assert_str_equal("check field", umesh.groups[1].path, "group 1");
  mu_assert_eq("check field", umesh.groups[1].entitytype, AH5_GROUP_EDGE);
  mu_assert_eq("check field", umesh.groups[1].nb_groupelts, 1);
  mu_assert_ne("check field", umesh.groups[1].groupelts, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups+2, "group 2", 10, AH5_GROUP_FACE),
    umesh.groups + 2);
  mu_assert_str_equal("check field", umesh.groups[2].path, "group 2");
  mu_assert_eq("check field", umesh.groups[2].entitytype, AH5_GROUP_FACE);
  mu_assert_eq("check field", umesh.groups[2].nb_groupelts, 10);
  mu_assert_ne("check field", umesh.groups[2].groupelts, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups+3, "group 3", 1, AH5_GROUP_VOLUME),
    umesh.groups + 3);
  mu_assert_str_equal("check field", umesh.groups[3].path, "group 3");
  mu_assert_eq("check field", umesh.groups[3].entitytype, AH5_GROUP_VOLUME);
  mu_assert_eq("check field", umesh.groups[3].nb_groupelts, 1);
  mu_assert_ne("check field", umesh.groups[3].groupelts, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups+3, NULL, 1, AH5_GROUP_ENTITYTYPE_UNDEF),
    NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_ugroup(umesh.groups+3, NULL, 1, AH5_GROUP_ENTITYTYPE_INVALID),
    NULL);

  /*Check group of group*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_groupgroup(NULL, NULL, 1, 1), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_groupgroup(umesh.groupgroups, NULL, 0, 0),
    umesh.groupgroups);
  mu_assert_eq_ptr("check field", umesh.groupgroups[0].path, NULL);
  mu_assert_eq("check field", umesh.groupgroups[0].nb_groupgroupnames, 0);
  mu_assert_eq_ptr("check field", umesh.groupgroups[0].groupgroupnames, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_groupgroup(umesh.groupgroups, "gg0", 10,
                                      0), /*default AH5 groups' name length*/
    umesh.groupgroups);
  mu_assert_str_equal("check field", umesh.groupgroups[0].path, "gg0");
  mu_assert_eq("check field", umesh.groupgroups[0].nb_groupgroupnames, 10);
  mu_assert_ne("check field", umesh.groupgroups[0].groupgroupnames, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_groupgroup(umesh.groupgroups+1, "gg1", 50, 100), /*user groups' name length*/
    umesh.groupgroups+1);
  mu_assert_str_equal("check field", umesh.groupgroups[1].path, "gg1");
  mu_assert_eq("check field", umesh.groupgroups[1].nb_groupgroupnames, 50);
  mu_assert_ne("check field", umesh.groupgroups[1].groupgroupnames, NULL);

  /*Check selector on mesh*/
  mu_assert_eq_ptr(
      "empty mesh", AH5_init_usom_table(umesh.som_tables, "som1", 2, SOM_POINT_IN_ELEMENT),
      umesh.som_tables);
  mu_assert_str_equal("check field", umesh.som_tables[0].path, "som1");
  mu_assert_equal("check field", umesh.som_tables[0].type, SOM_POINT_IN_ELEMENT);
  mu_assert_equal("check field", umesh.som_tables[0].data.pie.nb_dims, 3);
  mu_assert_equal("check field", umesh.som_tables[0].data.pie.nb_points, 2);
  mu_assert_ne("check field", umesh.som_tables[0].data.pie.indices, NULL);
  mu_assert_ne("check field", umesh.som_tables[0].data.pie.vectors, NULL);

  mu_assert_eq_ptr(
      "empty mesh", AH5_init_usom_table(umesh.som_tables+1, "som2", 3, SOM_EDGE),
      umesh.som_tables+1);
  mu_assert_str_equal("check field", umesh.som_tables[1].path, "som2");
  mu_assert_equal("check field", umesh.som_tables[1].type, SOM_EDGE);
  mu_assert_equal("check field", umesh.som_tables[1].data.ef.dims[0], 3);
  mu_assert_equal("check field", umesh.som_tables[1].data.ef.dims[1], 2);
  mu_assert_ne("check field", umesh.som_tables[1].data.ef.items, NULL);

  /*release mesh*/
  AH5_free_umesh(&umesh);
  mu_assert_eq("check mesh field", umesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementnodes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check mesh field", umesh.elementtypes, NULL);
  mu_assert_eq("check mesh field", umesh.nb_nodes[0], 0);
  mu_assert_eq("check mesh field", umesh.nb_nodes[1], 0);

  /*Check smesh*/
  mu_assert_eq_ptr(
    "null", AH5_init_smesh(NULL, 1, 1, 1), NULL);

  mu_assert_eq_ptr(
    "empty", AH5_init_smesh(&smesh, 0, 0, 0), &smesh);
  mu_assert_eq("check field", smesh.nb_groups, 0);
  mu_assert_eq_ptr("check field", smesh.groups, NULL);

  mu_assert_eq_ptr(
    "empty", AH5_init_smesh(&smesh, 4, 2, 0), &smesh);
  mu_assert_eq("check field", smesh.nb_groups, 4);
  mu_assert_ne("check field", smesh.groups, NULL);

  /*Check axis*/
  mu_assert_eq_ptr(
    "null", AH5_init_axis(NULL, 1), NULL);

  mu_assert_eq_ptr(
    "null", AH5_init_axis(&smesh.x, 0), &smesh.x);
  mu_assert_eq("check field", smesh.x.nb_nodes, 0);
  mu_assert_eq_ptr("check field", smesh.x.nodes, NULL);

  mu_assert_eq_ptr(
    "null", AH5_init_axis(&smesh.x, 10), &smesh.x);
  mu_assert_eq("check field", smesh.x.nb_nodes, 10);
  mu_assert_ne("check field", smesh.x.nodes, NULL);


  /*Check smesh group*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_sgroup(NULL, NULL, 1, AH5_GROUP_EDGE), NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups, "group 0", 1, AH5_GROUP_NODE),
    smesh.groups);
  mu_assert_str_equal("check field", smesh.groups->path, "group 0");
  mu_assert_eq("check field", smesh.groups->entitytype, AH5_GROUP_NODE);
  mu_assert_eq("check field", smesh.groups->dims[0], 1);
  mu_assert_ne("check field", smesh.groups->elements, NULL);
  mu_assert_eq_ptr("check field", smesh.groups->normals, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups+1, "group 1", 1, AH5_GROUP_EDGE),
    smesh.groups + 1);
  mu_assert_str_equal("check field", smesh.groups[1].path, "group 1");
  mu_assert_eq("check field", smesh.groups[1].entitytype, AH5_GROUP_EDGE);
  mu_assert_eq("check field", smesh.groups[1].dims[0], 1);
  mu_assert_ne("check field", smesh.groups[1].elements, NULL);
  mu_assert_eq_ptr("check field", smesh.groups[1].normals, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups+2, "group 2", 10, AH5_GROUP_FACE),
    smesh.groups + 2);
  mu_assert_str_equal("check field", smesh.groups[2].path, "group 2");
  mu_assert_eq("check field", smesh.groups[2].entitytype, AH5_GROUP_FACE);
  mu_assert_eq("check field", smesh.groups[2].dims[0], 10);
  mu_assert_ne("check field", smesh.groups[2].elements, NULL);
  mu_assert_ne("check field", smesh.groups[2].normals, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups+3, "group 3", 1, AH5_GROUP_VOLUME),
    smesh.groups + 3);
  mu_assert_str_equal("check field", smesh.groups[3].path, "group 3");
  mu_assert_eq("check field", smesh.groups[3].entitytype, AH5_GROUP_VOLUME);
  mu_assert_eq("check field", smesh.groups[3].dims[0], 1);
  mu_assert_ne("check field", smesh.groups[3].elements, NULL);
  mu_assert_eq_ptr("check field", smesh.groups[3].normals, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups+3, NULL, 1, AH5_GROUP_ENTITYTYPE_UNDEF),
    NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_sgroup(smesh.groups+3, NULL, 1, AH5_GROUP_ENTITYTYPE_INVALID),
    NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_groupgroup(smesh.groupgroups, "gg0", 10,
                                      0), /*default AH5 groups' name length*/
    smesh.groupgroups);
  mu_assert_str_equal("check field", smesh.groupgroups[0].path, "gg0");
  mu_assert_eq("check field", smesh.groupgroups[0].nb_groupgroupnames, 10);
  mu_assert_ne("check field", smesh.groupgroups[0].groupgroupnames, NULL);

  mu_assert_eq_ptr(
    "empty mesh", AH5_init_groupgroup(smesh.groupgroups+1, "gg1", 50, 100), /*user groups' name length*/
    smesh.groupgroups+1);
  mu_assert_str_equal("check field", smesh.groupgroups[1].path, "gg1");
  mu_assert_eq("check field", smesh.groupgroups[1].nb_groupgroupnames, 50);
  mu_assert_ne("check field", smesh.groupgroups[1].groupgroupnames, NULL);

  /*release mesh*/
  AH5_free_smesh(&smesh);
  mu_assert_eq("check field", smesh.nb_groups, 0);
  mu_assert_eq_ptr("check field", smesh.groups, NULL);

  /*Check mesh instance*/
  mu_assert_eq_ptr(
    "null mesh", AH5_init_msh_instance(NULL, NULL, MSH_INVALID), NULL);

  mu_assert_eq_ptr(
    "null mesh", AH5_init_msh_instance(&msh, "mesh", MSH_UNSTRUCTURED), &msh);
  mu_assert_str_equal("check field", msh.path, "mesh");
  mu_assert_eq("check field", msh.type, MSH_UNSTRUCTURED);

  AH5_free_msh_instance(&msh);
  mu_assert_eq_ptr("check field", msh.path, NULL);
  mu_assert_eq("check field", msh.type, MSH_INVALID);

  mu_assert_eq_ptr(
    "null mesh", AH5_init_msh_instance(&msh, "mesh", MSH_STRUCTURED), &msh);
  mu_assert_str_equal("check field", msh.path, "mesh");
  mu_assert_eq("check field", msh.type, MSH_STRUCTURED);

  AH5_free_msh_instance(&msh);
  mu_assert_eq_ptr("check field", msh.path, NULL);
  mu_assert_eq("check field", msh.type, MSH_INVALID);


  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Test write groupgroup function.
char *test_write_groupgroup()
{
#define DIM0 2
#define LENGTH 13
  AH5_groupgroup_t gg;
  hid_t file_id, filetype, memtype, space, dset;
  char **groupgroupnames;
  char **rdata;
  size_t sdim;
  hsize_t i;
  hsize_t dims[1];

  file_id = AH5_auto_test_file();

  // Chek some bad usages.
  gg.path = NULL;
  gg.nb_groupgroupnames = 0;
  gg.groupgroupnames = NULL;
  mu_assert_true("empty",
                 AH5_write_groupgroup(file_id, NULL, 0));

  groupgroupnames = (char **) malloc(DIM0 * sizeof(char *));
  groupgroupnames[0] = (char *) malloc(LENGTH * sizeof(char));
  groupgroupnames[1] = (char *) malloc(LENGTH * sizeof(char));
  strcpy(groupgroupnames[0], "group_name_1");
  strcpy(groupgroupnames[1], "group_name_2");
  AH5_write_str_dataset(file_id, "dataset_name", DIM0, LENGTH, groupgroupnames);

  // build a groupgroup with relative path.
  gg.path = "gg_name";
  gg.nb_groupgroupnames = 2;
  gg.groupgroupnames = groupgroupnames;
  mu_assert_true("Write valide data.", AH5_write_groupgroup(file_id, &gg, 1));

  // Test the written data using hdf5 API.
  dset = H5Dopen(file_id, "/groupGroup/gg_name", H5P_DEFAULT);
  filetype = H5Dget_type(dset);
  sdim = H5Tget_size(filetype);
  space = H5Dget_space(dset);
  H5Sget_simple_extent_dims(space, dims, NULL);
  rdata = (char **) malloc(dims[0] * sizeof (char *));
  rdata[0] = (char *) malloc(dims[0] * sdim * sizeof (char));
  for (i=1; i<dims[0]; i++)
    rdata[i] = rdata[0] + i * sdim;
  memtype = H5Tcopy(H5T_C_S1);
  mu_assert("HDF5 error in H5Tset_size.", H5Tset_size(memtype, sdim) >= 0);
  mu_assert("HDF5 error in H5Dread.",
            H5Dread(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata[0]) >= 0);
  for (i = 0; i < dims[0]; i++)
    mu_assert_str_equal("Check the groupGroup.",
                        rdata[i], groupgroupnames[i]);
  // Release resources.
  free(groupgroupnames[0]);
  free(groupgroupnames[1]);
  free(groupgroupnames);
  free(rdata[0]);
  free(rdata);
  mu_assert("HDF5 error in H5Dclose.", H5Dclose(dset) >= 0);
  mu_assert("HDF5 error in H5Sclose.", H5Sclose(space) >= 0);
  mu_assert("HDF5 error in H5Tclose.", H5Tclose(filetype) >= 0);
  mu_assert("HDF5 error in H5Tclose.", H5Tclose(memtype) >= 0);

  // Close and release.
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Test function to write unstructured mesh group.
char *test_write_unstructured_mesh_group()
{
  hid_t file_id;
  AH5_ugroup_t ugrp;
  hsize_t  i;

  file_id = AH5_auto_test_file();

  // Check some bad usages.
  ugrp.path = NULL;
  ugrp.groupelts = NULL;
  ugrp.nb_groupelts = 0;

  mu_assert_true("Negative numbre of groups.", AH5_write_ugroup(file_id, &ugrp, 0));
  mu_assert_false("Empty group.", AH5_write_ugroup(file_id, &ugrp, 1));

  // Write a simple group by relative path name.
  ugrp.path = "grp_name";
  ugrp.entitytype = AH5_GROUP_FACE;
  ugrp.nb_groupelts = 10;
  ugrp.groupelts = (int *)malloc(ugrp.nb_groupelts*sizeof(int));
  for (i = 0; i < ugrp.nb_groupelts; i++)
    ugrp.groupelts[i] = i;

  mu_assert_true(
      "Write a simple group by relative path [1].", AH5_write_ugroup(file_id, &ugrp, 1));
  mu_assert("Check 'group' category nodes in file [1].", AH5_path_valid(file_id, "/group"));
  mu_assert("Check 'group' nodes in file [1].", AH5_path_valid(file_id, "/group/grp_name"));


  // Write a simple group by abs path name.
  ugrp.path = "/mesh/$mesh_group/$mesh_name/group/grp_name_2";

  mu_assert_true(
      "Write a simple group by relative path [2].", AH5_write_ugroup(file_id, &ugrp, 1));
  mu_assert("Check 'group' category nodes in file [2].", AH5_path_valid(file_id, "/group"));
  mu_assert("Check 'group' nodes in file [2].", AH5_path_valid(file_id, "/group/grp_name_2"));

  // Close and release.
  free(ugrp.groupelts);
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! test umesh write function
char *test_write_umesh()
{
  AH5_umesh_t umesh;
  AH5_msh_instance_t mesh;
  hid_t file_id = -1;
  hid_t loc_id;

  // Bad used
  mu_assert("Can not write an empty umesh!",
            !AH5_write_umesh(file_id, NULL));

  // Test on simple unstructured mesh
  file_id = AH5_auto_test_file();
  build_umesh_1(&umesh);

  // Write the mesh into '/mesh' node.
  loc_id = H5Gcreate(file_id, "/mesh", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 2);
  mu_assert("Write an simple umesh.", AH5_write_umesh(loc_id, &umesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 2);
  H5Gclose(loc_id);

  // Check if all the mesh parts are written.
  mu_assert("Check 'nodes' nodes in file.", AH5_path_valid(file_id, "/mesh/nodes"));
  mu_assert("Check 'elementNodes' nodes in file.", AH5_path_valid(file_id, "/mesh/elementNodes"));
  mu_assert("Check 'elementTypes' nodes in file.", AH5_path_valid(file_id, "/mesh/elementTypes"));
  mu_assert("Check 'groupGroup' nodes in file.", AH5_path_valid(file_id, "/mesh/groupGroup"));
  mu_assert("Check 'group' nodes in file.", AH5_path_valid(file_id, "/mesh/group"));
  mu_assert("Check 'group/name' nodes in file.", AH5_path_valid(file_id, "/mesh/group/name"));

  //TODO: Used hdf5 API to check the behaviours not the read function.
  // Now read the mesh
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);
  mu_assert("The written mesh is readable.", AH5_read_msh_instance(file_id, "/mesh", &mesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);

  // Check some value
  mu_assert_equal("Check mesh type", mesh.type, MSH_UNSTRUCTURED);
  mu_assert_equal("Check nodes size", mesh.data.unstructured.nb_nodes[0], 5);
  mu_assert_equal("Check nodes size", mesh.data.unstructured.nb_nodes[1], 3);
  mu_assert_str_equal("Check the mesh path.", mesh.path, "/mesh");

  // Close and release
  AH5_free_umesh(&umesh);
  AH5_free_msh_instance(&mesh);
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Write an unstructured nodes mesh
char *test_write_unstructured_nodes_mesh()
{
  AH5_umesh_t umesh;
  hid_t file_id, loc_id;
  hsize_t i;

  // Build the nesh
  // init nodes
  umesh.nb_nodes[0] = 5;
  umesh.nb_nodes[1] = 3;
  umesh.nodes = (float *)malloc(umesh.nb_nodes[0]*umesh.nb_nodes[1]*sizeof(float));
  for (i = 0; i < umesh.nb_nodes[0]*umesh.nb_nodes[1]; ++i)
    umesh.nodes[i] = i;
  // init element
  umesh.nb_elementtypes = 0;
  umesh.elementtypes = NULL;
  umesh.nb_elementnodes = 0;
  umesh.elementnodes = NULL;
  // build group
  umesh.nb_groups = 1;
  umesh.groups = (AH5_ugroup_t *)malloc(umesh.nb_groups * sizeof(AH5_ugroup_t));
  umesh.groups[0].path = (char *)malloc(5 * sizeof(char));
  strcpy(umesh.groups[0].path, "name");
  umesh.groups[0].entitytype = AH5_GROUP_NODE;
  umesh.groups[0].nb_groupelts = 5;
  umesh.groups[0].groupelts = (int *)malloc(umesh.groups[0].nb_groupelts * sizeof(int));
  for (i = 0; i < umesh.groups[0].nb_groupelts; ++i)
    umesh.groups[0].groupelts[i] = i;
  // group of group
  umesh.nb_groupgroups = 0;
  umesh.groupgroups = NULL;
  // selector on mesh
  umesh.nb_som_tables = 0;
  umesh.som_tables = NULL;

  // Test on simple unstructured mesh
  file_id = AH5_auto_test_file();

  // Write the mesh into '/mesh' node.
  loc_id = H5Gcreate(file_id, "/mesh", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 2);
  mu_assert("Write an simple umesh.", AH5_write_umesh(loc_id, &umesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 2);
  H5Gclose(loc_id);

  // Check if all the mesh parts are written.
  mu_assert("Check 'nodes' nodes in file.", AH5_path_valid(file_id, "/mesh/nodes"));
  mu_assert("Check 'elementNodes' nodes in file.", AH5_path_valid(file_id, "/mesh/elementNodes"));
  mu_assert("Check 'elementTypes' nodes in file.", AH5_path_valid(file_id, "/mesh/elementTypes"));
  mu_assert("Check 'groupGroup' nodes in file.", AH5_path_valid(file_id, "/mesh/groupGroup"));
  mu_assert("Check 'group' nodes in file.", AH5_path_valid(file_id, "/mesh/group"));
  mu_assert("Check 'group/name' nodes in file.", AH5_path_valid(file_id, "/mesh/group/name"));

  // Close and release
  AH5_free_umesh(&umesh);
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Check the mesh write function.
char *test_write_mesh()
{
  AH5_mesh_t mesh;
  hid_t file_id;
  // Add mesh group.
  mesh.nb_groups = 1;
  mesh.groups = (AH5_msh_group_t *) malloc(mesh.nb_groups * sizeof(AH5_msh_group_t));
  mesh.groups[0].path = new_string("mesh_group_name");
  mesh.groups[0].nb_mlk_instances = 0;
  mesh.groups[0].nb_msh_instances = 1;
  // Add unstructured mesh instance.
  mesh.groups[0].msh_instances = (AH5_msh_instance_t *) malloc(mesh.groups[0].nb_msh_instances *
                                 sizeof(AH5_msh_instance_t));
  mesh.groups[0].msh_instances[0].path = new_string("mesh_name");
  mesh.groups[0].msh_instances[0].type = MSH_UNSTRUCTURED;
  // Filling the mesh data.
  build_umesh_1(&(mesh.groups[0].msh_instances[0].data.unstructured));

  // Write mesh category.
  file_id = AH5_auto_test_file();
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);
  mu_assert("Write mesh [1].", AH5_write_mesh(file_id, &mesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);

  // Release resource.
  free(mesh.groups[0].path);
  free(mesh.groups[0].msh_instances[0].path);
  free(mesh.groups);

  // Read data (build data with full path) and rename a node to build a copy.
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);
  mu_assert("Read mesh [1].", AH5_read_mesh(file_id, &mesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);

  free(mesh.groups[0].path);
  mesh.groups[0].path = new_string("mesh_group_name_copy");
  mu_assert("Write mesh [2].", AH5_write_mesh(file_id, &mesh));

  // Clean and release resource.
  AH5_free_mesh(&mesh);
  AH5_close_test_file(file_id);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Test read unstructured mesh function.
char *test_read_umesh()
{
  AH5_msh_instance_t mesh;
  AH5_umesh_t umesh;
  AH5_usom_table_t *som;
  hid_t file_id, loc_id;

  // Write a simple mesh test.
  file_id = AH5_auto_test_file();
  build_umesh_1(&umesh);
  loc_id = H5Gcreate(file_id, "/mesh", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  AH5_write_umesh(loc_id, &umesh);
  H5Gclose(loc_id);
  AH5_free_umesh(&umesh);

  // Now read the mesh
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);
  mu_assert("The written mesh is readable.", AH5_read_msh_instance(file_id, "/mesh", &mesh));
  mu_assert_eq("check HDF object closed", H5Fget_obj_count(file_id, H5F_OBJ_ALL), 1);

  // Check some value
  mu_assert("Check mesh type", mesh.type == MSH_UNSTRUCTURED);
  mu_assert("Check nodes size", mesh.data.unstructured.nb_nodes[0] == 5);
  mu_assert("Check nodes size", mesh.data.unstructured.nb_nodes[1] == 3);

  // Check node path values.
  mu_assert_str_equal("Check the mesh path.", mesh.path, "/mesh");

  mu_assert_str_equal(
      "Check group name.", mesh.data.unstructured.groups[0].path, "/mesh/group/name");

  // Check som
  mu_assert("Check nb som tables", mesh.data.unstructured.nb_som_tables == 3);
  som = mesh.data.unstructured.som_tables;
  mu_assert_str_equal(
      "Check umsh som tables 0 name.", som->path, "/mesh/selectorOnMesh/edge");
  mu_assert("Check umsh som tables 0 type.", som->type == SOM_EDGE);
  mu_assert("Check umsh som tables 0", som->data.ef.dims[0] == 3);
  mu_assert("Check umsh som tables 0", som->data.ef.dims[1] == 2);
  mu_assert("Check smsh som tables 0", som->data.ef.items[0] == 10);
  mu_assert("Check smsh som tables 0", som->data.ef.items[1] == 11);
  mu_assert("Check smsh som tables 0", som->data.ef.items[2] == 12);
  mu_assert("Check smsh som tables 0", som->data.ef.items[3] == 13);
  mu_assert("Check smsh som tables 0", som->data.ef.items[4] == 14);
  mu_assert("Check smsh som tables 0", som->data.ef.items[5] == 15);

  som = mesh.data.unstructured.som_tables + 1;
  mu_assert_str_equal(
      "Check umsh som tables 1 name.", som->path, "/mesh/selectorOnMesh/face");
  mu_assert("Check umsh som tables 1 type.", som->type == SOM_FACE);
  mu_assert("Check umsh som tables 1", som->data.ef.dims[0] == 4);
  mu_assert("Check umsh som tables 1", som->data.ef.dims[1] == 2);
  mu_assert("Check smsh som tables 1", som->data.ef.items[0] == 100);
  mu_assert("Check smsh som tables 1", som->data.ef.items[1] == 101);
  mu_assert("Check smsh som tables 1", som->data.ef.items[2] == 102);
  mu_assert("Check smsh som tables 1", som->data.ef.items[3] == 103);
  mu_assert("Check smsh som tables 1", som->data.ef.items[4] == 104);
  mu_assert("Check smsh som tables 1", som->data.ef.items[5] == 105);
  mu_assert("Check smsh som tables 1", som->data.ef.items[6] == 106);
  mu_assert("Check smsh som tables 1", som->data.ef.items[7] == 107);

  som = mesh.data.unstructured.som_tables + 2;
  mu_assert_str_equal(
      "Check umsh som tables 2 name.", som->path, "/mesh/selectorOnMesh/pie");
  mu_assert("Check umsh som tables 2 type.", som->type == SOM_POINT_IN_ELEMENT);
  mu_assert("Check umsh som tables 2", som->data.pie.nb_dims == 3);
  mu_assert("Check umsh som tables 2", som->data.pie.nb_points == 2);
  mu_assert("Check smsh som tables 2", som->data.pie.indices[0] == 0);
  mu_assert("Check smsh som tables 2", som->data.pie.indices[1] == 1);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[0][0] == 2);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[0][1] == 3);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[0][2] == 4);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[1][0] == 5);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[1][1] == 6);
  mu_assert("Check smsh som tables 2", som->data.pie.vectors[1][2] == 7);

  // Close and clean
  AH5_close_test_file(file_id);
  AH5_free_msh_instance(&mesh);

  return MU_FINISHED_WITHOUT_ERRORS;
}


//! Test
char *test_element_size()
{
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_INVALID), 0);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_BAR2), 2);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_BAR3), 3);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_TRI3), 3);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_TRI6), 6);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_QUAD4), 4);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_QUAD8), 8);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_QUAD9), 9);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_TETRA4), 4);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_PYRA5), 5);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_PENTA6), 6);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_HEXA8), 8);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_TETRA10), 10);
  mu_assert_eq("Check element_size", AH5_element_size(AH5_UELE_HEXA20), 20);

  return MU_FINISHED_WITHOUT_ERRORS;
}


// Test write structured mesh
char* test_write_smesh() {
  AH5_mesh_t mesh;
  AH5_msh_group_t* msh_group = NULL;
  AH5_msh_instance_t* msh_instance = NULL;
  AH5_smesh_t* smesh = NULL;
  AH5_sgroup_t* sgroup = NULL;
  AH5_groupgroup_t* groupgroup = NULL;
  AH5_ssom_pie_table_t *som = NULL;
  int i = 0;
  hid_t file_id;

  AH5_init_mesh(&mesh, 1);
  msh_group = mesh.groups;
  AH5_init_msh_group(msh_group, "msh_group", 1, 0);
  msh_instance = msh_group->msh_instances;
  AH5_init_msh_instance(msh_instance, "msh_instance", MSH_STRUCTURED);
  smesh = &msh_instance->data.structured;
  AH5_init_smesh(smesh, 2, 1, 2);
  AH5_init_axis(&smesh->x, 2);
  smesh->x.nodes[0] = 0;
  smesh->x.nodes[1] = 1;
  AH5_init_axis(&smesh->y, 2);
  smesh->y.nodes[0] = 2;
  smesh->y.nodes[1] = 3;
  AH5_init_axis(&smesh->z, 2);
  smesh->z.nodes[0] = 4;
  smesh->z.nodes[1] = 5;
  sgroup = smesh->groups + 0;
  AH5_init_sgroup(sgroup, "surface", 6, AH5_GROUP_FACE);

  i = 0;
  sgroup->elements[i++] = 0;  // x-
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;

  sgroup->elements[i++] = 0;  // y-
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;

  sgroup->elements[i++] = 0;  // z-
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 0;

  sgroup->elements[i++] = 1;  // x+
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;

  sgroup->elements[i++] = 0;  // y+
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;

  sgroup->elements[i++] = 0;  // z+
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;

  i = 0;
  strcpy(sgroup->normals[i++], "x-");
  strcpy(sgroup->normals[i++], "y-");
  strcpy(sgroup->normals[i++], "z-");
  strcpy(sgroup->normals[i++], "x+");
  strcpy(sgroup->normals[i++], "y+");
  strcpy(sgroup->normals[i++], "z+");

  sgroup = smesh->groups + 1;
  AH5_init_sgroup(sgroup, "volume", 1, AH5_GROUP_VOLUME);

  i = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 0;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;
  sgroup->elements[i++] = 1;

  groupgroup = smesh->groupgroups;
  AH5_init_groupgroup(groupgroup, "all", 2, 7);
  strcpy(groupgroup->groupgroupnames[0], "surface");
  strcpy(groupgroup->groupgroupnames[1], "volume");

  som = smesh->som_tables;
  AH5_init_ssom_pie_table(som, "som1", 2);
  som->elements[0][0] = 0;
  som->elements[0][1] = 1;
  som->elements[0][2] = 2;
  som->elements[0][3] = 3;
  som->elements[0][4] = 4;
  som->elements[0][5] = 5;
  som->elements[1][0] = 6;
  som->elements[1][1] = 7;
  som->elements[1][2] = 8;
  som->elements[1][3] = 9;
  som->elements[1][4] = 10;
  som->elements[1][5] = 11;
  som->vectors[0][0] = 12;
  som->vectors[0][1] = 13;
  som->vectors[0][2] = 14;
  som->vectors[1][0] = 15;
  som->vectors[1][1] = 16;
  som->vectors[1][2] = 17;

  som = smesh->som_tables + 1;
  AH5_init_ssom_pie_table(som, "som2", 3);
  som->elements[0][0] = 100;
  som->elements[0][1] = 101;
  som->elements[0][2] = 102;
  som->elements[0][3] = 103;
  som->elements[0][4] = 104;
  som->elements[0][5] = 105;
  som->elements[1][0] = 106;
  som->elements[1][1] = 107;
  som->elements[1][2] = 108;
  som->elements[1][3] = 109;
  som->elements[1][4] = 110;
  som->elements[1][5] = 111;
  som->elements[2][0] = 112;
  som->elements[2][1] = 113;
  som->elements[2][2] = 114;
  som->elements[2][3] = 115;
  som->elements[2][4] = 116;
  som->elements[2][5] = 117;
  som->vectors[0][0] = 118;
  som->vectors[0][1] = 119;
  som->vectors[0][2] = 120;
  som->vectors[1][0] = 121;
  som->vectors[1][1] = 122;
  som->vectors[1][2] = 123;
  som->vectors[2][0] = 124;
  som->vectors[2][1] = 125;
  som->vectors[2][2] = 126;


  file_id = AH5_auto_test_file();
  mu_assert("Check write smesh", AH5_write_mesh(file_id, &mesh));

  AH5_free_mesh(&mesh);

  // Read mesh to check the conservation of all data
  AH5_read_mesh(file_id, &mesh);

  mu_assert("Check nb msh groups", mesh.nb_groups == 1);
  mu_assert("Check msh groups", mesh.groups != NULL);
  msh_group = mesh.groups;
  mu_assert_str_equal(
      "Check msh group name", msh_group->path, "/mesh/msh_group");
  mu_assert("Check nb msh instance", msh_group->nb_msh_instances == 1);
  mu_assert("Check msh instance", msh_group->msh_instances != NULL);
  msh_instance = msh_group->msh_instances;
  mu_assert_str_equal(
      "Check msh instance name", msh_instance->path, "/mesh/msh_group/msh_instance");
  mu_assert("Check msh instance type", msh_instance->type == MSH_STRUCTURED);
  smesh = &msh_instance->data.structured;
  mu_assert("Check smsh x nb nodes", smesh->x.nb_nodes == 2);
  mu_assert("Check smsh x nodes", smesh->x.nodes != NULL);
  mu_assert("Check smsh x node", smesh->x.nodes[0] == 0);
  mu_assert("Check smsh x node", smesh->x.nodes[1] == 1);
  mu_assert("Check smsh y nb nodes", smesh->y.nb_nodes == 2);
  mu_assert("Check smsh y nodes", smesh->y.nodes != NULL);
  mu_assert("Check smsh y node", smesh->y.nodes[0] == 2);
  mu_assert("Check smsh y node", smesh->y.nodes[1] == 3);
  mu_assert("Check smsh z nb nodes", smesh->z.nb_nodes == 2);
  mu_assert("Check smsh z nodes", smesh->z.nodes != NULL);
  mu_assert("Check smsh z node", smesh->z.nodes[0] == 4);
  mu_assert("Check smsh z node", smesh->z.nodes[1] == 5);
  mu_assert("Check smsh nb groups", smesh->nb_groups == 2);
  mu_assert("Check smsh groups", smesh->groups != NULL);
  sgroup = smesh->groups + 0;
  mu_assert_str_equal(
      "Check smsh group 0", sgroup->path, "/mesh/msh_group/msh_instance/group/surface");
  mu_assert("Check smsh group 0", sgroup->entitytype == AH5_GROUP_FACE);
  mu_assert("Check smsh group 0", sgroup->dims[0] == 6);
  mu_assert("Check smsh group 0", sgroup->dims[1] == 6);
  mu_assert("Check smsh group 0", sgroup->elements != NULL);
  i = 0;
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 0", sgroup->elements[i++] == 1);
  i = 0;
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "x-");
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "y-");
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "z-");
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "x+");
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "y+");
  mu_assert_str_equal("Check smsh group 0", sgroup->normals[i++], "z+");
  sgroup = smesh->groups + 1;
  mu_assert_str_equal(
      "Check smsh group 1", sgroup->path, "/mesh/msh_group/msh_instance/group/volume");
  mu_assert("Check smsh group 1", sgroup->entitytype == AH5_GROUP_VOLUME);
  mu_assert("Check smsh group 1", sgroup->dims[0] == 1);
  mu_assert("Check smsh group 1", sgroup->dims[1] == 6);
  mu_assert("Check smsh group 1", sgroup->elements != NULL);
  i = 0;
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 0);
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 1", sgroup->elements[i++] == 1);
  mu_assert("Check smsh group 1", sgroup->normals == NULL);
  mu_assert("Check smsh nb groupgroups", smesh->nb_groupgroups == 1);
  mu_assert("Check smsh groupgroups", smesh->groupgroups != NULL);
  groupgroup = smesh->groupgroups + 0;
  mu_assert_str_equal(
      "Check smsh groupgroup 0", groupgroup->path,
      "/mesh/msh_group/msh_instance/groupGroup/all");
  mu_assert("Check smsh groupgroup 0 nb groups", groupgroup->nb_groupgroupnames == 2);
  mu_assert_str_equal(
      "Check smsh groupgroup 0 groups", groupgroup->groupgroupnames[0], "surface");
  mu_assert_str_equal(
      "Check smsh groupgroup 0 groups", groupgroup->groupgroupnames[1], "volume");

  mu_assert("Check smsh nb som tables", smesh->nb_som_tables == 2);
  som = smesh->som_tables;
  mu_assert("Check smsh som tables 0", som->nb_dims == 3);
  mu_assert("Check smsh som tables 0", som->nb_points == 2);
  mu_assert("Check smsh som tables 0", som->elements[0][0] == 0);
  mu_assert("Check smsh som tables 0", som->elements[0][1] == 1);
  mu_assert("Check smsh som tables 0", som->elements[0][2] == 2);
  mu_assert("Check smsh som tables 0", som->elements[0][3] == 3);
  mu_assert("Check smsh som tables 0", som->elements[0][4] == 4);
  mu_assert("Check smsh som tables 0", som->elements[0][5] == 5);
  mu_assert("Check smsh som tables 0", som->elements[1][0] == 6);
  mu_assert("Check smsh som tables 0", som->elements[1][1] == 7);
  mu_assert("Check smsh som tables 0", som->elements[1][2] == 8);
  mu_assert("Check smsh som tables 0", som->elements[1][3] == 9);
  mu_assert("Check smsh som tables 0", som->elements[1][4] == 10);
  mu_assert("Check smsh som tables 0", som->elements[1][5] == 11);
  mu_assert("Check smsh som tables 0", som->vectors[0][0] == 12);
  mu_assert("Check smsh som tables 0", som->vectors[0][1] == 13);
  mu_assert("Check smsh som tables 0", som->vectors[0][2] == 14);
  mu_assert("Check smsh som tables 0", som->vectors[1][0] == 15);
  mu_assert("Check smsh som tables 0", som->vectors[1][1] == 16);
  mu_assert("Check smsh som tables 0", som->vectors[1][2] == 17);
  som = smesh->som_tables + 1;
  mu_assert("Check smsh som tables 1", som->nb_dims == 3);
  mu_assert("Check smsh som tables 1", som->nb_points == 3);
  mu_assert("Check smsh som tables 1", som->elements[0][0] == 100);
  mu_assert("Check smsh som tables 1", som->elements[0][1] == 101);
  mu_assert("Check smsh som tables 1", som->elements[0][2] == 102);
  mu_assert("Check smsh som tables 1", som->elements[0][3] == 103);
  mu_assert("Check smsh som tables 1", som->elements[0][4] == 104);
  mu_assert("Check smsh som tables 1", som->elements[0][5] == 105);
  mu_assert("Check smsh som tables 1", som->elements[1][0] == 106);
  mu_assert("Check smsh som tables 1", som->elements[1][1] == 107);
  mu_assert("Check smsh som tables 1", som->elements[1][2] == 108);
  mu_assert("Check smsh som tables 1", som->elements[1][3] == 109);
  mu_assert("Check smsh som tables 1", som->elements[1][4] == 110);
  mu_assert("Check smsh som tables 1", som->elements[1][5] == 111);
  mu_assert("Check smsh som tables 1", som->elements[2][0] == 112);
  mu_assert("Check smsh som tables 1", som->elements[2][1] == 113);
  mu_assert("Check smsh som tables 1", som->elements[2][2] == 114);
  mu_assert("Check smsh som tables 1", som->elements[2][3] == 115);
  mu_assert("Check smsh som tables 1", som->elements[2][4] == 116);
  mu_assert("Check smsh som tables 1", som->elements[2][5] == 117);
  mu_assert("Check smsh som tables 1", som->vectors[0][0] == 118);
  mu_assert("Check smsh som tables 1", som->vectors[0][1] == 119);
  mu_assert("Check smsh som tables 1", som->vectors[0][2] == 120);
  mu_assert("Check smsh som tables 1", som->vectors[1][0] == 121);
  mu_assert("Check smsh som tables 1", som->vectors[1][1] == 122);
  mu_assert("Check smsh som tables 1", som->vectors[1][2] == 123);
  mu_assert("Check smsh som tables 1", som->vectors[2][0] == 124);
  mu_assert("Check smsh som tables 1", som->vectors[2][1] == 125);
  mu_assert("Check smsh som tables 1", som->vectors[2][2] == 126);

  AH5_free_mesh(&mesh);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_umsh_made_of_nodes()
{
  AH5_umesh_t mesh;
  AH5_init_umesh(&mesh, 0, 0, 1, 0, 0, 0);
  mu_assert_eq("check umesh nb element nodes", mesh.nb_elementnodes, 0);
  mu_assert_eq_ptr("check umesh element nodes", mesh.elementnodes, NULL);
  mu_assert_eq("check umesh nb element types", mesh.nb_elementtypes, 0);
  mu_assert_eq_ptr("check umesh element types", mesh.elementtypes, NULL);
  mu_assert_eq("check umesh nb nodes", mesh.nb_nodes[0], 1);
  mu_assert("check umesh nodes", mesh.nodes != NULL);

  AH5_free_umesh(&mesh);
  mu_assert_eq("check umesh nb element types", mesh.nb_nodes[0], 0);
  mu_assert_eq_ptr("check umesh element types", mesh.nodes, NULL);

  return MU_FINISHED_WITHOUT_ERRORS;
}


char *test_misformed_umesh()
{
  hid_t ahdf;

  mu_assert_eq("misformed mesh creation", build_misformed_umesh("test.h5"), AH5_TRUE);

  ahdf = H5Fopen("test.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
  mu_assert("open", ahdf > 0);
  AH5_msh_instance_t msh;
  mu_assert_eq("read", AH5_read_msh_instance(ahdf, "/mesh/mesh/mesh", &msh), AH5_TRUE);
  H5Fclose(ahdf);
  mu_assert_eq("type", msh.type, MSH_UNSTRUCTURED);
  mu_assert_eq("nb etypes", msh.data.unstructured.nb_elementtypes, 1);
  mu_assert_eq("etypes", msh.data.unstructured.elementtypes[0], 1);
  mu_assert_eq("nb enodes", msh.data.unstructured.nb_elementnodes, 2);
  mu_assert_eq("enodes", msh.data.unstructured.elementnodes[0], 0);
  mu_assert_eq("enodes", msh.data.unstructured.elementnodes[1], 1);

  return MU_FINISHED_WITHOUT_ERRORS;
}


// Run all tests
char *all_tests()
{
  mu_run_test(test_copy_umesh);
  mu_run_test(test_write_groupgroup);
  mu_run_test(test_init_functions);
  mu_run_test(test_write_unstructured_mesh_group);
  mu_run_test(test_write_umesh);
  mu_run_test(test_write_unstructured_nodes_mesh);
  mu_run_test(test_read_umesh);
  mu_run_test(test_write_mesh);
  mu_run_test(test_element_size);
  mu_run_test(test_write_smesh);
  mu_run_test(test_umsh_made_of_nodes);
  mu_run_test(test_misformed_umesh);

  return MU_FINISHED_WITHOUT_ERRORS;
}

// Main function, run tests and print results.
int main(int UNUSED(argc), char **UNUSED(argv))
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
