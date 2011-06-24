#include "ah5_globenv.h"


// Read globalEnvironment instance
char AH5_read_global_environment_instance (hid_t file_id, const char *path, AH5_gle_instance_t *gle_instance)
{
    char path2[AH5_ABSOLUTE_PATH_LENGTH], path3[AH5_ABSOLUTE_PATH_LENGTH], rdata = TRUE;
    char mandatory[][AH5_ATTR_LENGTH] = {};

    gle_instance->path = strdup(path);
    gle_instance->type = GE_INVALID;
    gle_instance->limit_conditions.instances = NULL;
    gle_instance->data.type = FT_INVALID;

    if (AH5_path_valid(file_id, path))
    {
        strcpy(path2, path);
        strcat(path2, AH5_G_LIMIT_CONDITIONS);
        AH5_read_opt_attrs(file_id, path2, &(gle_instance->limit_conditions), mandatory, sizeof(mandatory)/AH5_ATTR_LENGTH);

        strcpy(path2, path);
        strcpy(path3, path);
        strcat(path2, AH5_G_FREQUENCY);
        strcat(path3, AH5_G_TIME);
        if (H5Lexists(file_id, path2, H5P_DEFAULT) == TRUE && H5Lexists(file_id, path3, H5P_DEFAULT) != TRUE)  // only frequency...
        {
            gle_instance->type = GE_FREQUENCY;
            if (!AH5_read_floatingtype(file_id, path2, &(gle_instance->data)))
                rdata = FALSE;
        }
        else if (H5Lexists(file_id, path3, H5P_DEFAULT) == TRUE && H5Lexists(file_id, path2, H5P_DEFAULT) != TRUE)  // only time...
        {
            gle_instance->type = GE_TIME;
            if (!AH5_read_floatingtype(file_id, path3, &(gle_instance->data)))
                rdata = FALSE;
        }
        else
            rdata = FALSE;
    }
    else
    {
        AH5_print_err_path(AH5_C_GLOBAL_ENVIRONMENT, path);
        rdata = FALSE;
    }
    return rdata;
}


// Read globalEnvironment category
char AH5_read_global_environment (hid_t file_id, AH5_global_environment_t *global_environment)
{
    char path[AH5_ABSOLUTE_PATH_LENGTH], rdata = TRUE;
    AH5_children_t children;
    hsize_t i;

    global_environment->instances = NULL;

    if (H5Lexists(file_id, AH5_C_GLOBAL_ENVIRONMENT, H5P_DEFAULT) == TRUE)
    {
        children = AH5_read_children_name(file_id, AH5_C_GLOBAL_ENVIRONMENT);
        global_environment->nb_instances = children.nb_children;
        if (children.nb_children > 0)
        {
            global_environment->instances = (AH5_gle_instance_t *) malloc(children.nb_children * sizeof(AH5_gle_instance_t));
            for (i = 0; i < children.nb_children; i++)
            {
                strcpy(path, AH5_C_GLOBAL_ENVIRONMENT);
                strcat(path, children.childnames[i]);
                if (!AH5_read_global_environment_instance(file_id, path, global_environment->instances + i))
                    rdata = FALSE;
                free(children.childnames[i]);
            }
            free(children.childnames);
        }
    }
    else
    {
        AH5_print_err_path(AH5_C_GLOBAL_ENVIRONMENT, AH5_C_GLOBAL_ENVIRONMENT);
        rdata = FALSE;
    }
    return rdata;
}




// Print globalEnvironment instance
void AH5_print_global_environment_instance (const AH5_gle_instance_t *gle_instance, int space)
{
    printf("%*sInstance: %s\n", space, "", AH5_get_name_from_path(gle_instance->path));
    AH5_print_floatingtype(&(gle_instance->data), space + 3);
    AH5_print_opt_attrs(&(gle_instance->limit_conditions), space + 6);
}


// Print globalEnvironment category
void AH5_print_global_environment (const AH5_global_environment_t *global_environment)
{
    hsize_t i;

    printf("############################  Global environment  ############################\n\n");
    for (i = 0; i < global_environment->nb_instances; i++)
        AH5_print_global_environment_instance(&(global_environment->instances[i]), 0);
    printf("\n");
}




// Free memory used by globalEnvironment instance
void AH5_free_global_environment_instance (AH5_gle_instance_t *gle_instance)
{
    if (gle_instance->path != NULL)
    {
        free(gle_instance->path);
        gle_instance->path = NULL;
    }
    if (gle_instance->type != GE_INVALID)
    {
        AH5_free_floatingtype(&(gle_instance->data));
        gle_instance->type = GE_INVALID;
    }
    AH5_free_opt_attrs(&(gle_instance->limit_conditions));
}


// Free memory used by globalEnvironment category
void AH5_free_global_environment (AH5_global_environment_t *global_environment)
{
    hsize_t i;

    if (global_environment->instances != NULL)
    {
        for (i = 0; i < global_environment->nb_instances; i++)
            AH5_free_global_environment_instance(global_environment->instances + i);
        free(global_environment->instances);
        global_environment->instances = NULL;
        global_environment->nb_instances = 0;
    }
}
