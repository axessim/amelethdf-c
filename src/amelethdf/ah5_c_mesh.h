#ifndef AH5_C_MESH_H
#define AH5_C_MESH_H

#include "ah5_general.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _AH5_groupgroup_t
{
  char            *path;
  hsize_t         nb_groupgroupnames;
  char            **groupgroupnames;
} AH5_groupgroup_t;

typedef struct _AH5_ssom_pie_table_t
{
  char            *path;
  hsize_t         nb_dims;
  hsize_t         nb_points;
  unsigned int    **elements;
  float           **vectors;
} AH5_ssom_pie_table_t;

typedef struct _AH5_axis_t
{
  hsize_t         nb_nodes;
  float           *nodes;
} AH5_axis_t;

typedef enum _AH5_group_entitytype_t
{
  AH5_GROUP_ENTITYTYPE_INVALID = -1,
  AH5_GROUP_ENTITYTYPE_UNDEF = 0,
  AH5_GROUP_NODE = 1,
  AH5_GROUP_EDGE = 2,
  AH5_GROUP_FACE = 3,
  AH5_GROUP_VOLUME = 4
} AH5_group_entitytype_t;

typedef struct _AH5_sgroup_t
{
  char                   *path;
  AH5_group_entitytype_t entitytype;
  hsize_t                dims[2];
  int                    *elements;    // TODO: implement with 2d-array and first element is flatten list.
  char                   **normals;    // TODO: The first element is flatten normals list.
  char                   *flat_normals;
} AH5_sgroup_t;

typedef struct _AH5_smesh_t
{
  AH5_axis_t      x;
  AH5_axis_t      y;
  AH5_axis_t      z;
  hsize_t         nb_groups;
  AH5_sgroup_t    *groups;
  hsize_t         nb_groupgroups;
  AH5_groupgroup_t *groupgroups;
  hsize_t         nb_som_tables;
  AH5_ssom_pie_table_t *som_tables;
} AH5_smesh_t;

typedef struct _AH5_ugroup_t
{
  char                   *path;
  AH5_group_entitytype_t entitytype;
  hsize_t                nb_groupelts;
  int                    *groupelts;
} AH5_ugroup_t;

typedef enum _AH5_usom_class_t
{
  SOM_INVALID             = -1,
  SOM_POINT_IN_ELEMENT    = 1,
  SOM_EDGE                = 2,
  SOM_FACE                = 3
} AH5_usom_class_t;

typedef struct _AH5_usom_pie_table_t
{
  hsize_t         nb_dims;
  hsize_t         nb_points;
  int             *indices;
  float           **vectors;
} AH5_usom_pie_table_t;

typedef struct _AH5_usom_ef_table_t
{
  hsize_t         dims[2];
  int             *items;
} AH5_usom_ef_table_t;

typedef union _AH5_usomx_t
{
  AH5_usom_pie_table_t pie;
  AH5_usom_ef_table_t ef;
} AH5_usomx_t;

typedef struct _AH5_usom_table_t
{
  char            *path;
  AH5_usom_class_t type;
  AH5_usomx_t     data;
} AH5_usom_table_t;

typedef enum _AH5_uelement_type_t
{
  AH5_UELE_INVALID            = -1,
  AH5_UELE_NODE               = 0,  // Not native AH5 element.
  AH5_UELE_BAR2               = 1,
  AH5_UELE_BAR3               = 2,
  AH5_UELE_TRI3               = 11,
  AH5_UELE_TRI6               = 12,
  AH5_UELE_QUAD4              = 13,
  AH5_UELE_QUAD8              = 14,
  AH5_UELE_QUAD9              = 18,
  AH5_UELE_TETRA4             = 101,
  AH5_UELE_PYRA5              = 102,
  AH5_UELE_PENTA6             = 103,
  AH5_UELE_HEXA8              = 104,
  AH5_UELE_TETRA10            = 108,
  AH5_UELE_HEXA20             = 109
} AH5_uelement_type_t;

/**
 * @def AH5_UMESH_DIM
 * Read the mesh dimension from umesh.nb_nodes
 * @def AH5_UMESH_NODES_SIZE
 * Read the number of nodes from umesh.nb_nodes
 */
#define AH5_UMESH_DIM 1
#define AH5_UMESH_NODES_SIZE 0

typedef struct _AH5_umesh_t
{
  hsize_t         nb_elementnodes;
  int             *elementnodes;
  hsize_t         nb_elementtypes;
  char            *elementtypes;
  hsize_t         nb_nodes[2];
  float           *nodes;
  hsize_t         nb_groups;
  AH5_ugroup_t    *groups;
  hsize_t         nb_groupgroups;
  AH5_groupgroup_t *groupgroups;
  hsize_t         nb_som_tables;
  AH5_usom_table_t *som_tables;
} AH5_umesh_t;

typedef enum _AH5_mesh_class_t
{
  MSH_INVALID             = -1,
  MSH_STRUCTURED          = 1,
  MSH_UNSTRUCTURED        = 2
} AH5_mesh_class_t;

