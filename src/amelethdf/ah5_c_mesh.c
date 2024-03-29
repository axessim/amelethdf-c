/**
 * @file   ah5_c_mesh.c
 * @author Nathanaël MUOT <nathanael.muot@axessim.fr>
 * @date   Mon Nov 23 12:49:21 2015
 *
 * @brief  AH5 mesh interface.
 *
 *
 */

#include "ah5_general.h"
#include "ah5_internal.h"
#include "ah5_c_mesh.h"
#include "ah5_log.h"

#include <assert.h>


/**
 * Convert group entity type to ah5 C string type.
 *
 * @param[in] entitytype the group type.
 * @param[out] ctype (constant C string) the group type (node or element)
 * @param[out] centitytype (constant C string) the group element type (NULL edge face or volume)
 *
 * @return the entity type dimension
 */
int AH5_write_group_entitytype(
    AH5_group_entitytype_t entitytype, char **ctype, char **centitytype)
{
  int dimension = -1;

  *ctype = NULL;
  *centitytype = NULL;

  switch (entitytype)
  {
    case AH5_GROUP_NODE:
      *ctype = AH5_V_NODE;
      dimension = 0;
      break;

    case AH5_GROUP_EDGE:
      *ctype = AH5_V_ELEMENT;
      *centitytype = AH5_V_EDGE;
      dimension = 1;
      break;

    case AH5_GROUP_FACE:
      *ctype = AH5_V_ELEMENT;
      *centitytype = AH5_V_FACE;
      dimension = 2;
      break;

    case AH5_GROUP_VOLUME:
      *ctype = AH5_V_ELEMENT;
      *centitytype = AH5_V_VOLUME;
      dimension = 3;
      break;
    default:
      dimension = -1;
  }

  return dimension;
}


/**
 * Convert ah5 group type from C string type to group entity type.
 *
 * @param[in] ctype (constant C string) the group type (node or element)
 * @param[in] centitytype (constant C string) the group element type (NULL edge face or volume)
 * @param[out] entitytype the group type.
 *
 * @return
 */
int AH5_read_group_entitytype(
    const char *ctype, const char *centitytype, AH5_group_entitytype_t *entitytype)
{
  int dimension = -1;

  if (AH5_strcmp(ctype, AH5_V_NODE) == 0)
  {
    *entitytype = AH5_GROUP_NODE;
    dimension = 0;
  }
  else
  {
    if (AH5_strcmp(centitytype, AH5_V_EDGE) == 0)
    {
      *entitytype = AH5_GROUP_EDGE;
      dimension = 1;
    }
    else if (AH5_strcmp(centitytype, AH5_V_FACE) == 0)
    {
      *entitytype = AH5_GROUP_FACE;
      dimension = 2;
    }
    else if (AH5_strcmp(centitytype, AH5_V_VOLUME) == 0)
    {
      *entitytype = AH5_GROUP_VOLUME;
      dimension = 3;
    }
    else
    {
      *entitytype = AH5_GROUP_ENTITYTYPE_UNDEF;
    }
  }

  return dimension;
}


/**
 * Initialized and allocates mesh group.
 *
 * @param groupgroup the initialized group
 * @param path the mesh groupgroup name
 * @param nb the number of entry
 * @param length the entry size
 *
 * @return On success, a pointer to the mesh group. If the function failed to
 * allocate memory, a null pointer is returned.
 *
 * groupgroupnames[0] -> {0, ..., length, length+1, ... 2*length, 2*length+1, ... 3*length}
 * groupgroupnames[1] --------------------^
 * groupgroupnames[2] --------------------------------------------^
 *
 */
AH5_groupgroup_t *AH5_init_groupgroup(
  AH5_groupgroup_t *groupgroup, const char *path, hsize_t nb, size_t length)
{
  char success = AH5_TRUE;
  hsize_t i;

  if (groupgroup)
  {
    groupgroup->path = NULL;
    groupgroup->nb_groupgroupnames = nb;
    groupgroup->groupgroupnames = NULL;

    if (path)
    {
      AH5_setpath(&groupgroup->path, path);
    }

    if (nb)
    {
      groupgroup->groupgroupnames = (char**) malloc(sizeof(char*) * nb);
      success &= groupgroup->groupgroupnames != NULL;

      if (success)
      {
        ++length;  // null terminator
        *groupgroup->groupgroupnames = (char*) malloc(sizeof(char) * nb * length);
        success &= *groupgroup->groupgroupnames != NULL;
      }

      if (success)
      {
        for (i = 1; i < nb; ++i)
          groupgroup->groupgroupnames[i] = *groupgroup->groupgroupnames + i * length;
      }
    }
  }

  if (!success)
  {
    AH5_free_groupgroup(groupgroup);
    return NULL;
  }

  return groupgroup;
}


/**
 * Initialized and allocates mesh axis.
 *
 * @param axis the initialized axis
 * @param nb_nodes number of nodes
 *
 * @return On success, a pointer to the axis. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_axis_t *AH5_init_axis(AH5_axis_t *axis, hsize_t nb_nodes)
{
  if (axis)
  {
    axis->nb_nodes = nb_nodes;
    axis->nodes = NULL;

    if (nb_nodes)
    {
      axis->nodes = (float *)malloc(nb_nodes*sizeof(float));
      if (axis->nodes == NULL)
        return NULL;
    }
  }

  return axis;
}



AH5_ssom_pie_table_t *AH5_init_smsh_som(AH5_ssom_pie_table_t *som, const char *path, hsize_t size)
{
  return AH5_init_ssom_pie_table(som, path, size);
}
AH5_ssom_pie_table_t *AH5_init_ssom_pie_table(
    AH5_ssom_pie_table_t *som, const char *path, hsize_t nb_points) {
  hsize_t i, nb_dims = 3;

  if (som) {
    som->path = NULL;
    som->elements = NULL;
    som->vectors = NULL;

    if (path)
      AH5_setpath(&som->path, path);

    som->nb_dims = nb_dims;
    som->nb_points = nb_points;

    if (nb_points) {
      som->elements = (unsigned int **)malloc(nb_points * sizeof(unsigned int *));
      som->elements[0] = (unsigned int *)malloc(
          nb_points * nb_dims * 2 * sizeof(unsigned int));

      som->vectors = (float **)malloc(nb_points * sizeof(float *));
      som->vectors[0] = (float *)malloc(nb_points * nb_dims * sizeof(float));

      for (i = 1; i < nb_points; ++i) {
        som->elements[i] = som->elements[0] + i * 2 * nb_dims;
        som->vectors[i] = som->vectors[0] + i * nb_dims;
      }

      // Initialize with default value
      for (i = 0; i < nb_points * nb_dims * 2; ++i)
        som->elements[0][i] = 0;
      for (i = 0; i < nb_points * nb_dims; ++i)
        som->vectors[0][i] = -1;
    }
  }

  return som;
}


/**
 * Initialized and allocates mesh group.
 *
 * @param group the initialized group
 * @param path the mesh name
 * @param nb_eles number of element pointed by the group
 * @param type the group type (GROUP_NODE, GROUP_ELEMENT)
 * @param entitytype the group element type (GROUP_EDGE, GROUP_FACE,
 *   GROUP_VOLUME or GROUP_ENTITYTYPE_UNDEF)
 *
 * @deprecated AH5_init_sgroup
 *
 * @return On success, a pointer to the mesh group. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_sgroup_t *AH5_init_smsh_group(
  AH5_sgroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype)
{
  return AH5_init_sgroup(group, path, nb_eles, entitytype);
}

AH5_sgroup_t *AH5_init_sgroup(
  AH5_sgroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype)
{
  char success = AH5_TRUE;
  hsize_t i;

  if (entitytype == AH5_GROUP_ENTITYTYPE_INVALID ||
      entitytype == AH5_GROUP_ENTITYTYPE_UNDEF)
  {
    printf("***** ERROR: Fail to initialize group: no group provided.\n");
    return NULL;
  }

  if (group)
  {
    group->path = NULL;
    group->entitytype = entitytype;
    group->dims[0] = nb_eles;
    group->dims[1] = 0;
    group->elements = NULL;
    group->normals = NULL;
    group->flat_normals = NULL;

    if (path)
    {
      AH5_setpath(&group->path, path);
    }

    if (nb_eles)
    {
      if (entitytype == AH5_GROUP_NODE)
      {
        group->dims[1] = 3;
      }
      else
      {
        group->dims[1] = 6;
      }

      group->elements = (int*) malloc(sizeof(int) * group->dims[0] * group->dims[1]);
      success &= group->elements != NULL;
      if (!success) {
        printf("***** ERROR: Fail to initialize group: fail to allocate elements: %d x %d.\n",
               (int)group->dims[0], (int)group->dims[1]);
      }

      if (entitytype == AH5_GROUP_FACE)
      {
        if (success)
        {
          group->normals = (char**) malloc(sizeof(char*) * group->dims[0]);
          success &= group->normals != NULL;
        }

        if (success)
        {
          group->flat_normals = (char*) malloc(sizeof(char) * (group->dims[0] * 2 + 1));
          *group->normals = group->flat_normals;
          success &= *group->normals != NULL;
        }

        if (success)
        {
          (*group->normals)[group->dims[0] * 2] = '\0';
          for (i = 0; i < group->dims[0]; ++i)
            group->normals[i] = *group->normals + i * 2;
        }
      }
    } else {
      printf("***** WARNING: Fail to initialize group: no elements provided.\n");
    }
  }

  if (!success)
  {
    printf("***** ERROR: Fail to initialize group.\n");
    AH5_free_sgroup(group);
    return NULL;
  }

  return group;
}


/**
 * Initialized and allocates mesh group.
 *
 * @param group the initialized group
 * @param path the mesh name
 * @param nb_eles number of element pointed by the group
 * @param type the group type (GROUP_NODE, GROUP_ELEMENT)
 * @param entitytype the group element type (GROUP_EDGE, GROUP_FACE,
 *   GROUP_VOLUME or GROUP_ENTITYTYPE_UNDEF)
 *
 * @deprecated AH5_init_ugroup
 *
 * @return On success, a pointer to the mesh group. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_ugroup_t *AH5_init_umsh_group(
  AH5_ugroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype)
{
  return AH5_init_ugroup(group, path, nb_eles, entitytype);
}

AH5_ugroup_t *AH5_init_ugroup(
  AH5_ugroup_t *group, const char *path, hsize_t nb_eles, AH5_group_entitytype_t entitytype)
{

  if (entitytype == AH5_GROUP_ENTITYTYPE_UNDEF || entitytype == AH5_GROUP_ENTITYTYPE_INVALID)
  {
    return NULL;
  }

  if (group)
  {
    group->path = NULL;
    group->entitytype = entitytype;
    group->nb_groupelts = nb_eles;
    group->groupelts = NULL;

    if (nb_eles)
    {
      group->groupelts = (int *)malloc(nb_eles*sizeof(int));
      /*release memory in error.*/
      if (group->groupelts == NULL)
      {
        return NULL;
      }
    }

    if (path)
      AH5_setpath(&group->path, path);
  }

  return group;
}


/**
 * Initialized and allocates unstructured selector on mesh.
 *
 * @param som the initialized selector on mesh
 * @param path the selector on mesh name
 * @param type the selector on mesh type
 * @param size the number of selectors
 *
 * @deprecated AH5_init_usom_table
 *
 * @return On success, a pointer to the selector on mesh. If the
 * function failed to allocate memory, a null pointer is returned.
 */
AH5_usom_table_t *AH5_init_umsh_som(
    AH5_usom_table_t *som, const char *path, hsize_t size, AH5_usom_class_t type) {
  return AH5_init_usom_table(som, path, size, type);
}

AH5_usom_table_t *AH5_init_usom_table(
    AH5_usom_table_t *som, const char *path, hsize_t size, AH5_usom_class_t type) {
  if (som) {
    som->path = NULL;
    som->type = type;

    if (path)
      AH5_setpath(&som->path, path);

    switch (type) {
      case SOM_INVALID:
        return NULL;

      case SOM_POINT_IN_ELEMENT:
        AH5_init_usom_pie_table(&som->data.pie, size);
        break;

      case SOM_EDGE:
      case SOM_FACE:
        AH5_init_usom_ef_table(&som->data.ef, size);
        break;
    }
  }

  return som;
}


AH5_usom_pie_table_t *AH5_init_usom_pie_table(AH5_usom_pie_table_t *som, hsize_t size) {
  hsize_t i, nb_dims = 3;

  if (som) {
    som->nb_dims = nb_dims;
    som->nb_points = size;

    if (size) {
      som->indices = (int *)malloc(size * sizeof(int));

      som->vectors = (float **)malloc(size * sizeof(float *));
      som->vectors[0] = (float *)malloc(size * nb_dims * sizeof(float));
      for (i = 1; i < size; ++i)
        som->vectors[i] = som->vectors[0] + i * nb_dims;

      // Initialize with default value
      for (i = 0; i < size; ++i)
        som->indices[i] = -1;
      for (i = 0; i < size * nb_dims; ++i)
        som->vectors[0][i] = -1;
    }
  }

  return som;
}


