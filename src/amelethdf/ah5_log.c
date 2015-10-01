/**
 * @file   ah5_log.c
 * @author Nathanaël MUOT <nathanael.muot@axessim.fr>
 * @date   Thu Oct  1 11:51:27 2015
 *
 * @brief
 *
 *
 */


#include "ah5_log.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
AH5_log_t g_ah5_log_level = eAH5_DEBUG;
#else
AH5_log_t g_ah5_log_level = eAH5_WARN;
#endif

int g_ah5_log_count[eAH5_NB_LOG_LEVEL + 1];

void AH5_set_log_level(AH5_log_t level)
{
  g_ah5_log_level = level;
}


AH5_log_t AH5_get_log_level()
{
  AH5_log_t log_level = g_ah5_log_level;
  char *log_level_str = getenv("AH5_LOG_LEVEL");

  if (log_level_str)
    log_level = (AH5_log_t)atoi(log_level_str);

  return log_level;
}


void _AH5_log(AH5_log_t level, const char *format, va_list args)
{
  if (level >= AH5_get_log_level())
  {
    printf("[AH5] ***** ");

    switch (level)
    {
    case eAH5_DEBUG:
      printf("{debug} ");
      break;
    case eAH5_INFO:
      printf("{info}  ");
      break;
    case eAH5_WARN:
      printf("{warn}  ");
      break;
    case eAH5_ERROR:
      printf("{error} ");
      break;
    default:
      printf("{%3d}   ", level);
      break;
    }

    if (level > eAH5_NB_LOG_LEVEL)
      level = eAH5_NB_LOG_LEVEL;
    ++g_ah5_log_count[level];

    vprintf(format, args);
    printf(" *****\n");
  }
}


void AH5_log(AH5_log_t level, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _AH5_log(level, format, args);
  va_end(args);
}


void AH5_log_debug(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _AH5_log(eAH5_DEBUG, format, args);
  va_end(args);
}


void AH5_log_info(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _AH5_log(eAH5_INFO, format, args);
  va_end(args);
}


void AH5_log_warn(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _AH5_log(eAH5_WARN, format, args);
  va_end(args);
}


void AH5_log_error(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  _AH5_log(eAH5_ERROR, format, args);
  va_end(args);
}


void AH5_erase_log_count()
{
  int level;

  for (level = 0; level <= eAH5_NB_LOG_LEVEL; ++level)
    g_ah5_log_count[level] = 0;
}


int AH5_count_log(AH5_log_t level)
{
  int count = 0;
  int i;

  for (i = level; i <= eAH5_NB_LOG_LEVEL; ++i)
    count += g_ah5_log_count[i];

  return count;
}


int AH5_check()
{
  return AH5_count_log(eAH5_WARN) == 0;
}