typedef union _AH5_meshx_t
{
  AH5_smesh_t     structured;
  AH5_umesh_t     unstructured;
} AH5_meshx_t;

typedef struct _AH5_msh_instance_t
{
  char            *path;
  AH5_mesh_class_t type;
  AH5_meshx_t     data;
} AH5_msh_instance_t;

typedef enum _AH5_meshlink_class_t
{
  MSHLNK_INVALID          = -1,
  MSHLNK_NODE             = 1,
  MSHLNK_EDGE             = 2,
  MSHLNK_FACE             = 3,
  MSHLNK_VOLUME           = 4
} AH5_meshlink_class_t;

typedef struct _AH5_mlk_instance_t
{
  char            *path;
  AH5_meshlink_class_t type;
  char            *mesh1;
  char            *mesh2;
  hsize_t         dims[2];
  int             *data;
} AH5_mlk_instance_t;

typedef struct _AH5_msh_group_t
{
  char            *path;
  hsize_t         nb_msh_instances;
  AH5_msh_instance_t *msh_instances;
  hsize_t         nb_mlk_instances;
  AH5_mlk_instance_t *mlk_instances;
} AH5_msh_group_t;

typedef struct _AH5_mesh_t
{
  hsize_t         nb_groups;
  AH5_msh_group_t *groups;
} AH5_mesh_t;




AH5_PUBLIC AH5_groupgroup_t *AH5_init_groupgroup(AH5_groupgroup_t *groupgroup, const char *path,
    hsize_t nb, size_t length);
AH5_PUBLIC AH5_axis_t *AH5_init_axis(AH5_axis_t *axis, hsize_t nb_nodes);
  AH5_PUBLIC AH5_ssom_pie_table_t *AH5_init_smsh_som(AH5_ssom_pie_table_t *som, const char *path, hsize_t size);
AH5_PUBLIC AH5_sgroup_t *AH5_init_smsh_group(  // deprecated in favor of AH5_init_sgroup
    AH5_sgroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype);
AH5_PUBLIC AH5_sgroup_t *AH5_init_sgroup(
    AH5_sgroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype);
AH5_PUBLIC AH5_ugroup_t *AH5_init_umsh_group(AH5_ugroup_t *group, const char *path, hsize_t nb_eles,
    AH5_group_entitytype_t entitytype);

AH5_PUBLIC AH5_usom_table_t *AH5_init_umsh_som(AH5_usom_table_t *som, const char *path, hsize_t size, AH5_usom_class_t type);

AH5_PUBLIC AH5_smesh_t *AH5_init_smesh(AH5_smesh_t *smesh, hsize_t nb_groups,
                                       hsize_t nb_groupgroups, hsize_t nb_som_tables);
AH5_PUBLIC AH5_umesh_t *AH5_init_umesh(AH5_umesh_t *umesh, hsize_t nb_elementnodes,
                                       hsize_t nb_elementtypes, hsize_t nb_nodes, hsize_t nb_groups, hsize_t nb_groupgroups,
                                       hsize_t nb_som_tables);
AH5_PUBLIC AH5_msh_instance_t *AH5_init_msh_instance(AH5_msh_instance_t *msh_instance,
    const char *path, AH5_mesh_class_t type);
AH5_PUBLIC AH5_mlk_instance_t *AH5_init_mlk_instance(AH5_mlk_instance_t *mlk_instance,
    const char *path, AH5_meshlink_class_t type);
AH5_PUBLIC AH5_msh_group_t *AH5_init_msh_group(AH5_msh_group_t *msh_group, const char *path,
    hsize_t nb_meshs, hsize_t nb_mesh_links);
AH5_PUBLIC AH5_mesh_t *AH5_init_mesh(AH5_mesh_t *mesh, hsize_t nb_groups);

AH5_PUBLIC char AH5_read_groupgroup(hid_t file_id, const char *path, AH5_groupgroup_t *groupgroup);
AH5_PUBLIC char AH5_read_smsh_axis(hid_t file_id, const char *path, AH5_axis_t *axis);
AH5_PUBLIC char AH5_read_smsh_group(hid_t file_id, const char *path, AH5_sgroup_t *sgroup);
AH5_PUBLIC char AH5_read_ssom_pie_table(hid_t file_id, const char *path,
                                        AH5_ssom_pie_table_t *ssom_pie_table);
AH5_PUBLIC char AH5_read_smesh(hid_t file_id, const char *path, AH5_smesh_t *smesh);
AH5_PUBLIC char AH5_read_umsh_group(hid_t file_id, const char *path, AH5_ugroup_t *ugroup);
AH5_PUBLIC char AH5_read_usom_pie_table(hid_t file_id, const char *path,
                                        AH5_usom_pie_table_t *usom_pie_table);
AH5_PUBLIC char AH5_read_usom_ef_table(hid_t file_id, const char *path,
                                       AH5_usom_ef_table_t *usom_ef_table);