AH5_usom_ef_table_t *AH5_init_usom_ef_table(AH5_usom_ef_table_t *som, hsize_t size) {
  hsize_t i, nb_dims = 2;

  if (som) {
    som->dims[0] = size;
    som->dims[1] = nb_dims;

    if (size) {
      som->items = (int *)malloc(nb_dims * size * sizeof(int));

      // Initialize with default value
      for (i = 0; i < nb_dims * size; ++i)
        som->items[i] = -1;
    }
  }

  return som;
}


/**
 * Initialized and allocates structured mesh.
 *
 * @param smesh the initialized mesh
 * @param nb_groups number of groups
 * @param nb_groupgroups numbers of groupgroup
 * @param nb_som_tables number of selector on mesh.
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_smesh_t *AH5_init_smesh(
  AH5_smesh_t *smesh, hsize_t nb_groups, hsize_t nb_groupgroups, hsize_t nb_som_tables)
{
  char success = AH5_TRUE;

  if (smesh)
  {
    AH5_init_axis(&smesh->x, 0);
    AH5_init_axis(&smesh->y, 0);
    AH5_init_axis(&smesh->z, 0);
    smesh->nb_groups = nb_groups;
    smesh->groups = NULL;
    smesh->nb_groupgroups = nb_groupgroups;
    smesh->groupgroups = NULL;
    smesh->nb_som_tables = nb_som_tables;
    smesh->som_tables = NULL;

    if (nb_groups)
    {
      smesh->groups = (AH5_sgroup_t *)malloc(nb_groups*sizeof(AH5_sgroup_t));
      success &= (smesh->groups != NULL);
    }

    if (nb_groupgroups)
    {
      smesh->groupgroups = (AH5_groupgroup_t *)malloc(nb_groupgroups*sizeof(AH5_groupgroup_t));
      success &= (smesh->groupgroups != NULL);
    }

    if (nb_som_tables)
    {
      smesh->som_tables = (AH5_ssom_pie_table_t *)malloc(nb_som_tables*sizeof(AH5_ssom_pie_table_t));
      success &= (smesh->som_tables != NULL);
    }

    /*release memory in error.*/
    if (!success)
    {
      free(smesh->groups);
      free(smesh->groupgroups);
      free(smesh->som_tables);
      return NULL;
    }
  }

  return smesh;
}


/**
 *  Initialized and allocates unstructured mesh.
 *
 * @param umesh the initialized mesh
 * @param nb_elementnodes number of elements' node
 * @param nb_elementtypes number of elements' type
 * @param nb_nodes numbers of nodes
 * @param nb_groups numbers of groups
 * @param nb_groupgroups numbers of group of groups
 * @param nb_som_tables numbers of selector on mesh
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_umesh_t *AH5_init_umesh(
  AH5_umesh_t *umesh, hsize_t nb_elementnodes, hsize_t nb_elementtypes, hsize_t nb_nodes,
  hsize_t nb_groups, hsize_t nb_groupgroups, hsize_t nb_som_tables)
{
  hsize_t i;
  char success = AH5_TRUE;

  if (umesh)
  {
    umesh->nb_elementnodes = nb_elementnodes;
    umesh->elementnodes = NULL;
    umesh->nb_elementtypes = nb_elementtypes;
    umesh->elementtypes = NULL;
    umesh->nb_nodes[0] = nb_nodes;
    umesh->nb_nodes[1] = 0;
    umesh->nodes = NULL;
    umesh->nb_groups = nb_groups;
    umesh->groups = NULL;
    umesh->nb_groupgroups = nb_groupgroups;
    umesh->groupgroups = NULL;
    umesh->nb_som_tables = nb_som_tables;
    umesh->som_tables = NULL;

    /*allocate elements.*/
    if (success && nb_nodes)
    {
      if (nb_elementnodes && nb_elementtypes)
      {
        umesh->elementnodes = (int *)malloc(nb_elementnodes*sizeof(int));
        success &= (umesh->elementnodes != NULL);

        umesh->elementtypes = (char *)malloc(nb_elementtypes*sizeof(char));
        success &= (umesh->elementtypes != NULL);
      }

      umesh->nb_nodes[1] = 3;
      umesh->nodes = (float *)malloc(3*nb_nodes*sizeof(float));
      success &= (umesh->nodes != NULL);

      /*no groups if no elements.*/
      if (nb_groups)
      {
        umesh->groups = (AH5_ugroup_t *)malloc(nb_groups*sizeof(AH5_ugroup_t));
        success &= (umesh->groups != NULL);

        if (success == AH5_TRUE)
          for (i = 0; i < nb_groups; ++i)
            AH5_init_ugroup(umesh->groups + i, NULL, 0, AH5_GROUP_ENTITYTYPE_UNDEF);

        /*no group of groups if no groups.*/
        if (nb_groupgroups)
        {
          umesh->groupgroups = (AH5_groupgroup_t *)malloc(nb_groupgroups*sizeof(AH5_groupgroup_t));//////////////////////
          success &= (umesh->groupgroups != NULL);

          if (success == AH5_TRUE)
            for (i = 0; i < nb_groupgroups; ++i)
              AH5_init_groupgroup(umesh->groupgroups + i, NULL, 0, 0);
        }
      }

      /*no selector on mesh if no elements.*/
      if (nb_som_tables)
      {
        umesh->som_tables = (AH5_usom_table_t *)malloc(nb_som_tables*sizeof(AH5_usom_table_t));
        success &= (umesh->som_tables != NULL);

        if (success == AH5_TRUE)
          for (i = 0; i < nb_som_tables; ++i)
            AH5_init_usom_table(umesh->som_tables + i, NULL, 0, SOM_INVALID);
      }

      /*release memory in error*/
      if (!success)
      {
        free(umesh->elementnodes);
        free(umesh->elementtypes);
        free(umesh->nodes);
        free(umesh->groups);
        free(umesh->groupgroups);
        free(umesh->som_tables);
        return NULL;
      }
    }
  }

  return umesh;
}


/**
 * Initialized and allocates mesh.
 *
 * @param msh_instance the initialized mesh
 * @param path the mesh name
 * @param type the mesh type (MSH_UNSTRUCTURED, MSH_STRUCTURED)
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_msh_instance_t *AH5_init_msh_instance(
  AH5_msh_instance_t *msh_instance, const char *path, AH5_mesh_class_t type)
{
  if (msh_instance)
  {
    msh_instance->path = NULL;
    msh_instance->type = type;

    if (path)
      AH5_setpath(&msh_instance->path, path);

    if (type == MSH_UNSTRUCTURED)
      AH5_init_umesh(&msh_instance->data.unstructured, 0, 0, 0, 0, 0, 0);
    else if (type == MSH_STRUCTURED)
      AH5_init_smesh(&msh_instance->data.structured, 0, 0, 0);
  }

  return msh_instance;
}


/**
 * Initialized and allocates mesh link.
 *
 * @param mlk_instance the initialized mesh
 * @param path the mesh link name
 * @param type the mesh link type (MSHLNK_INVALID, MSHLNK_NODE, MSHLNK_EDGE,
 *   MSHLNK_FACE, MSHLNK_VOLUME)
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_mlk_instance_t *AH5_init_mlk_instance(
  AH5_mlk_instance_t *mlk_instance, const char *path, AH5_meshlink_class_t type)
{
  if (mlk_instance)
  {
    mlk_instance->path = NULL;
    mlk_instance->mesh1 = NULL;
    mlk_instance->mesh2 = NULL;
    mlk_instance->data = NULL;
    mlk_instance->dims[0] = 0;
    mlk_instance->dims[1] = 0;
    mlk_instance->type = type;

    if (path)
      AH5_setpath(&mlk_instance->path, path);
  }

  return mlk_instance;
}


/**
 * Initialized and allocates mesh group.
 *
 * @param msh_group the initialized mesh
 * @param path the mesh group name
 * @param nb_meshs the number of meshs,
 * @param nb_mesh_links the number of mesh links.
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_msh_group_t *AH5_init_msh_group(
  AH5_msh_group_t *msh_group, const char *path, hsize_t nb_meshs, hsize_t nb_mesh_links)
{
  AH5_mlk_instance_t *mlk = NULL;
  AH5_msh_instance_t *msh = NULL;

  if (msh_group)
  {
    msh_group->path = NULL;
    msh_group->nb_msh_instances = nb_meshs;
    msh_group->msh_instances = NULL;
    msh_group->nb_mlk_instances = nb_mesh_links;
    msh_group->mlk_instances = NULL;

    if (path)
      AH5_setpath(&msh_group->path, path);

    if (nb_meshs)
    {
      msh_group->msh_instances = (AH5_msh_instance_t *)malloc(nb_meshs*sizeof(AH5_msh_instance_t));

      if (msh_group->msh_instances == NULL)
        return NULL;

      for (msh = msh_group->msh_instances;
           msh != msh_group->msh_instances + nb_meshs;
           ++msh)
        AH5_init_msh_instance(msh, /*path=*/NULL, MSH_INVALID);
    }

    if (nb_mesh_links)
    {
      msh_group->mlk_instances = (AH5_mlk_instance_t *)malloc(nb_mesh_links*sizeof(AH5_mlk_instance_t));
      if (msh_group->mlk_instances == NULL)
      {
        free(msh_group->msh_instances);
        return NULL;
      }

      for (mlk = msh_group->mlk_instances;
           mlk != msh_group->mlk_instances + nb_mesh_links;
           ++mlk)
        AH5_init_mlk_instance(mlk, NULL, MSHLNK_INVALID);

    }
  }

  return msh_group;
}


/**
 * Initialized and allocates mesh category.
 *
 * @param mesh the initialized mesh
 * @param nb_groups the number of group allocated.
 *
 * @return On success, a pointer to the mesh. If the function failed to
 * allocate memory, a null pointer is returned.
 */
AH5_mesh_t *AH5_init_mesh(AH5_mesh_t *mesh, hsize_t nb_groups)
{
  hsize_t i;
  if (mesh)
  {
    mesh->nb_groups = nb_groups;
    mesh->groups = NULL;

    if (mesh->nb_groups)
    {
      mesh->groups = (AH5_msh_group_t *)malloc(nb_groups*sizeof(AH5_msh_group_t));
      if (mesh->groups == NULL)
        return NULL;

      for (i = 0; i < nb_groups; ++i)
        AH5_init_msh_group(mesh->groups + i, NULL, 0, 0);
    }
  }

  return mesh;
}


// Read groupGroup (both the un/structured)
char AH5_read_groupgroup(hid_t file_id, const char *path, AH5_groupgroup_t *groupgroup)
{
  int nb_dims;
  H5T_class_t type_class;
  size_t length;
  char rdata = AH5_FALSE;

  groupgroup->nb_groupgroupnames = 1;  /* in case of single value */
  groupgroup->path = strdup(path);
  if (AH5_path_valid(file_id, path))
    if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
      if (nb_dims <= 1)
        if (H5LTget_dataset_info(file_id, path, &(groupgroup->nb_groupgroupnames), &type_class,
                                 &length) >= 0)
          if (type_class == H5T_STRING)
            if(AH5_read_str_dataset(file_id, path, groupgroup->nb_groupgroupnames, length,
                                    &(groupgroup->groupgroupnames)))
              rdata = AH5_TRUE;
  if (!rdata)
  {
    AH5_print_err_dset(AH5_C_MESH, path);
    groupgroup->path = NULL;
    groupgroup->nb_groupgroupnames = 0;
    groupgroup->groupgroupnames = NULL;
  }
  return rdata;
}


// Read m x n dataset "/x, /y or /z" (32-bit signed float)
char AH5_read_smesh_axis(hid_t file_id, const char *path, AH5_axis_t *axis)
{
  H5T_class_t type_class;
  int nb_dims;
  size_t length;
  char rdata = AH5_FALSE;

  axis->nb_nodes = 1;
  if (AH5_path_valid(file_id, path))
    if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
      if (nb_dims <= 1)
        if (H5LTget_dataset_info(file_id, path, &(axis->nb_nodes), &type_class, &length) >= 0)
          if (type_class == H5T_FLOAT && length == 4)
            if (AH5_read_flt_dataset(file_id, path, axis->nb_nodes, &(axis->nodes)))
              rdata = AH5_TRUE;
  if (!rdata)
  {
    /* print error only in case of missing "x" */
    if (strcmp(AH5_get_name_from_path(path),"x") == 0)
      AH5_print_err_dset(AH5_C_MESH, path);
    axis->nb_nodes = 0;
    axis->nodes = NULL;
  }
  return rdata;
}


