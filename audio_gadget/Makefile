# Makefile template for compiling audio_gadget.c
ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE = /opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-
export LD_LIBRARY_PATH=/opt/gcc-ubuntu-9.3.0-2020.03-x86_64-aarch64-linux-gnu/lib/x86_64-linux-gnu
endif
# Compiler and flags
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -O2

# Source file and executable name
SRC = audio_gadget.c
EXEC = audio_gadget
export SYSROOT_DIR=$(realpath ../../../deploy)/rootfs
LIB_DIR = -L../lib/ -L../lib/hbmedia -L../lib/aarch64-linux-gnu  -Wl,-rpath-link=../lib/ -L./
INCLUDE_DIR = -I../include/iar/ -I../include/vio/
LIB_DIR += -L${SYSROOT_DIR}/usr/lib/aarch64-linux-gnu --sysroot=${SYSROOT_DIR}
# Libraries to link
LIBS = -lasound

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDE_DIR) $(LIB_DIR) $(LIBS) 

clean:
	rm -f $(EXEC)

.PHONY: all clean
