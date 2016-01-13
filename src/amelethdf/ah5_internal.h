/**
 * @file   ah5_internal.h
 * @author Nathanaël MUOT <nathanael.muot@axessim.fr>
 * @date   Tue Jan 12 10:18:18 2016
 * 
 * @brief  
 * 
 * 
 */

#if !defined(AMELETHDF_C_LIBRARY)
#error "Try to import ah5 C library!"
#endif


/**
 * @macro AH5_COPY_ARRAY_FIELD(dest, src, field_name)
 * Define a common way for copy AH5 structure field.
 *
 * @macro AH5_COPY_STRING_ARRAY_FIELD(dest, src, field_name)
 * Define a common way for copy AH5 structure field.
 */

#define AH5_FIELD_NAME(value) value
#define AH5_FIELD_SIZE(value) nb_ ## value

#define AH5_COPY_ARRAY_FIELD(dest, src, field_name)                     \
  memcpy(dest->AH5_FIELD_NAME(field_name), src->AH5_FIELD_NAME(field_name), \
         src->AH5_FIELD_SIZE(field_name) * sizeof(*src->AH5_FIELD_NAME(field_name)));

#define AH5_COPY_STRING_ARRAY_FIELD(dest, src, field_name)              \
  memcpy(*dest->AH5_FIELD_NAME(field_name), *src->AH5_FIELD_NAME(field_name), \
         sizeof(**src->AH5_FIELD_NAME(field_name)));