// Read group in structured mesh (+normals)
char AH5_read_smsh_group(hid_t file_id, const char *path, AH5_sgroup_t *sgroup)
{
  return AH5_read_sgroup(file_id, path, sgroup);
}
char AH5_read_sgroup(hid_t file_id, const char *path, AH5_sgroup_t *sgroup)
{
  char *temp, success1 = AH5_FALSE, success2 = AH5_TRUE, success3 = AH5_FALSE, rdata = AH5_TRUE;
  char *normalpath;
  hsize_t dims[2] = {1, 1};
  H5T_class_t type_class;
  size_t length;
  int nb_dims;

  char *type = NULL, *entitytype = NULL;

  sgroup->path = strdup(path);
  sgroup->entitytype = AH5_GROUP_ENTITYTYPE_UNDEF;
  sgroup->normals = NULL;
  sgroup->flat_normals = NULL;
  sgroup->elements = NULL;

  if (AH5_path_valid(file_id, path))
  {
    sgroup->dims[0] = 1;
    sgroup->dims[1] = 1;
    if (!AH5_read_str_attr(file_id, path, AH5_A_TYPE, &type))
    {
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_TYPE);
      success2 = AH5_FALSE;
    }
    if (!AH5_read_str_attr(file_id, path, AH5_A_ENTITY_TYPE, &entitytype))
    {
      if (AH5_strcmp(type, AH5_V_NODE) != 0)
      {
        AH5_print_err_attr(AH5_C_MESH, path, AH5_A_ENTITY_TYPE);
        success2 = AH5_FALSE;
      }
    }

    AH5_read_group_entitytype(type, entitytype, &(sgroup->entitytype));
    free(type);
    free(entitytype);

    if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
      if (nb_dims == 2)
        if (H5LTget_dataset_info(file_id, path, sgroup->dims, &type_class, &length) >= 0)
          if (sgroup->dims[1] >= 1 && sgroup->dims[1] <= 6 && type_class == H5T_INTEGER && length == 4)
            if (AH5_read_int_dataset(file_id, path, sgroup->dims[0] * sgroup->dims[1], &(sgroup->elements)))
              success1 = AH5_TRUE;

    if (!success1)
    {
      AH5_print_err_dset(AH5_C_MESH, path);
      sgroup->dims[0] = 0;  /* in case of invalid dataset only */
      sgroup->dims[1] = 0;
      sgroup->normals = NULL;
      sgroup->flat_normals = NULL;
      sgroup->elements = NULL;
      rdata = AH5_FALSE;
    }
    else
    {
      if (success2)
      {
        if (sgroup->entitytype == AH5_GROUP_FACE)
        {
          /* path = <mesh_path>/group/<group_name> */
          normalpath = malloc((strlen(path) + strlen(AH5_G_NORMAL) - strlen(AH5_G_GROUP) + 1)
                              * sizeof(*normalpath));
          strcpy(normalpath, path);
          temp = strstr(path, "/group/");
          normalpath[temp-path] = '\0';  /* get <mesh_path> */
          temp = AH5_get_name_from_path(path);  /* temp = <group_name> */
          strcat(normalpath, AH5_G_NORMAL);
          strcat(normalpath, "/");
          strcat(normalpath, temp);
          /* normalpath = <mesh_path>/normal/<group_name> */

          // TODO(XXX) read the normals in flat_normal member.
          if (AH5_path_valid(file_id, normalpath))
            if (H5LTget_dataset_ndims(file_id, normalpath, &nb_dims) >= 0)
              if (nb_dims <= 1)
                if (H5LTget_dataset_info(file_id, normalpath, dims, &type_class, &length) >= 0)
                  if (dims[0] == sgroup->dims[0] && type_class == H5T_STRING && length == 2)
                    if(AH5_read_str_dataset(file_id, normalpath, dims[0], length, &(sgroup->normals)))
                      success3 = AH5_TRUE;
          if (!success3)
          {
            AH5_print_err_dset(AH5_C_MESH, normalpath);
            rdata = AH5_FALSE;
          }
          free(normalpath);
        }
      }
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  return rdata;
}


char AH5_read_ssom_pie_table(hid_t file_id, const char *path, AH5_ssom_pie_table_t *som) {
  char success = AH5_FALSE;
  hsize_t nb_fields, nb_dims, nb_points, i, j;
  char **field_names;
  size_t *field_sizes;
  size_t *field_offsets;
  size_t type_size;
  int points_index[] = {0, 1, 2, 3, 4, 5};
  int fields_index[] = {6, 7, 8};
  unsigned int **elements;
  float **vectors;

  if (!som)
    return AH5_FALSE;

  if (AH5_path_valid(file_id, path) &&
      H5TBget_table_info(file_id, path, &nb_fields, &nb_points) >= 0 &&
      (nb_fields == 3 || nb_fields == 6 || nb_fields == 9) && nb_points > 0) {
    field_names = (char **)malloc(nb_fields * sizeof(char *));
    field_names[0] = (char *)malloc(nb_fields * AH5_TABLE_FIELD_NAME_LENGTH * sizeof(char));

    for (i = 1; i < nb_fields; ++i)
      field_names[i] = field_names[0] + i * AH5_TABLE_FIELD_NAME_LENGTH;

    field_sizes = (size_t *)malloc(nb_fields * sizeof(size_t));
    field_offsets = (size_t *)malloc(nb_fields * sizeof(size_t));

    if (H5TBget_field_info(
            file_id, path, field_names, field_sizes, field_offsets, &type_size) >= 0 &&
        ((nb_fields == 9 &&
          AH5_strcmp(field_names[0], AH5_F_IMIN) == 0 &&
          AH5_strcmp(field_names[1], AH5_F_JMIN) == 0 &&
          AH5_strcmp(field_names[2], AH5_F_KMIN) == 0 &&
          AH5_strcmp(field_names[3], AH5_F_IMAX) == 0 &&
          AH5_strcmp(field_names[4], AH5_F_JMAX) == 0 &&
          AH5_strcmp(field_names[5], AH5_F_KMAX) == 0 &&
          AH5_strcmp(field_names[6], AH5_F_V1) == 0 &&
          AH5_strcmp(field_names[7], AH5_F_V2) == 0 &&
          AH5_strcmp(field_names[8], AH5_F_V3) == 0) ||
         (nb_fields == 6 &&
          AH5_strcmp(field_names[0], AH5_F_IMIN) == 0 &&
          AH5_strcmp(field_names[1], AH5_F_JMIN) == 0 &&
          AH5_strcmp(field_names[2], AH5_F_IMAX) == 0 &&
          AH5_strcmp(field_names[3], AH5_F_JMAX) == 0 &&
          AH5_strcmp(field_names[4], AH5_F_V1) == 0 &&
          AH5_strcmp(field_names[5], AH5_F_V2) == 0) ||
         (nb_fields == 3 &&
          AH5_strcmp(field_names[0], AH5_F_IMIN) == 0 &&
          AH5_strcmp(field_names[1], AH5_F_IMAX) == 0 &&
          AH5_strcmp(field_names[2], AH5_F_V1) == 0))) {
      AH5_init_ssom_pie_table(som, path, nb_points);

      nb_dims = nb_fields / 3;

      if (nb_fields == 9) {
        elements = som->elements;
        vectors = som->vectors;
      } else {
        elements = (unsigned int **)malloc(nb_points * sizeof(unsigned int *));
        elements[0] = (unsigned int *)malloc(nb_points * nb_dims * 2 * sizeof(unsigned int));
        for (i = 1; i < nb_points; ++i)
          elements[i] = elements[0] + i * nb_dims * 2;

        vectors = (float **)malloc(nb_points * sizeof(float *));
        vectors[0] = (float *)malloc(nb_points * nb_dims * sizeof(float));
        for (i = 1; i < nb_points; ++i)
          vectors[i] = vectors[0] + i * nb_dims;
      }

      if (H5TBread_fields_index(
              file_id, path, nb_dims * 2, points_index, 0, nb_points,
              nb_dims * 2 * sizeof(int), field_offsets, field_sizes,
              elements[0]) < 0
          ||
          H5TBread_fields_index(
              file_id, path, nb_dims, fields_index, 0, nb_points,
              nb_dims * sizeof(float), field_offsets, field_sizes,
              vectors[0]) < 0) {
        AH5_free_ssom_pie_table(som);

      } else {
        success = AH5_TRUE;

        if (nb_fields != 9) {
          // Transfer values
          for (i = 0; i < nb_points; ++i) {
            for (j = 0; j < nb_dims; ++j) {
              som->elements[i][j] = elements[i][j];
              som->elements[i][j + 3] = elements[i][j + nb_dims];
              som->vectors[i][j] = vectors[i][j];
            }
          }
        }

        // Check values
        for (i = 0; i < nb_points; ++i) {
          for (j = 0; j < nb_dims; ++j) {
            if (som->elements[i][j] > som->elements[i][j + nb_dims] ||
                som->elements[i][j + nb_dims] - som->elements[i][j] > 1)
              AH5_log_warn("Selector on mesh read '%s' id %d: invalid indices", path, i);
            if ((som->vectors[i][j] < 0 && som->vectors[i][j] != -1) || som->vectors[i][j] > 1)
              AH5_log_warn("Selector on mesh read '%s' id %d: invalid v%d", path, i, j + 1);
          }
        }
      }

      if (nb_fields != 9) {
        free(elements[0]);
        free(elements);
        free(vectors[0]);
        free(vectors);
      }
    }

    free(field_names[0]);
    free(field_names);
    free(field_sizes);
    free(field_offsets);
  }

  if (!success)
    AH5_print_err_tble(AH5_C_MESH, path);

  return success;
}


// Read structured mesh
char AH5_read_smesh(hid_t file_id, const char *path, AH5_smesh_t *smesh)
{
  char *path2, *path3;
  AH5_children_t children;
  char *type, success, rdata = AH5_TRUE;
  hsize_t i;

  smesh->groups = NULL;
  smesh->groupgroups = NULL;
  smesh->som_tables = NULL;

  if (AH5_path_valid(file_id, path))
  {
    // X Axis
    path2 = malloc((strlen(path) + strlen(AH5_G_CARTESIAN_GRID) + strlen(AH5_G_X) + 1)
                   * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_CARTESIAN_GRID);
    strcat(path2, AH5_G_X);
    if (!AH5_read_smesh_axis(file_id, path2, &(smesh->x)))
      rdata = AH5_FALSE;

    // Y Axis
    strcpy(path2, path);
    strcat(path2, AH5_G_CARTESIAN_GRID);
    strcat(path2, AH5_G_Y);
    AH5_read_smesh_axis(file_id, path2, &(smesh->y));
    /* problem can be one-dimensional */

    // Z Axis
    strcpy(path2, path);
    strcat(path2, AH5_G_CARTESIAN_GRID);
    strcat(path2, AH5_G_Z);
    AH5_read_smesh_axis(file_id, path2, &(smesh->z));
    /* problem can be two-dimensional */

    // groups
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_GROUP) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_GROUP);
    children = AH5_read_children_name(file_id, path2);
    smesh->nb_groups = children.nb_children;
    if (children.nb_children > 0)
    {
      smesh->groups = (AH5_sgroup_t *) malloc((size_t) children.nb_children * sizeof(AH5_sgroup_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_sgroup(file_id, path3, smesh->groups + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path3);
    }

    // read groupGroup if exists
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_GROUPGROUP) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_GROUPGROUP);
    children = AH5_read_children_name(file_id, path2);
    smesh->nb_groupgroups = children.nb_children;
    if (children.nb_children > 0)
    {
      smesh->groupgroups = (AH5_groupgroup_t *) malloc((size_t) children.nb_children * sizeof(
                             AH5_groupgroup_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_groupgroup(file_id, path3, smesh->groupgroups + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path3);
    }

    // read selectorOnMesh
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_SELECTOR_ON_MESH) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_SELECTOR_ON_MESH);
    children = AH5_read_children_name(file_id, path2);
    smesh->nb_som_tables = children.nb_children;
    if (children.nb_children > 0)
    {
      smesh->som_tables = (AH5_ssom_pie_table_t *) malloc((size_t) children.nb_children * sizeof(
                            AH5_ssom_pie_table_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        AH5_init_ssom_pie_table(smesh->som_tables + i, NULL, 0);

        success = AH5_FALSE;
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (AH5_read_str_attr(file_id, path3, AH5_A_TYPE, &type)) {
          if (AH5_strcmp(type,AH5_V_POINT_IN_ELEMENT) == 0)
            success = AH5_read_ssom_pie_table(file_id, path3, smesh->som_tables + i);
          free(type);
        }
        if (!success) {
          AH5_print_err_attr(AH5_C_MESH, AH5_A_TYPE, path3);
          rdata = AH5_FALSE;
        }
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path3);
    }

    free(path2);
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  return rdata;
}


// Read group in unstructured mesh
char AH5_read_umsh_group(hid_t file_id, const char *path, AH5_ugroup_t *ugroup)
{
  return AH5_read_ugroup(file_id, path, ugroup);
}
char AH5_read_ugroup(hid_t file_id, const char *path, AH5_ugroup_t *ugroup)
{
  H5T_class_t type_class;
  char rdata = AH5_FALSE;
  size_t length;
  int nb_dims;
  char *type, *entitytype;

  ugroup->nb_groupelts = 1; /* see H5LTget_dataset_info() below */
  ugroup->path = strdup(path);
  ugroup->entitytype = AH5_GROUP_ENTITYTYPE_UNDEF;
  ugroup->groupelts = NULL;

  if (AH5_path_valid(file_id, path))
  {
    if(!AH5_read_str_attr(file_id, path, AH5_A_TYPE, &type))
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_TYPE);
    else
    {
      if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
        if (nb_dims <= 1)
          if (H5LTget_dataset_info(file_id, path, &(ugroup->nb_groupelts), &type_class, &length) >= 0)
            if (type_class == H5T_INTEGER && length == 4)
              if (AH5_read_int_dataset(file_id, path, ugroup->nb_groupelts, &(ugroup->groupelts)))
                rdata = AH5_TRUE;
      //XXX Why not point to the constant value (AH5_V_ELEMENT, ...)?
      //      Does not forgot to update free function.
      if(!AH5_read_str_attr(file_id, path, AH5_A_ENTITY_TYPE, &entitytype))
      {
        if (AH5_strcmp(type, AH5_V_NODE) != 0)
        {
          AH5_print_err_attr(AH5_C_MESH, path, AH5_A_ENTITY_TYPE);
          rdata = AH5_FALSE;
        }
      }
      AH5_read_group_entitytype(type, entitytype, &(ugroup->entitytype));
      free(type);
      free(entitytype);
    }
  }
  if (!rdata)
  {
    AH5_print_err_dset(AH5_C_MESH, path);
    ugroup->nb_groupelts = 0;
  }
  return rdata;
}


