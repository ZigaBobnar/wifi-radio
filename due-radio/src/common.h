#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// TODO: Remove direct dependency to ASF
#include "asf.h"

#include "config/config.h"

inline void debug_assert(char* value) {}
inline void debug_crash_shutdown() {}

#ifdef __cplusplus
#define __EXTERN_C_BEGIN extern "C" {
#define __EXTERN_C_END }
#else
#define __EXTERN_C_BEGIN
#define __EXTERN_C_END
#endif

#endif  // COMMON_H_
