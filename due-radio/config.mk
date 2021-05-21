#
# Copyright (c) 2011 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = .

# Target CPU architecture: cortex-m3, cortex-m4
ARCH = cortex-m3

# Target part: none, sam3n4 or sam4l4aa
PART = sam3x8e

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.

# PROJ is defined in Makefile (modified by Brane)
TARGET_FLASH = $(PN).elf
TARGET_SRAM = $(PN).elf

# List of C source files.
CSRCS = \
	$(wildcard app/src/*.c)										\
	asf/sam/utils/cmsis/sam3x/source/templates/system_sam3x.c		\
	asf/sam/utils/cmsis/sam3x/source/templates/exceptions.c			\
	asf/sam/utils/cmsis/sam3x/source/templates/gcc/startup_sam3x.c	\
	asf/common/services/clock/sam3x/sysclk.c						\
	asf/common/services/delay/sam/cycle_counter.c					\
	asf/sam/utils/syscalls/gcc/syscalls.c                          	\
	asf/sam/drivers/pmc/pmc.c										\
	asf/sam/drivers/pio/pio.c										\
	asf/sam/drivers/uart/uart.c										\
	asf/sam/drivers/usart/usart.c									\
	asf/sam/drivers/dacc/dacc.c									    \
	asf/sam/drivers/tc/tc.c									    	\


# List of assembler source files.
ASSRCS =

# List of include paths.
INC_PATH = \
	app/include								\
	asf/sam/boards								\
	asf/sam/utils								\
	asf/sam/utils/header_files					\
	asf/sam/utils/preprocessor					\
	asf/sam/utils/cmsis/sam3x/include			\
	asf/sam/utils/cmsis/sam3x/source/templates	\
	asf/common/boards							\
	asf/common/utils							\
	asf/thirdparty/CMSIS/include				\
	asf/common/services/gpio					\
	asf/common/services/clock					\
	asf/common/services/delay					\
	asf/common/services/ioport					\
	asf/sam/drivers/pmc							\
	asf/sam/drivers/pio							\
	asf/sam/drivers/uart					    \
	asf/sam/drivers/usart					    \
	asf/sam/drivers/dacc					    \
	asf/sam/drivers/tc						    \



# Additional search paths for libraries.
LIB_PATH =  \

# List of libraries to use during linking.
LIBS =  \

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT_FLASH = asf/sam/utils/linker_scripts/sam3x/sam3x8/gcc/flash.ld
LINKER_SCRIPT_SRAM  = asf/sam/utils/linker_scripts/sam3x/sam3x8/gcc/sram.ld

# Path relative to top level directory pointing to a linker script.
DEBUG_SCRIPT_FLASH = asf/sam/boards/arduino_due_x/debug_scripts/gcc/arduino_due_x_flash.gdb
DEBUG_SCRIPT_SRAM  = asf/sam/boards/arduino_due_x/debug_scripts/gcc/arduino_due_x_sram.gdb

# Project type parameter: all, sram or flash
PROJECT_TYPE = flash

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O0

# Extra flags to use when archiving.
ARFLAGS =

# Extra flags to use when assembling.
ASFLAGS = -g

# Extra flags to use when compiling.
CFLAGS =

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D ARM_MATH_CM3=true                               \
       -D BOARD=ARDUINO_DUE_X                             \
       -D __SAM3X8E__                                     \
       -D printf=iprintf                                  \
       -D scanf=iscanf

# Extra flags to use when linking
LDFLAGS = \

# Pre- and post-build commands
PREBUILD_CMD =
POSTBUILD_CMD =