char AH5_read_usom_pie_table(hid_t file_id, const char *path, AH5_usom_pie_table_t *som) {
  char success = AH5_FALSE;
  hsize_t nb_fields, size, i, j;
  char **field_names;
  size_t *field_sizes;
  size_t *field_offsets;
  size_t type_size;
  int field_index[] = {0, 1, 2, 3};
  float **vectors;

  if (!som)
    return AH5_FALSE;

  if (AH5_path_valid(file_id, path) &&
      H5TBget_table_info(file_id, path, &nb_fields, &size) >= 0 &&
      nb_fields > 1 && nb_fields < 5 && size > 0) {
    field_names = (char **)malloc(nb_fields * sizeof(char *));
    field_names[0] = (char *)malloc(nb_fields * AH5_TABLE_FIELD_NAME_LENGTH * sizeof(char));

    for (i = 1; i < nb_fields; i++)
      field_names[i] = field_names[0] + i * AH5_TABLE_FIELD_NAME_LENGTH;

    field_sizes = (size_t *)malloc(nb_fields * sizeof(size_t));
    field_offsets = (size_t *)malloc(nb_fields * sizeof(size_t));

    if (H5TBget_field_info(
            file_id, path, field_names, field_sizes, field_offsets, &type_size) >= 0 &&
        AH5_strcmp(field_names[0], AH5_F_INDEX) == 0 &&
        AH5_strcmp(field_names[1], AH5_F_V1) == 0 &&
        (nb_fields < 3 || AH5_strcmp(field_names[2], AH5_F_V2) == 0) &&
        (nb_fields < 4 || AH5_strcmp(field_names[3], AH5_F_V3) == 0)) {
      AH5_init_usom_pie_table(som, size);

      if (nb_fields == 4) {
        vectors = som->vectors;
      } else {
        vectors = (float **)malloc(size * sizeof(float *));
        vectors[0] = (float *)malloc(size * (nb_fields - 1) * sizeof(float));
        for (i = 1; i < size; ++i)
          vectors[i] = vectors[0] + i * (nb_fields - 1);
      }

      if (H5TBread_fields_index(
              file_id, path, 1, field_index, 0, size, sizeof(int),
              field_offsets, field_sizes, som->indices) < 0
          ||
          H5TBread_fields_index(
              file_id, path, (nb_fields - 1), field_index + 1, 0, size,
              (nb_fields - 1) * sizeof(float), field_offsets, field_sizes,
              vectors[0]) < 0) {
        AH5_free_usom_pie_table(som);

      } else {
        success = AH5_TRUE;

        if (nb_fields != 4) {
          // Transfer values
          for (i = 0; i < size; ++i)
            for (j = 0; j < nb_fields - 1; ++j)
              som->vectors[i][j] = vectors[i][j];
        }

        // Check values
        for (i = 0; i < size; ++i) {
          if (som->indices[i] < 0)
            AH5_log_warn("Selector on mesh read '%s' id %d: invalid index", path, i);
          for (j = 0; j < nb_fields - 1; ++j)
            if ((som->vectors[i][j] < 0 && som->vectors[i][j] != -1) ||
                som->vectors[i][j] > 1)
              AH5_log_warn("Selector on mesh read '%s' id %d: invalid v%d", path, i, j + 1);
        }
      }

      if (nb_fields != 4) {
        free(vectors[0]);
        free(vectors);
      }
    }

    free(field_names[0]);
    free(field_names);
    free(field_sizes);
    free(field_offsets);
  }

  if (!success)
    AH5_print_err_tble(AH5_C_MESH, path);

  return success;
}


char AH5_read_usom_ef_table(hid_t file_id, const char *path, AH5_usom_ef_table_t *som) {
  char success = AH5_FALSE;
  int nb_dims;
  H5T_class_t type_class;
  size_t type_size;

  if (!som)
    return AH5_FALSE;

  if (AH5_path_valid(file_id, path) &&
      H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0 && nb_dims == 2 &&
      H5LTget_dataset_info(file_id, path, som->dims, &type_class, &type_size) >= 0 &&
      type_class == H5T_INTEGER && type_size == 4)
    success = AH5_read_int_dataset(file_id, path, som->dims[0] * som->dims[1], &(som->items));

  if (!success) {
    AH5_init_usom_ef_table(som, 0);
    AH5_print_err_dset(AH5_C_MESH, path);
  }

  return success;
}


char AH5_read_umesh_som_table(hid_t file_id, const char *path, AH5_usom_table_t *som)
{
  return AH5_read_usom_table(file_id, path, som);
}
char AH5_read_usom_table(hid_t file_id, const char *path, AH5_usom_table_t *som) {
  char success = AH5_FALSE;
  char *type;

  if (!som)
    return AH5_FALSE;

  if (AH5_path_valid(file_id, path)) {
    if (AH5_read_str_attr(file_id, path, AH5_A_TYPE, &type)) {
      if (AH5_strcmp(type, AH5_V_POINT_IN_ELEMENT) == 0) {
        AH5_init_usom_table(som, path, 0, SOM_POINT_IN_ELEMENT);
        success = AH5_read_usom_pie_table(file_id, path, &(som->data.pie));

      } else if (AH5_strcmp(type, AH5_V_EDGE) == 0) {
        AH5_init_usom_table(som, path, 0, SOM_EDGE);
        success = AH5_read_usom_ef_table(file_id, path, &(som->data.ef));

      } else if (AH5_strcmp(type, AH5_V_FACE) == 0) {
        AH5_init_usom_table(som, path, 0, SOM_FACE);
        success = AH5_read_usom_ef_table(file_id, path, &(som->data.ef));
      }

      free(type);

    } else {
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_TYPE);
    }
  }

  if (!success)
    AH5_print_err_path(AH5_C_MESH, path);

  return success;
}


