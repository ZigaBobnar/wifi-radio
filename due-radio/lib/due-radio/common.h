#ifndef COMMON_H_
#define COMMON_H_

#ifndef FAKE_HARDWARE
#define FAKE_HARDWARE 0
#endif

#if FAKE_HARDWARE
#define REAL_HARDWARE 0
#else
#define REAL_HARDWARE 1
#endif

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#if REAL_HARDWARE
#include <asf.h>
#endif

#if FAKE_HARDWARE
#include <stdio.h>
#endif

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
