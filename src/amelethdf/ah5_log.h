/**
 * @file   ah5_log.h
 * @author Nathanaël MUOT <nathanael.muot@axessim.fr>
 * @date   Thu Oct  1 11:50:40 2015
 *
 * @brief
 *
 *
 */

#include "ah5_general.h"

#ifndef _AH5_LOG_H_
#define _AH5_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  eAH5_DEBUG,
  eAH5_INFO,
  eAH5_WARN,
  eAH5_ERROR,
  eAH5_NB_LOG_LEVEL
}
AH5_log_t;

/**
 * Set the max log level.
 *
 * After calling this function only the logging messages greatest or equal
 * than the given level are display in logging output stream.
 *
 * @param level
 */
AH5_PUBLIC void AH5_set_log_level(AH5_log_t level);

/**
 * Return the lowest logging level.
 *
 * @return
 */
AH5_PUBLIC AH5_log_t AH5_get_log_level();

/**
 * Write a log message.
 *
 * @param level message level
 * @param format see printf
 * @param ... (additional arguments)
 */
AH5_PUBLIC void AH5_log(AH5_log_t level, const char *format, ...);

/**
 * Write a debug message.
 */
AH5_PUBLIC void AH5_log_debug(const char *format, ...);

/**
 * Write a information message.
 */
AH5_PUBLIC void AH5_log_info(const char *format, ...);

/**
 * Write a warning message.
 */
AH5_PUBLIC void AH5_log_warn(const char *format, ...);

/**
 * Write a error message.
 */
AH5_PUBLIC void AH5_log_error(const char *format, ...);

/**
 * Set the log call to 0
 *
 */
AH5_PUBLIC void AH5_erase_log_count();

/**
 * Return the number of logging call greatest or equal than the given level.
 *
 * @param level
 *
 * @return
 *
 * @code
 * int nb_errors = AH5_init_count_log(eAH5_WARN);
 * @endcode
 */
AH5_PUBLIC int AH5_count_log(AH5_log_t level);

/**
 * check if an error is produced
 *
 * @return
 */
AH5_PUBLIC int AH5_check();


AH5_PUBLIC void AH5_print_err_dset(const char *category, const char *path);
AH5_PUBLIC void AH5_print_err_tble(const char *category, const char *path);
AH5_PUBLIC void AH5_print_err_attr(const char *category, const char *path, const char *attr_name);
AH5_PUBLIC void AH5_print_err_path(const char *category, const char *path);
AH5_PUBLIC void AH5_print_err_inv_attr(const char *category, const char *path,
                                       const char *attr_name);
AH5_PUBLIC void AH5_print_err_func_not_implemented(const char *category, const char *path,
    const char *func_name);
AH5_PUBLIC void AH5_print_wrn_attr(const char *category, const char *path, const char *attr_name);
AH5_PUBLIC void AH5_print_wrn_outputs(const char *path);


// Some helpers macros of manage error.
#define AH5_PRINT_ERR_FUNC_NOT_IMPLEMENTED(category, path)              \
  AH5_print_err_func_not_implemented(category, path, (__FUNCTION__))

#ifdef __cplusplus
}
#endif

#endif /* _AH5_LOG_H_ */