// Read unstructured mesh
char AH5_read_umesh(hid_t file_id, const char *path, AH5_umesh_t *umesh)
{
  char *path2, *path3;
  char rdata = AH5_TRUE, success = AH5_FALSE;
  hsize_t i;
  int nb_dims;
  H5T_class_t type_class;
  size_t length;
  hid_t dset_id;
  AH5_children_t children;

  umesh->elementnodes = NULL;
  umesh->elementtypes = NULL;
  umesh->nodes = NULL;
  umesh->groups = NULL;
  umesh->groupgroups = NULL;
  umesh->som_tables = NULL;

  if (AH5_path_valid(file_id, path))
  {
    // Read m x 1 dataset "elementNodes" (32-bit signed integer)
    umesh->nb_elementnodes = 1;
    path2 = malloc((strlen(path) + strlen(AH5_G_ELEMENT_NODES) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_ELEMENT_NODES);
    if (AH5_path_valid(file_id, path2)) {
      if (H5LTget_dataset_ndims(file_id, path2, &nb_dims) >= 0)
        if (nb_dims <= 1)
          if (H5LTget_dataset_info(file_id, path2, &(umesh->nb_elementnodes), &type_class, &length) >= 0)
            if (type_class == H5T_INTEGER)
              if (AH5_read_int_dataset(file_id, path2, umesh->nb_elementnodes, &(umesh->elementnodes)))
                success = AH5_TRUE;
    }
    else
    {
      umesh->nb_elementnodes = 0;
      success = AH5_TRUE;
    }
    if (!success)
    {
      AH5_print_err_dset(AH5_C_MESH, path2);
      umesh->nb_elementnodes = 0;
      rdata = AH5_FALSE;
    }

    success = AH5_FALSE;

    // Read m x 1 dataset "elementTypes" (8-bit signed char)
    umesh->nb_elementtypes = 1;
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_ELEMENT_TYPES) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_ELEMENT_TYPES);
    if (AH5_path_valid(file_id, path2)) {
      if (H5LTget_dataset_ndims(file_id, path2, &nb_dims) >= 0)
        if (nb_dims <= 1)
          if (H5LTget_dataset_info(file_id, path2, &(umesh->nb_elementtypes), &type_class, &length) >= 0)
            if (type_class == H5T_INTEGER)
            {
              umesh->elementtypes = (char *) malloc((size_t) umesh->nb_elementtypes * sizeof(char));
              dset_id = H5Dopen(file_id, path2, H5P_DEFAULT);
              if (H5Dread(dset_id, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, umesh->elementtypes) >= 0)
                success = AH5_TRUE;
              H5Dclose(dset_id);
              if (!success)
              {
                free(umesh->elementtypes);
                umesh->elementtypes = NULL;
              }
            }
    }
    else
    {
      umesh->nb_elementtypes = 0;
      success = AH5_TRUE;
    }
    if (!success)
    {
      AH5_print_err_dset(AH5_C_MESH, path2);
      umesh->nb_elementtypes = 0;
      rdata = AH5_FALSE;
    }

    success = AH5_FALSE;
    umesh->nb_nodes[0] = 1;
    umesh->nb_nodes[1] = 1;
    // Read m x n dataset "nodes" (32-bit signed float)
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_NODES) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_NODES);
    if (AH5_path_valid(file_id, path2))
      if (H5LTget_dataset_ndims(file_id, path2, &nb_dims) >= 0)
        if (nb_dims == 2)
          if (H5LTget_dataset_info(file_id, path2, umesh->nb_nodes, &type_class, &length) >= 0)
            if (umesh->nb_nodes[1] <= 3 && type_class == H5T_FLOAT && length == 4)
              if (AH5_read_flt_dataset(file_id, path2, umesh->nb_nodes[0] * umesh->nb_nodes[1], &(umesh->nodes)))
                success = AH5_TRUE;
    if (!success)
    {
      AH5_print_err_dset(AH5_C_MESH, path2);
      umesh->nb_nodes[0] = 0;
      umesh->nb_nodes[1] = 0;
      rdata = AH5_FALSE;
    }

    // read groupGroup if exists
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_GROUPGROUP) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_GROUPGROUP);
    children = AH5_read_children_name(file_id, path2);
    umesh->nb_groupgroups = children.nb_children;
    if (children.nb_children > 0)
    {
      umesh->groupgroups = (AH5_groupgroup_t *) malloc((size_t) children.nb_children * sizeof(
                             AH5_groupgroup_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_groupgroup(file_id, path3, umesh->groupgroups + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path3);
    }

    // read group
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_GROUP) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_GROUP);
    children = AH5_read_children_name(file_id, path2);
    umesh->nb_groups = children.nb_children;
    if (children.nb_children > 0)
    {
      umesh->groups = (AH5_ugroup_t *) malloc((size_t) children.nb_children * sizeof(AH5_ugroup_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_ugroup(file_id, path3, umesh->groups + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(path3);
      free(children.childnames);
    }

    // read selectorOnMesh
    path2 = realloc(path2, (strlen(path) + strlen(AH5_G_SELECTOR_ON_MESH) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_SELECTOR_ON_MESH);
    children = AH5_read_children_name(file_id, path2);
    umesh->nb_som_tables = children.nb_children;
    if (children.nb_children > 0)
    {
      umesh->som_tables = (AH5_usom_table_t *) malloc((size_t) children.nb_children * sizeof(
                            AH5_usom_table_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path3));
      for (i = 0; i < children.nb_children; i++)
      {
        AH5_init_usom_table(umesh->som_tables + i, NULL, 0, SOM_INVALID);

        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path3));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_usom_table(file_id, path3, umesh->som_tables + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(path3);
      free(children.childnames);
    }

    free(path2);
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  return rdata;
}


// Read mesh instance
char AH5_read_msh_instance(hid_t file_id, const char *path, AH5_msh_instance_t *msh_instance)
{
  char *type, rdata = AH5_TRUE;

  msh_instance->path = strdup(path);
  msh_instance->type = MSH_INVALID;

  if (AH5_path_valid(file_id, path))
  {
    if (AH5_read_str_attr(file_id, path, AH5_A_TYPE, &type))
    {
      if (AH5_strcmp(type, AH5_V_STRUCTURED) == 0)
      {
        msh_instance->type = MSH_STRUCTURED;
        if (!AH5_read_smesh(file_id, path, &(msh_instance->data.structured)))
          rdata = AH5_FALSE;
      }
      else if (AH5_strcmp(type, AH5_V_UNSTRUCTURED) == 0)
      {
        msh_instance->type = MSH_UNSTRUCTURED;
        if (!AH5_read_umesh(file_id, path, &(msh_instance->data.unstructured)))
          rdata = AH5_FALSE;
      }
      else
      {
        printf("***** ERROR(%s): Unexpected attribute value of \"%s@%s=%s\". *****\n\n",
               AH5_C_MESH, path, AH5_A_TYPE, type);
        rdata = AH5_FALSE;
      }
      free(type);
    }
    else
    {
      msh_instance->type = MSH_INVALID;
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_TYPE);
      rdata = AH5_FALSE;
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  return rdata;
}


// Read meshLink instance
char AH5_read_mlk_instance(hid_t file_id, const char *path, AH5_mlk_instance_t *mlk_instance)
{
  char *type, dataset_read = AH5_FALSE, rdata = AH5_TRUE;
  H5T_class_t type_class;
  size_t length;
  int nb_dims;

  mlk_instance->path = strdup(path);
  mlk_instance->mesh1 = NULL;
  mlk_instance->mesh2 = NULL;
  mlk_instance->data = NULL;
  mlk_instance->type = MSHLNK_INVALID;

  if (AH5_path_valid(file_id, path))
  {
    if (!AH5_read_str_attr(file_id, path, AH5_A_MESH1, &(mlk_instance->mesh1)))
    {
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_MESH1);
      rdata = AH5_FALSE;
    }
    if (!AH5_read_str_attr(file_id, path, AH5_A_MESH2, &(mlk_instance->mesh2)))
    {
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_MESH2);
      rdata = AH5_FALSE;
    }

    if (!AH5_read_str_attr(file_id, path, AH5_A_TYPE, &type))
    {
      AH5_print_err_attr(AH5_C_MESH, path, AH5_A_TYPE);
      rdata = AH5_FALSE;
    }

    if (AH5_path_valid(file_id, path))
      if (H5LTget_dataset_ndims(file_id, path, &nb_dims) >= 0)
        if (nb_dims == 2)
          if (H5LTget_dataset_info(file_id, path, mlk_instance->dims, &type_class, &length) >= 0)
            if (type_class == H5T_INTEGER)
              if (AH5_read_int_dataset(file_id, path, mlk_instance->dims[0] * mlk_instance->dims[1],
                                       &(mlk_instance->data)))
                dataset_read = AH5_TRUE;
    if (!dataset_read)
    {
      AH5_print_err_dset(AH5_C_MESH, path);
      mlk_instance->dims[0] = 0;
      mlk_instance->dims[1] = 0;
      rdata = AH5_FALSE;
    }

    if (rdata)
    {
      if (AH5_strcmp(type, AH5_V_NODE))
        mlk_instance->type = MSHLNK_NODE;
      else if (AH5_strcmp(type, AH5_V_EDGE))
        mlk_instance->type = MSHLNK_EDGE;
      else if (AH5_strcmp(type, AH5_V_FACE))
        mlk_instance->type = MSHLNK_FACE;
      else if (AH5_strcmp(type, AH5_V_VOLUME))
        mlk_instance->type = MSHLNK_VOLUME;
    }
    else
      mlk_instance->type = MSHLNK_INVALID;
    if (type != NULL)
    {
      free(type);
      type = NULL;
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  return rdata;
}



// Read mesh group
char AH5_read_msh_group(hid_t file_id, const char *path, AH5_msh_group_t *msh_group)
{
  char *path2 = NULL;
  char *path3 = NULL;
  char rdata = AH5_TRUE;
  AH5_children_t children;
  hsize_t i, j = 0;

  msh_group->path = strdup(path);
  msh_group->msh_instances = NULL;
  msh_group->mlk_instances = NULL;

  if (AH5_path_valid(file_id, path))
  {
    children = AH5_read_children_name(file_id, path);
    msh_group->nb_msh_instances = children.nb_children;
    for (i = 0; i < children.nb_children; i++)
      if (AH5_strcmp(children.childnames[i], AH5_G_MESH_LINK) == 0)
        msh_group->nb_msh_instances--;    // do not count /meshLink
    if (children.nb_children > 0)
    {
      msh_group->msh_instances = (AH5_msh_instance_t *) malloc((size_t) msh_group->nb_msh_instances *
                                 sizeof(AH5_msh_instance_t));
      path2 = malloc((strlen(path) + 1) * sizeof(*path2));
      for (i = 0; i < children.nb_children; i++)
      {
        if (AH5_strcmp(children.childnames[i], AH5_G_MESH_LINK) != 0)
        {
          path2 = realloc(path2, (strlen(path) + strlen(children.childnames[i]) + 1) * sizeof(*path2));
          strcpy(path2, path);
          strcat(path2, children.childnames[i]);
          if (!AH5_read_msh_instance(file_id, path2, msh_group->msh_instances + j++))
            rdata = AH5_FALSE;
        }
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path2);
    }

    path2 = malloc((strlen(path) + strlen(AH5_G_MESH_LINK) + 1) * sizeof(*path2));
    strcpy(path2, path);
    strcat(path2, AH5_G_MESH_LINK);
    children = AH5_read_children_name(file_id, path2);
    msh_group->nb_mlk_instances = children.nb_children;
    if (children.nb_children > 0)
    {
      msh_group->mlk_instances = (AH5_mlk_instance_t *) malloc((size_t) children.nb_children * sizeof(
                                   AH5_mlk_instance_t));
      path3 = malloc((strlen(path2) + 1) * sizeof(*path2));
      for (i = 0; i < children.nb_children; i++)
      {
        path3 = realloc(path3, (strlen(path2) + strlen(children.childnames[i]) + 1) * sizeof(*path2));
        strcpy(path3, path2);
        strcat(path3, children.childnames[i]);
        if (!AH5_read_mlk_instance(file_id, path3, msh_group->mlk_instances + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path3);
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, path);
    rdata = AH5_FALSE;
  }
  if (path2 != NULL) free(path2);
  if (path3 != NULL) free(path3);
  return rdata;
}


// Read mesh category
char AH5_read_mesh(hid_t file_id, AH5_mesh_t *mesh)
{
  char *path, rdata = AH5_TRUE;
  AH5_children_t children;
  hsize_t i;

  mesh->groups = NULL;

  if (AH5_path_valid(file_id, AH5_C_MESH))
  {
    children = AH5_read_children_name(file_id, AH5_C_MESH);
    mesh->nb_groups = children.nb_children;
    if (children.nb_children > 0)
    {
      path = malloc((strlen(AH5_C_MESH) + 1) * sizeof(*path));
      mesh->groups = (AH5_msh_group_t *) malloc((size_t) children.nb_children * sizeof(AH5_msh_group_t));
      for (i = 0; i < children.nb_children; i++)
      {
        path = realloc(path, (strlen(AH5_C_MESH) + strlen(children.childnames[i]) + 1) * sizeof(*path));
        strcpy(path, AH5_C_MESH);
        strcat(path, children.childnames[i]);
        if (!AH5_read_msh_group(file_id, path, mesh->groups + i))
          rdata = AH5_FALSE;
        free(children.childnames[i]);
      }
      free(children.childnames);
      free(path);
    }
  }
  else
  {
    AH5_print_err_path(AH5_C_MESH, AH5_C_MESH);
    rdata = AH5_FALSE;
  }
  return rdata;
}


// Write group of structured mesh
char AH5_write_smsh_group(hid_t id, const AH5_sgroup_t* sgroup) {
  return AH5_write_sgroup(id, sgroup);
}
char AH5_write_sgroup(hid_t id, const AH5_sgroup_t* sgroup) {
  hid_t loc_id;
  char* basename;
  char* ctype;
  char* centitytype;
  char success = AH5_TRUE;

  // Invalid group
  if (sgroup == NULL)
    return AH5_FALSE;

  // Empty group
  if (sgroup->dims[0] == 0)
    return AH5_TRUE;

  basename = AH5_get_name_from_path(sgroup->path);

  // Open / create "groups" node
  if (AH5_path_valid(id, AH5_CATEGORY_NAME(AH5_G_GROUP))) {
    loc_id = H5Gopen(
        id, AH5_CATEGORY_NAME(AH5_G_GROUP), H5P_DEFAULT);
  } else {
    loc_id = H5Gcreate(
        id, AH5_CATEGORY_NAME(AH5_G_GROUP),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }

  if (loc_id < 0)
    return AH5_FALSE;

  // Write m x 6 dataset "elements" (32-bit signed int)
  if (AH5_write_int_array(loc_id, basename, 2, sgroup->dims, sgroup->elements)) {
    success = AH5_TRUE;

    // Get type and entityType
    AH5_write_group_entitytype(sgroup->entitytype, &ctype, &centitytype);

    // Set type
    success &= AH5_write_str_attr(loc_id, basename, AH5_A_TYPE, ctype);

    // Set entityType
    if (centitytype != NULL)
      success &= AH5_write_str_attr(loc_id, basename, AH5_A_ENTITY_TYPE, centitytype);
  }

  // Close "groups" node
  if (HDF5_FAILED(H5Gclose(loc_id)) || !success)
    return AH5_FALSE;


  // Surface normals
  if (sgroup->entitytype == AH5_GROUP_FACE) {
    // Open / create "normal" node
    if (AH5_path_valid(id, AH5_CATEGORY_NAME(AH5_G_NORMAL))) {
      loc_id = H5Gopen(
          id, AH5_CATEGORY_NAME(AH5_G_NORMAL), H5P_DEFAULT);
    } else {
      loc_id = H5Gcreate(
          id, AH5_CATEGORY_NAME(AH5_G_NORMAL),
          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }

    if (loc_id < 0)
      return AH5_FALSE;

    // Write m x 1 dataset "normal" (str)
    if (sgroup->flat_normals == NULL)
    {
      success = AH5_write_str_dataset(loc_id, basename, sgroup->dims[0], 2, sgroup->normals);
    } else
    {
      success = AH5_write_flat_str_dataset(loc_id, basename, sgroup->dims[0], 2, sgroup->flat_normals);
    }

    // Close "normal" node
    if (HDF5_FAILED(H5Gclose(loc_id)) || !success)
      return AH5_FALSE;
  }

  return AH5_TRUE;
}


typedef struct AH5_ssom_pie_t {
  int element[6];
  float vector[3];
} AH5_ssom_pie_t;

char AH5_write_ssom_pie_table(hid_t id, const AH5_ssom_pie_table_t *som) {
  char success = AH5_TRUE;
  hsize_t nb_fields, i, j;
  hid_t loc_id;
  char* basename;
  char** field_names;
  size_t* field_offsets;
  hid_t* field_types;
  AH5_ssom_pie_t* data;

  if (som && som->path && som->nb_dims && som->nb_points) {
    if (som->elements && som->elements[0] &&
        som->vectors && som->vectors[0]) {
      // Check values
      for (i = 0; i < som->nb_points; ++i) {
        for (j = 0; j < som->nb_dims; ++j) {
          if (som->elements[i][j] > som->elements[i][j + som->nb_dims] ||
              som->elements[i][j + som->nb_dims] - som->elements[i][j] > 1)
            AH5_log_warn("Selector on mesh write '%s' id %d: invalid indices", som->path, i);
          if ((som->vectors[i][j] < 0 && som->vectors[i][j] != -1) || som->vectors[i][j] > 1)
            AH5_log_warn("Selector on mesh write '%s' id %d: invalid v%d", som->path, i, j + 1);
        }
      }

      // Non empty selector on mesh table
      basename = AH5_get_name_from_path(som->path);

      // Open / create selector on mesh node
      if (AH5_path_valid(id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH))) {
        loc_id = H5Gopen(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH), H5P_DEFAULT);
      } else {
        loc_id = H5Gcreate(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      }

      if (loc_id >= 0) {
        nb_fields = som->nb_dims * 3;

        // Generate write data
        field_names = (char **)malloc(nb_fields * sizeof(char *));
        field_names[0] = (char *)malloc(
            nb_fields * AH5_TABLE_FIELD_NAME_LENGTH * sizeof(char));

        field_offsets = (size_t *)malloc(nb_fields * sizeof(size_t));
        field_types = (hid_t *)malloc(nb_fields * sizeof(hid_t));

        for (i = 0; i < nb_fields; ++i) {
          field_names[i] = field_names[0] + i * AH5_TABLE_FIELD_NAME_LENGTH;
          field_names[i][0] = '\0';

          if (i < som->nb_dims * 2) {
            field_offsets[i] = i * sizeof(int);
            field_types[i] = H5T_NATIVE_UINT;
          } else {
            field_offsets[i] = (som->nb_dims * 2) * sizeof(int) +
                               (i - som->nb_dims * 2) * sizeof(float);
            field_types[i] = H5T_NATIVE_FLOAT;
          }
        }

        // fields: imin jmin kmin imax jmax kmax v1 v2 v3
        strcat(field_names[0], AH5_F_IMIN);
        strcat(field_names[1], AH5_F_JMIN);
        strcat(field_names[2], AH5_F_KMIN);
        strcat(field_names[3], AH5_F_IMAX);
        strcat(field_names[4], AH5_F_JMAX);
        strcat(field_names[5], AH5_F_KMAX);
        strcat(field_names[6], AH5_F_V1);
        strcat(field_names[7], AH5_F_V2);
        strcat(field_names[8], AH5_F_V3);

        // Fill write data
        data = (AH5_ssom_pie_t *)malloc(som->nb_points * sizeof(AH5_ssom_pie_t));
        for (i = 0; i < som->nb_points; ++i) {
          for (j = 0; j < som->nb_dims; ++j) {
            data[i].element[j] = som->elements[i][j];
            data[i].element[j + som->nb_dims] = som->elements[i][j + som->nb_dims];
            data[i].vector[j] = som->vectors[i][j];
          }
        }

        // Write selector on mesh table
        if (H5TBmake_table(
                basename,  // not used
                loc_id, basename, nb_fields, som->nb_points, sizeof(AH5_ssom_pie_t),
                (const char**)field_names, field_offsets, field_types, nb_fields, NULL, 1, data) < 0) {
          success = AH5_FALSE;

        } else {
          // Set pie attribute
          success &= AH5_write_str_attr(
              loc_id, basename, AH5_A_TYPE, AH5_V_POINT_IN_ELEMENT);
        }

        free(field_names[0]);
        free(field_names);
        free(field_offsets);
        free(field_types);
        free(data);

        success &= !HDF5_FAILED(H5Gclose(loc_id));

      } else {
        success = AH5_FALSE;
      }

    } else {
      success = AH5_FALSE;
    }
  }

  return success;
}


// Write structured mesh
char AH5_write_smesh(hid_t msh_id, const AH5_smesh_t *smesh)
{
  hsize_t i;
  hid_t loc_id;

  // Check smesh sanity first
  if (smesh == NULL ||
      smesh->x.nodes == NULL ||
      smesh->y.nodes == NULL ||
      smesh->z.nodes == NULL)
    return AH5_FALSE;


  // Mesh type
  if (!AH5_write_str_attr(msh_id, ".", AH5_A_TYPE, AH5_V_STRUCTURED))
    return AH5_FALSE;


  // Open / create cartesianGrid
  if (AH5_path_valid(msh_id, AH5_CATEGORY_NAME(AH5_G_CARTESIAN_GRID)))
    loc_id = H5Gopen(msh_id, AH5_CATEGORY_NAME(AH5_G_CARTESIAN_GRID), H5P_DEFAULT);
  else
    loc_id = H5Gcreate(
        msh_id, AH5_CATEGORY_NAME(AH5_G_CARTESIAN_GRID),
        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (loc_id < 0)
    return AH5_FALSE;

  // Write m x 1 dataset "x" (32-bit signed float)
  if (!AH5_write_flt_dataset(
          loc_id, AH5_CATEGORY_NAME(AH5_G_X), smesh->x.nb_nodes, smesh->x.nodes))
    return AH5_FALSE;

  // Write m x 1 dataset "y" (32-bit signed float)
  if (!AH5_write_flt_dataset(
          loc_id, AH5_CATEGORY_NAME(AH5_G_Y), smesh->y.nb_nodes, smesh->y.nodes))
    return AH5_FALSE;

  // Write m x 1 dataset "z" (32-bit signed float)
  if (!AH5_write_flt_dataset(
          loc_id, AH5_CATEGORY_NAME(AH5_G_Z), smesh->z.nb_nodes, smesh->z.nodes))
    return AH5_FALSE;

  // Close cartesianGrid
  if (HDF5_FAILED(H5Gclose(loc_id)))
    return AH5_FALSE;

  // Write groups
  if (smesh->nb_groups)
    for (i = 0; i < smesh->nb_groups; ++i)
      if (!AH5_write_sgroup(msh_id, smesh->groups + i))
        return AH5_FALSE;

  // Write groupGroups
  if (smesh->nb_groupgroups)
    if (!AH5_write_groupgroup(msh_id, smesh->groupgroups, smesh->nb_groupgroups))
      return AH5_FALSE;

  // Write selectorOnMesh
  if (smesh->nb_som_tables)
    for (i = 0; i < smesh->nb_som_tables; ++i)
      if (!AH5_write_ssom_pie_table(msh_id, smesh->som_tables + i))
        return AH5_FALSE;

  return AH5_TRUE;
}

// Write groupGroup
char AH5_write_groupgroup(hid_t loc_id, const AH5_groupgroup_t *groupgroup, hsize_t nb_ggrp)
{
  char success = AH5_FALSE;
  hid_t grp;
  hsize_t i, j;
  char *ggrp_name;
  hsize_t slen, len;

  // NMT: I prefer build an empty group, because I am not sure that everyone
  // check that the group exist before to open it.
  if (AH5_path_valid(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUPGROUP)))
    grp = H5Gopen(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUPGROUP), H5P_DEFAULT);
  else
    grp = H5Gcreate(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUPGROUP), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  success = AH5_TRUE;
  for (i = 0; i < nb_ggrp; i++)
  {
    if (groupgroup[i].nb_groupgroupnames > 0)
    {
      if (groupgroup[i].path != NULL)
      {
        ggrp_name = AH5_get_name_from_path(groupgroup[i].path);

        // Get longest string
        slen = 0;
        for (j = 0; j < groupgroup[i].nb_groupgroupnames; ++j) {
          len = strlen(groupgroup[i].groupgroupnames[j]) + 1;
          if (len > slen) slen = len;
        }

        success &= AH5_write_str_dataset(
            grp, ggrp_name, groupgroup[i].nb_groupgroupnames,
            slen, groupgroup[i].groupgroupnames);
      }
      else
      {
        success &= AH5_FALSE;
      }
    }
  }

  success &= !HDF5_FAILED(H5Gclose(grp));

  return success;
}

// Write group in unstructured mesh
char AH5_write_umsh_group(hid_t loc_id, const AH5_ugroup_t *ugroup, hsize_t nb_ugroup)
{
  return AH5_write_ugroup(loc_id, ugroup, nb_ugroup);
}
char AH5_write_ugroup(hid_t loc_id, const AH5_ugroup_t *ugroup, hsize_t nb_ugroup)
{
  char success = AH5_FALSE;
  hsize_t i;
  hid_t grp;
  char *basename;
  char *ctype, *centitytype;

  // NMT: I prefer build an empty group, because I am not sure that everyone
  // check that the group exist before to open it.
  if (AH5_path_valid(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUP)))
    grp = H5Gopen(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUP), H5P_DEFAULT);
  else
    grp = H5Gcreate(loc_id, AH5_CATEGORY_NAME(AH5_G_GROUP), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  if (grp >= 0) {
    if (nb_ugroup == 0)
    {
      success = AH5_TRUE;
    }
    else
    {
      for (i = 0; i < nb_ugroup; ++i)
      {
        if (ugroup[i].nb_groupelts > 0)
        {
          basename = AH5_get_name_from_path(ugroup[i].path);  // No allocation.
          if (AH5_write_int_dataset(grp, basename, ugroup[i].nb_groupelts, ugroup[i].groupelts))
          {
            AH5_write_group_entitytype(ugroup[i].entitytype, &ctype, &centitytype);
            if (AH5_write_str_attr(grp, basename, AH5_A_TYPE, ctype))
              success = AH5_TRUE;
            if (AH5_strcmp(ctype, AH5_V_NODE))
              success = AH5_write_str_attr(grp, basename, AH5_A_ENTITY_TYPE, centitytype);
          }
          if (!success)
            break;
        }
        else
        {
          success = AH5_FALSE;
          break;
        }
      }
    }
    success &= !HDF5_FAILED(H5Gclose(grp));
  }
  return success;
}


typedef struct AH5_usom_pie_t {
  int indice;
  float vector[3];
} AH5_usom_pie_t;

char AH5_write_usom_pie_table(hid_t id, const AH5_usom_pie_table_t *som, const char *path) {
  char success = AH5_TRUE;
  hsize_t nb_fields, i, j;
  hid_t loc_id;
  char* basename;
  char **field_names;
  size_t *field_offsets;
  hid_t *field_types;
  AH5_usom_pie_t* data;

  if (som && som->nb_dims && som->nb_points) {
    if (som->indices && som->vectors && som->vectors[0]) {
      // Check values
      for (i = 0; i < som->nb_points; ++i) {
        if (som->indices[i] < 0)
          AH5_log_warn("Selector on mesh read '%s' id %d: invalid index", path, i);
        for (j = 0; j < som->nb_dims; ++j)
          if ((som->vectors[i][j] < 0 && som->vectors[i][j] != -1) ||
              som->vectors[i][j] > 1)
            AH5_log_warn("Selector on mesh read '%s' id %d: invalid v%d", path, i, j + 1);
      }

      // Non empty selector on mesh table
      basename = AH5_get_name_from_path(path);

      // Open / create selector on mesh node
      if (AH5_path_valid(id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH))) {
        loc_id = H5Gopen(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH), H5P_DEFAULT);
      } else {
        loc_id = H5Gcreate(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      }

      if (loc_id >= 0) {
        nb_fields = 1 + som->nb_dims;

        // Generate write data
        field_names = (char **)malloc(nb_fields * sizeof(char *));
        field_names[0] = (char *)malloc(
            nb_fields * AH5_TABLE_FIELD_NAME_LENGTH * sizeof(char));

        field_offsets = (size_t *)malloc(nb_fields * sizeof(size_t));
        field_types = (hid_t *)malloc(nb_fields * sizeof(hid_t));

        for (i = 0; i < nb_fields; ++i) {
          field_names[i] = field_names[0] + i * AH5_TABLE_FIELD_NAME_LENGTH;
          field_names[i][0] = '\0';

          if (i == 0) {
            field_offsets[i] = i * sizeof(int);
            field_types[i] = H5T_NATIVE_UINT;
          } else {
            field_offsets[i] = sizeof(int) + (i - 1) * sizeof(float);
            field_types[i] = H5T_NATIVE_FLOAT;
          }
        }

        // fields: index v1 v2 v3
        strcat(field_names[0], AH5_F_INDEX);
        strcat(field_names[1], AH5_F_V1);
        strcat(field_names[2], AH5_F_V2);
        strcat(field_names[3], AH5_F_V3);

        // Fill write data
        data = (AH5_usom_pie_t *)malloc(som->nb_points * sizeof(AH5_usom_pie_t));
        for (i = 0; i < som->nb_points; ++i) {
          data[i].indice = som->indices[i];
          for (j = 0; j < som->nb_dims; ++j)
            data[i].vector[j] = som->vectors[i][j];
        }

        // Write selector on mesh table
        if (H5TBmake_table(
                basename,  // not used
                loc_id, basename, nb_fields, som->nb_points, sizeof(AH5_usom_pie_t),
                (const char**)field_names, field_offsets, field_types, nb_fields, NULL, 1, data) < 0) {
          success = AH5_FALSE;

        } else {
          // Set pie attribute
          success &= AH5_write_str_attr(
              loc_id, basename, AH5_A_TYPE, AH5_V_POINT_IN_ELEMENT);
        }

        free(field_names[0]);
        free(field_names);
        free(field_offsets);
        free(field_types);
        free(data);

        success &= !HDF5_FAILED(H5Gclose(loc_id));

      } else {
        success = AH5_FALSE;
      }

    } else {
      success = AH5_FALSE;
    }
  }

  return success;
}


char AH5_write_usom_ef_table(
    hid_t id, const AH5_usom_ef_table_t *som, const char *path, AH5_usom_class_t type) {
  char success = AH5_TRUE;
  hid_t loc_id;
  char* basename;

  if (som && som->dims && som->dims[0] && som->dims[1]) {
    if (som->items) {
      // Non empty selector on mesh dataset
      basename = AH5_get_name_from_path(path);

      // Open / create selector on mesh node
      if (AH5_path_valid(id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH))) {
        loc_id = H5Gopen(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH), H5P_DEFAULT);
      } else {
        loc_id = H5Gcreate(
            id, AH5_CATEGORY_NAME(AH5_G_SELECTOR_ON_MESH),
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      }

      if (loc_id >= 0) {
        // Write selector on mesh dataset
        if (H5LTmake_dataset_int(loc_id, basename, 2, som->dims, som->items) < 0) {
          success = AH5_FALSE;

        } else {
          // Set pie attribute
          if (type == SOM_EDGE) {
            success &= AH5_write_str_attr(
                loc_id, basename, AH5_A_TYPE, AH5_V_EDGE);
          } else if (type == SOM_FACE) {
            success &= AH5_write_str_attr(
                loc_id, basename, AH5_A_TYPE, AH5_V_FACE);
          }
        }

        success &= !HDF5_FAILED(H5Gclose(loc_id));

      } else {
        success = AH5_FALSE;
      }

    } else {
      success = AH5_FALSE;
    }
  }

  return success;
}


char AH5_write_umesh_som_table(hid_t file_id, const AH5_usom_table_t *som, hsize_t nb_som)
{
  hsize_t i;

  for (i = 0; i < nb_som; ++i)
    if (!AH5_write_usom_table(file_id, som))
      return AH5_FALSE;

  return AH5_TRUE;
}
char AH5_write_usom_table(hid_t id, const AH5_usom_table_t *som) {
  char success = AH5_TRUE;

  switch (som->type) {
    case SOM_INVALID:
      break;

    case SOM_POINT_IN_ELEMENT:
      return AH5_write_usom_pie_table(id, &som->data.pie, som->path);

    case SOM_EDGE:
    case SOM_FACE:
      return AH5_write_usom_ef_table(id, &som->data.ef, som->path, som->type);
  }

  return success;
}

/** Write unstructured mesh */
char AH5_write_umesh(hid_t msh_id, const AH5_umesh_t *umesh)
{
  hsize_t i;
  char success = AH5_FALSE;

  // Check umesh sanity first
  if (umesh == NULL
      || umesh->nodes == NULL)
    return success;

  if (!AH5_write_str_attr(msh_id, ".", AH5_A_TYPE, AH5_V_UNSTRUCTURED))
    return success;

  // Write m x 1 dataset "elementNodes" (32-bit signed integer)
  if (!AH5_write_int_dataset(msh_id, AH5_CATEGORY_NAME(AH5_G_ELEMENT_NODES),
                             umesh->nb_elementnodes, umesh->elementnodes))
    return success;

  // Write m x 1 dataset "elementTypes" (8-bit signed char)
  if (!AH5_write_char_dataset(msh_id, AH5_CATEGORY_NAME(AH5_G_ELEMENT_TYPES),
                              umesh->nb_elementtypes, umesh->elementtypes))
    return success;

  // Write m x n dataset "nodes" (32-bit signed float)
  if (!AH5_write_flt_array(msh_id, AH5_CATEGORY_NAME(AH5_G_NODES), 2, umesh->nb_nodes, umesh->nodes))
    return success;

  // Write groups
  if (!AH5_write_ugroup(msh_id, umesh->groups, umesh->nb_groups))
    // handled error
    return success;

  // Write groupGroups
  if (!AH5_write_groupgroup(msh_id, umesh->groupgroups, umesh->nb_groupgroups))
    // handled error
    return success;

  // Write selectorOnMesh
  for (i = 0; i < umesh->nb_som_tables; ++i)
    if (!AH5_write_usom_table(msh_id, umesh->som_tables + i))
      return success;

  success = AH5_TRUE;
  return success;
}

// Write mesh instance
char AH5_write_msh_instance(hid_t loc_id, const AH5_msh_instance_t *msh_instance)
{
  char success = AH5_FALSE;
  char *basename;
  hid_t msh_id;

  basename = AH5_get_name_from_path(msh_instance->path);
  msh_id = H5Gcreate(loc_id, basename, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  if (msh_instance->type == MSH_UNSTRUCTURED)
  {
    success = AH5_write_umesh(msh_id, &msh_instance->data.unstructured);
  }
  else if (msh_instance->type == MSH_STRUCTURED)
  {
    success = AH5_write_smesh(msh_id, &msh_instance->data.structured);
  }
  else
  {
    AH5_print_err_inv_attr(AH5_C_MESH, msh_instance->path, AH5_A_TYPE);
    success = AH5_FALSE;
  }

  success &= !HDF5_FAILED(H5Gclose(msh_id));

  return success;
}

// Write link between mesh
char AH5_write_mlk_instance(hid_t UNUSED(loc_id), const AH5_mlk_instance_t *UNUSED(mlk_instance))
{
  char success = AH5_FALSE;

  AH5_PRINT_ERR_FUNC_NOT_IMPLEMENTED(AH5_C_MESH, "UNKNOWN PATH");

  return success;
}

// Write mesh group
char AH5_write_msh_group(hid_t loc_id, const AH5_msh_group_t *msh_group)
{
  char success = AH5_TRUE;
  hsize_t i;
  hid_t msh_group_id;
  char *basename;

  basename = AH5_get_name_from_path(msh_group->path);
  if (AH5_path_valid(loc_id, basename))
    msh_group_id = H5Gopen(loc_id, basename, H5P_DEFAULT);
  else
    msh_group_id = H5Gcreate(loc_id, basename, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  for (i = 0; i < msh_group->nb_msh_instances; i++)
    success &= AH5_write_msh_instance(msh_group_id, msh_group->msh_instances + i);

  for (i = 0; i < msh_group->nb_mlk_instances; i++)
    success &= AH5_write_mlk_instance(msh_group_id, msh_group->mlk_instances + i);

  success &= !HDF5_FAILED(H5Gclose(msh_group_id));

  return success;
}

// Write mesh categories
char AH5_write_mesh(hid_t file_id, const AH5_mesh_t *mesh)
{
  char success = AH5_TRUE;
  hsize_t i;
  hid_t msh_category_id;

  if (AH5_path_valid(file_id, AH5_CATEGORY_NAME(AH5_C_MESH)))
    msh_category_id = H5Gopen(file_id, AH5_CATEGORY_NAME(AH5_C_MESH), H5P_DEFAULT);
  else
    msh_category_id = H5Gcreate(file_id, AH5_CATEGORY_NAME(AH5_C_MESH), H5P_DEFAULT, H5P_DEFAULT,
                                H5P_DEFAULT);

  for (i = 0; i < mesh->nb_groups; i++)
    success &= AH5_write_msh_group(msh_category_id, mesh->groups + i);

  success &= !HDF5_FAILED(H5Gclose(msh_category_id));

  return success;
}



// Print structured mesh
void AH5_print_smesh(const AH5_smesh_t *smesh, int space)
{
  hsize_t i;
  char *ctype, *centitytype;

  AH5_print_str_attr(AH5_A_TYPE, AH5_V_STRUCTURED, space + 4);
  printf("%*s-groups: %lu\n", space + 2, "", (long unsigned) smesh->nb_groups);
  for (i = 0; i < smesh->nb_groups; i++)
  {
    printf("%*sName: %s\n", space + 5, "", AH5_get_name_from_path(smesh->groups[i].path));
    AH5_write_group_entitytype(smesh->groups[i].entitytype, &ctype, &centitytype);
    if (ctype != NULL)
      AH5_print_str_attr(AH5_A_TYPE, ctype, space + 8);
    if (centitytype != NULL)
      AH5_print_str_attr(AH5_A_ENTITY_TYPE, centitytype, space + 8);
    if (smesh->groups[i].normals != NULL)
      printf("%*s-normals: yes\n", space + 8, "");
  }
  printf("%*s-groupgroups: %lu\n", space + 2, "", (unsigned long) smesh->nb_groupgroups);
  for (i = 0; i < smesh->nb_groupgroups; i++)
    printf("%*sName: %s\n", space + 5, "", AH5_get_name_from_path(smesh->groupgroups[i].path));

  if (smesh->nb_som_tables > 0)
  {
    printf("%*s-selector on mesh: %lu\n", space + 2, "", (unsigned long) smesh->nb_som_tables);
    for (i = 0; i < smesh->nb_som_tables; i++)
    {
      printf("%*sName: %s\n", space + 5, "", AH5_get_name_from_path(smesh->som_tables[i].path));
      AH5_print_str_attr(AH5_A_TYPE, AH5_V_POINT_IN_ELEMENT, space + 9);
      printf("%*s-number of points: %lu\n", space + 7, "",
             (long unsigned) smesh->som_tables[i].nb_points);
    }
  }
}


// Print selectorOnMesh table in unstructured mesh
void AH5_print_umesh_som_table(const AH5_usom_table_t *som, int space)
{
  return AH5_print_usom_table(som, space);
}
void AH5_print_usom_table(const AH5_usom_table_t *som, int space)
{
  hsize_t k;
  char dim;

  switch (som->type)
  {
  case SOM_POINT_IN_ELEMENT:
    printf("%*sInstance: %s\n", space + 5, "", AH5_get_name_from_path(som->path));
    AH5_print_str_attr(AH5_A_TYPE, AH5_V_POINT_IN_ELEMENT, space + 9);
    for (k = 0; k < som->data.pie.nb_points; k++)
    {
      dim = som->data.pie.nb_dims;
      if (dim == 3)
        if (som->data.pie.vectors[k][2] == -1)
          dim = 2;
      if (dim == 2)
        if (som->data.pie.vectors[k][1] == -1)
          dim = 1;

      switch (dim)
      {
      case 1:
        printf("%*sPoint %lu: index=%i, v1=%f\n", space + 7, "", (long unsigned) k,
               som->data.pie.indices[k], som->data.pie.vectors[k][0]);
        break;
      case 2:
        printf("%*sPoint %lu: index=%i, v1=%f, v2=%f\n", space + 7, "", (long unsigned) k,
               som->data.pie.indices[k], som->data.pie.vectors[k][0],
               som->data.pie.vectors[k][1]);
        break;
      case 3:
        printf("%*sPoint %lu: index=%i, v1=%f, v2=%f, v3=%f\n", space + 7, "", (long unsigned) k,
               som->data.pie.indices[k], som->data.pie.vectors[k][0],
               som->data.pie.vectors[k][1], som->data.pie.vectors[k][2]);
        break;
      }
    }
    break;
  case SOM_EDGE:
    printf("%*sInstance: %s\n", space + 5, "", AH5_get_name_from_path(som->path));
    AH5_print_str_attr(AH5_A_TYPE, AH5_V_EDGE, space + 9);
    for (k = 0; k < som->data.ef.dims[0]; k++)
      printf("%*sId %lu: element=%i, inner_id=%i\n", space + 7, "", (long unsigned) k,
             som->data.ef.items[2*k], som->data.ef.items[2*k+1]);
    break;
  case SOM_FACE:
    printf("%*sInstance: %s\n", space + 5, "", AH5_get_name_from_path(som->path));
    AH5_print_str_attr(AH5_A_TYPE, AH5_V_FACE, space + 9);
    for (k = 0; k < som->data.ef.dims[0]; k++)
      printf("%*sId %lu: element=%i, inner_id=%i\n", space + 7, "", (long unsigned) k,
             som->data.ef.items[2*k], som->data.ef.items[2*k+1]);
    break;
  default:
    break;
  }
}


// Print unstructured mesh
void AH5_print_umesh(const AH5_umesh_t *umesh, int space)
{
  hsize_t i, offset = 0;
  int j, element_type;

  AH5_print_str_attr(AH5_A_TYPE, AH5_V_UNSTRUCTURED, space + 4);
  if (umesh->nb_nodes[0] > 0)
  {
    printf("%*s-nodes: %lu\n", space + 2, "", (unsigned long) umesh->nb_nodes[0]);
    for (i = 0; i < umesh->nb_nodes[0]; i++)
    {
      printf("%*sNode n %lu: ", space + 5, "", (unsigned long) i);
      for (j = 0; j < (int) umesh->nb_nodes[1]; j++)
        printf("%f ", umesh->nodes[i * umesh->nb_nodes[1] + j]);
      printf("\n");
    }
  }
  if (umesh->nb_elementtypes > 0 && umesh->nb_elementnodes > 0)
  {
    printf("%*s-elements: %lu\n", space + 2, "", (unsigned long) umesh->nb_elementtypes);
    for (i = 0; i < umesh->nb_elementtypes; i++)
    {
      printf("%*sElement %lu: type=%i", space + 5, "", (unsigned long) i, umesh->elementtypes[i]);
      element_type = umesh->elementtypes[i];
      if (element_type == 1)
        for (j = 0; j < 2; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 2)
        for (j = 0; j < 3; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 11)
        for (j = 0; j < 3; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 12)
        for (j = 0; j < 6; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 13)
        for (j = 0; j < 4; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 14)
        for (j = 0; j < 8; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 101)
        for (j = 0; j < 4; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 102)
        for (j = 0; j < 5; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 103)
        for (j = 0; j < 6; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      else if (element_type == 104)
        for (j = 0; j < 8; j++)
          printf(", N%i=%i", j, umesh->elementnodes[offset++]);
      printf("\n");
    }
  }
  printf("%*s-groups: %lu\n", space + 2, "", (unsigned long) umesh->nb_groups);
  for (i = 0; i < umesh->nb_groups; i++)
    printf("%*sName: %s\n", space + 5, "", AH5_get_name_from_path(umesh->groups[i].path));
  printf("%*s-groupgroups: %lu\n", space + 2, "", (unsigned long) umesh->nb_groupgroups);
  for (i = 0; i < umesh->nb_groupgroups; i++)
    printf("%*sName : %s\n", space + 5, "", AH5_get_name_from_path(umesh->groupgroups[i].path));

  if (umesh->nb_som_tables > 0)
  {
    printf("%*s-selector on mesh: %lu\n", space + 2, "", (unsigned long) umesh->nb_som_tables);
    for (i = 0; i < umesh->nb_som_tables; i++)
      AH5_print_usom_table(&(umesh->som_tables[i]), space);
  }
}


// Print mesh instance
void AH5_print_msh_instance(const AH5_msh_instance_t *msh_instance, int space)
{
  printf("%*sInstance: %s\n", space, "", AH5_get_name_from_path(msh_instance->path));
  switch (msh_instance->type)
  {
  case MSH_STRUCTURED:
    AH5_print_smesh(&(msh_instance->data.structured), space);
    break;
  case MSH_UNSTRUCTURED:
    AH5_print_umesh(&(msh_instance->data.unstructured), space);
    break;
  default:
    break;
  }
}


// Print meshLink instance
void AH5_print_mlk_instance(const AH5_mlk_instance_t *mlk_instance, int space)
{
  printf("%*sMeshLink instance: %s\n", space, "", AH5_get_name_from_path(mlk_instance->path));
  AH5_print_str_attr(AH5_A_MESH1, mlk_instance->mesh1, space + 3);
  AH5_print_str_attr(AH5_A_MESH2, mlk_instance->mesh2, space + 3);
}


// Print mesh group
void AH5_print_msh_group(const AH5_msh_group_t *msh_group, int space)
{
  hsize_t i;

  printf("%*sGroup: %s\n", space, "", AH5_get_name_from_path(msh_group->path));
  for (i = 0; i < msh_group->nb_msh_instances; i++)
    AH5_print_msh_instance(&(msh_group->msh_instances[i]), space + 2);
  for (i = 0; i < msh_group->nb_mlk_instances; i++)
    AH5_print_mlk_instance(&(msh_group->mlk_instances[i]), space + 2);
  printf("\n");
}


// Print mesh category
void AH5_print_mesh(const AH5_mesh_t *mesh)
{
  hsize_t i;

  printf("###################################  Mesh  ###################################\n\n");
  for (i = 0; i < mesh->nb_groups; i++)
    AH5_print_msh_group(&(mesh->groups[i]), 0);
  printf("\n");
}




// Free memory used by groupGgroup
void AH5_free_groupgroup(AH5_groupgroup_t *groupgroup)
{
  if (groupgroup)
  {
    if (groupgroup->path)
    {
      free(groupgroup->path);
      groupgroup->path = NULL;
    }

    if (groupgroup->groupgroupnames)
    {
      if (*groupgroup->groupgroupnames)
      {
        free(*groupgroup->groupgroupnames);
        *groupgroup->groupgroupnames = NULL;
      }

      free(groupgroup->groupgroupnames);
      groupgroup->groupgroupnames = NULL;
    }

    groupgroup->nb_groupgroupnames = 0;
  }
}


void AH5_free_ssom_pie_table(AH5_ssom_pie_table_t *som) {
  if (som) {
    if (som->path) {
      free(som->path);
      som->path = NULL;
    }

    if (som->elements) {
      if (som->elements[0])
        free(som->elements[0]);

      free(som->elements);
      som->elements = NULL;
    }

    if (som->vectors) {
      if (som->vectors[0])
        free(som->vectors[0]);

      free(som->vectors);
      som->vectors = NULL;
    }

    som->nb_dims = 0;
    som->nb_points = 0;
  }
}


void AH5_free_usom_table(AH5_usom_table_t *som) {
  if (som) {
    if (som->path) {
      free(som->path);
      som->path = NULL;
    }

    switch (som->type) {
      case SOM_INVALID:
        break;

      case SOM_POINT_IN_ELEMENT:
        AH5_free_usom_pie_table(&som->data.pie);
        break;

      case SOM_EDGE:
      case SOM_FACE:
        AH5_free_usom_ef_table(&som->data.ef);
        break;
    }
  }

  som->type = SOM_INVALID;
}


void AH5_free_usom_pie_table(AH5_usom_pie_table_t *som) {
  if (som) {
    if (som->indices) {
      free(som->indices);
      som->indices = NULL;
    }

    if (som->vectors) {
      if (som->vectors[0])
        free(som->vectors[0]);

      free(som->vectors);
      som->vectors = NULL;
    }

    som->nb_dims = 0;
    som->nb_points = 0;
  }
}


void AH5_free_usom_ef_table(AH5_usom_ef_table_t *som) {
  if (som) {
    if (som->items) {
      free(som->items);
      som->items = NULL;
    }

    som->dims[0] = 0;
    som->dims[1] = 0;
  }
}


// Free memory used by sgroup
void AH5_free_sgroup(AH5_sgroup_t *sgroup) {
  if (sgroup)
  {
    if (sgroup->path)
    {
      free(sgroup->path);
      sgroup->path = NULL;
    }

    if (sgroup->elements)
    {
      free(sgroup->elements);
      sgroup->elements = NULL;
    }

    if (sgroup->normals)
    {
      if ((*sgroup->normals != NULL) && (sgroup->flat_normals == NULL))
      {
        free(*sgroup->normals);
        *sgroup->normals = NULL;
      }

      free(sgroup->normals);
      sgroup->normals = NULL;
    }
    if (sgroup->flat_normals) {
      free(sgroup->flat_normals);
      sgroup->flat_normals = NULL;
    }

    sgroup->entitytype = AH5_GROUP_ENTITYTYPE_UNDEF;
    sgroup->dims[0] = 0;
    sgroup->dims[1] = 0;
  }
}


// Free memory used by structured mesh
void AH5_free_smesh(AH5_smesh_t *smesh)
{
  hsize_t i;

  if (smesh->x.nodes != NULL)
  {
    free(smesh->x.nodes);
    smesh->x.nodes = NULL;
    smesh->x.nb_nodes = 0;
  }
  if (smesh->y.nodes != NULL)
  {
    free(smesh->y.nodes);
    smesh->y.nodes = NULL;
    smesh->y.nb_nodes = 0;
  }
  if (smesh->z.nodes != NULL)
  {
    free(smesh->z.nodes);
    smesh->z.nodes = NULL;
    smesh->z.nb_nodes = 0;
  }

  if (smesh->groups != NULL)
  {
    for (i = 0; i < smesh->nb_groups; ++i)
      AH5_free_sgroup(smesh->groups + i);

    free(smesh->groups);
    smesh->groups = NULL;
    smesh->nb_groups = 0;
  }

  if (smesh->groupgroups != NULL)  // if any groupGroups...
  {
    for (i = 0; i < smesh->nb_groupgroups; i++)    // for each groupGroup...
      AH5_free_groupgroup(smesh->groupgroups + i);  // free AH5_groupgroup_t structures
    free(smesh->groupgroups);  // free space for pointers to groupGroups
    smesh->groupgroups = NULL;
    smesh->nb_groupgroups = 0;
  }

  if (smesh->som_tables != NULL) {
    for (i = 0; i < smesh->nb_som_tables; ++i)
      AH5_free_ssom_pie_table(smesh->som_tables + i);
    free(smesh->som_tables);
    smesh->som_tables = NULL;
    smesh->nb_som_tables = 0;
  }
}


// Free memory used by unstructured mesh
void AH5_free_umesh(AH5_umesh_t *umesh)
{
  hsize_t i;

  if (umesh->elementnodes != NULL)  // if any elementnodes...
  {
    free(umesh->elementnodes);
    umesh->elementnodes = NULL;
    umesh->nb_elementnodes = 0;
  }

  if (umesh->elementtypes != NULL)  // if any elementtypes...
  {
    free(umesh->elementtypes);
    umesh->elementtypes = NULL;
    umesh->nb_elementtypes = 0;
  }

  if (umesh->nodes != NULL)  // if any nodes...
  {
    free(umesh->nodes);
    umesh->nodes = NULL;
    umesh->nb_nodes[0] = 0;
    umesh->nb_nodes[1] = 0;
  }

  if (umesh->groups != NULL)  // if any groups...
  {
    for (i = 0; i < umesh->nb_groups; i++)    // for each group...
    {
      free(umesh->groups[i].path);  // free group name
      free(umesh->groups[i].groupelts);  // free group values (no need to assign NULL & set nb_groupelts to 0
    }
    free(umesh->groups);  // free space for pointers to groups
    umesh->groups = NULL;
    umesh->nb_groups = 0;
  }

  if (umesh->groupgroups != NULL)  // if any groupGroups...
  {
    for (i = 0; i < umesh->nb_groupgroups; i++)    // for each groupGroup...
      AH5_free_groupgroup(umesh->groupgroups + i);  // free AH5_groupgroup_t structures
    free(umesh->groupgroups);  // free space for pointers to groupGroups
    umesh->groupgroups = NULL;
    umesh->nb_groupgroups = 0;
  }

  if (umesh->som_tables != NULL) {
    for (i = 0; i < umesh->nb_som_tables; ++i)
      AH5_free_usom_table(umesh->som_tables + i);
    free(umesh->som_tables);
    umesh->som_tables = NULL;
    umesh->nb_som_tables = 0;
  }
}


// Free memory used by mesh instance
void AH5_free_msh_instance(AH5_msh_instance_t *msh_instance)
{
  if (msh_instance->path != NULL)
  {
    free(msh_instance->path);
    msh_instance->path = NULL;
  }

  switch (msh_instance->type)
  {
  case MSH_STRUCTURED:
    AH5_free_smesh(&(msh_instance->data.structured));
    break;
  case MSH_UNSTRUCTURED:
    AH5_free_umesh(&(msh_instance->data.unstructured));
    break;
  default:
    break;
  }
  msh_instance->type = MSH_INVALID;
}


void AH5_free_mlk_instance(AH5_mlk_instance_t *mlk_instance)
{
  if (mlk_instance->path != NULL)
  {
    free(mlk_instance->path);
    mlk_instance->path = NULL;
  }
  mlk_instance->type = MSHLNK_INVALID;
  if (mlk_instance->mesh1 != NULL)
  {
    free(mlk_instance->mesh1);
    mlk_instance->mesh1 = NULL;
  }
  if (mlk_instance->mesh2 != NULL)
  {
    free(mlk_instance->mesh2);
    mlk_instance->mesh2 = NULL;
  }
  if (mlk_instance->data != NULL)
  {
    free(mlk_instance->data);
    mlk_instance->data = NULL;
    mlk_instance->dims[0] = 0;
    mlk_instance->dims[1] = 0;
  }
}


// Free memory used by mesh group
void AH5_free_msh_group(AH5_msh_group_t *msh_group)
{
  hsize_t i;

  if (msh_group->path != NULL)
    free(msh_group->path);

  if (msh_group->msh_instances != NULL)
  {
    for (i = 0; i < msh_group->nb_msh_instances; i++)
      AH5_free_msh_instance(msh_group->msh_instances + i);
    free(msh_group->msh_instances);
  }
  if (msh_group->mlk_instances != NULL)
  {
    for (i = 0; i < msh_group->nb_mlk_instances; i++)
      AH5_free_mlk_instance(msh_group->mlk_instances + i);
    free(msh_group->mlk_instances);
  }
  AH5_init_msh_group(msh_group, NULL, 0, 0);
}


// Free memory used by mesh category
void AH5_free_mesh(AH5_mesh_t *mesh)
{
  hsize_t i;

  if (mesh->groups != NULL)
  {
    for (i = 0; i < mesh->nb_groups; i++)
      AH5_free_msh_group(mesh->groups + i);
    free(mesh->groups);
  }
  AH5_init_mesh(mesh, 0);
}


// Copy a mesh's groupgroup
AH5_groupgroup_t * AH5_copy_groupgroup(AH5_groupgroup_t *dest, const AH5_groupgroup_t *src)
{
  if (src && dest)
  {
    AH5_init_groupgroup(dest, src->path, src->nb_groupgroupnames, strlen(src->groupgroupnames[0]));
    AH5_COPY_STRING_ARRAY_FIELD(dest, src, groupgroupnames);
  }

  return dest;
}


// Copy an unstructured group
AH5_ugroup_t * AH5_copy_ugroup(AH5_ugroup_t *dest, const AH5_ugroup_t *src)
{
  if (src && dest)
  {
    if (!AH5_init_ugroup(
            dest, src->path, src->nb_groupelts, src->entitytype))
      return NULL;

    AH5_COPY_ARRAY_FIELD(dest, src, groupelts);
  }
  return dest;
}


// Copy an unstructured mesh
AH5_umesh_t * AH5_copy_umesh(AH5_umesh_t *dest, const AH5_umesh_t *src)
{
  unsigned i;

  if (src && dest)
  {
    AH5_init_umesh(
        dest, src->nb_elementnodes, src->nb_elementtypes, src->nb_nodes[AH5_UMESH_NODES_SIZE],
        src->nb_groups, src->nb_groupgroups, src->nb_som_tables);

    AH5_COPY_ARRAY_FIELD(dest, src, elementnodes);
    AH5_COPY_ARRAY_FIELD(dest, src, elementtypes);
    memcpy(dest->nodes, src->nodes,
           src->nb_nodes[0] * src->nb_nodes[1] * sizeof(*src->nodes));

    for (i = 0; i < src->nb_groups; ++i)
      AH5_copy_ugroup(dest->groups + i, src->groups + i);
    for (i = 0; i < src->nb_groupgroups; ++i)
      AH5_copy_groupgroup(dest->groupgroups + i, src->groupgroups + i);
    // TODO(nmt) Copy selector on mesh
  }
  return dest;
}


// Return the nodes number associated to the given element type or 0.
int AH5_element_size(char element_type)
{
  int size = 0;

  switch (element_type)
  {
    case AH5_UELE_NODE:
      size = 1;
      break;
    case AH5_UELE_BAR2:
      size = 2;
      break;

    case AH5_UELE_BAR3:
    case AH5_UELE_TRI3:
      size = 3;
      break;

    case AH5_UELE_QUAD4:
    case AH5_UELE_TETRA4:
      size = 4;
      break;

    case AH5_UELE_PYRA5:
      size = 5;
      break;

    case AH5_UELE_TRI6:
    case AH5_UELE_PENTA6:
      size = 6;
      break;

    case AH5_UELE_QUAD8:
    case AH5_UELE_HEXA8:
      size = 8;
      break;

    case AH5_UELE_QUAD9:
      size = 9;
      break;

    case AH5_UELE_TETRA10:
      size = 10;
      break;

    case AH5_UELE_HEXA20:
      size = 20;
      break;

    default:
      size = 0;
  }

  return size;
}