AH5_PUBLIC char AH5_read_umesh_som_table(hid_t file_id, const char *path,
    AH5_usom_table_t *usom_table);
AH5_PUBLIC char AH5_read_umesh(hid_t file_id, const char *path, AH5_umesh_t *umesh);
AH5_PUBLIC char AH5_read_msh_instance(hid_t file_id, const char *path,
                                      AH5_msh_instance_t *msh_instance);
AH5_PUBLIC char AH5_read_mlk_instance(hid_t file_id, const char *path,
                                      AH5_mlk_instance_t *mlk_instance);
AH5_PUBLIC char AH5_read_msh_group(hid_t file_id, const char *path, AH5_msh_group_t *msh_group);
AH5_PUBLIC char AH5_read_mesh(hid_t file_id, AH5_mesh_t *mesh);

// General notes about write functions.
//
// The data can be relatively named (data.path = the node name not the full path).
// In this case the 'file_id' must be the direct parent node.
//
// The 'file_id' can be the root node but in this case the first data node's path must be the full node path.
//
AH5_PUBLIC char AH5_write_groupgroup(hid_t file_id, const AH5_groupgroup_t *groupgroup, hsize_t nb);
AH5_PUBLIC char AH5_write_smesh_axis(hid_t file_id, const AH5_axis_t *axis);
AH5_PUBLIC char AH5_write_smsh_group(hid_t file_id, const AH5_sgroup_t *sgroup);
AH5_PUBLIC char AH5_write_ssom_pie_table(hid_t file_id, const AH5_ssom_pie_table_t *ssom_pie_table);
AH5_PUBLIC char AH5_write_smesh(hid_t file_id, const AH5_smesh_t *smesh);
AH5_PUBLIC char AH5_write_umsh_group(hid_t file_id, const AH5_ugroup_t *ugroup, hsize_t nb_grp);
AH5_PUBLIC char AH5_write_usom_pie_table(hid_t file_id, const AH5_usom_pie_table_t *usom_pie_table);
AH5_PUBLIC char AH5_write_usom_ef_table(hid_t file_id, const AH5_usom_ef_table_t *usom_ef_table);
AH5_PUBLIC char AH5_write_umesh_som_table(hid_t file_id, const AH5_usom_table_t *usom_table,
    hsize_t nb_som);
AH5_PUBLIC char AH5_write_umesh(hid_t file_id, const AH5_umesh_t *umesh);
AH5_PUBLIC char AH5_write_msh_instance(hid_t file_id, const AH5_msh_instance_t *msh_instance);
AH5_PUBLIC char AH5_write_mlk_instance(hid_t file_id, const AH5_mlk_instance_t *mlk_instance);
AH5_PUBLIC char AH5_write_msh_group(hid_t file_id, const AH5_msh_group_t *msh_group);
AH5_PUBLIC char AH5_write_mesh(hid_t file_id, const AH5_mesh_t *mesh);

AH5_PUBLIC void AH5_print_smesh(const AH5_smesh_t *smesh, int space);
AH5_PUBLIC void AH5_print_umesh_som_table(const AH5_usom_table_t *usom_table, int space);
AH5_PUBLIC void AH5_print_umesh(const AH5_umesh_t *umesh, int space);
AH5_PUBLIC void AH5_print_msh_instance(const AH5_msh_instance_t *msh_instance, int space);
AH5_PUBLIC void AH5_print_mlk_instance(const AH5_mlk_instance_t *mlk_instance, int space);
AH5_PUBLIC void AH5_print_msh_group(const AH5_msh_group_t *msh_group, int space);
AH5_PUBLIC void AH5_print_mesh(const AH5_mesh_t *mesh);

AH5_PUBLIC void AH5_free_groupgroup(AH5_groupgroup_t *groupgroup);
AH5_PUBLIC void AH5_free_sgroup(AH5_sgroup_t *sgroup);
AH5_PUBLIC void AH5_free_smesh(AH5_smesh_t *smesh);
AH5_PUBLIC void AH5_free_umesh(AH5_umesh_t *umesh);
AH5_PUBLIC void AH5_free_msh_instance(AH5_msh_instance_t *msh_instance);
AH5_PUBLIC void AH5_free_mlk_instance(AH5_mlk_instance_t *mlk_instance);
AH5_PUBLIC void AH5_free_msh_group(AH5_msh_group_t *msh_group);
AH5_PUBLIC void AH5_free_mesh(AH5_mesh_t *mesh);

// Copy AH5 mesh structure general prototype:
// AH5_XXX_t * AH5_copy_XXX(AH5_XXX_t *destination, const AH5_XXX_t *source);
// return destination on success.
AH5_PUBLIC AH5_umesh_t * AH5_copy_umesh(AH5_umesh_t *destination, const AH5_umesh_t *source);

// Define some useful tools for manege / used / make mesh;
AH5_PUBLIC int AH5_element_size(char element_type);

#ifdef __cplusplus
}
#endif

#endif // AH5_C_MESH_H
