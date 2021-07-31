#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "due-radio/config/config.h"

void debug_assert(char* value);
void debug_crash_shutdown(void);

#ifdef __cplusplus
#define __EXTERN_C_BEGIN extern "C" {
#define __EXTERN_C_END }
#else
#define __EXTERN_C_BEGIN
#define __EXTERN_C_END
#endif

#endif  // COMMON_H_
