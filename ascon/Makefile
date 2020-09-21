# Copyright lowRISC contributors.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0
#
# Generate a baremetal application

PROGRAM_CFLAGS = -Wall -g -O3 -ffunction-sections -Iinclude -I../include
ifeq ($(SIM),1)
  PROGRAM_CFLAGS += -DSIMULATION
endif
ROM_FILE ?= ../../sv/CustomRom_8192X32.sv

MULTIROUND_CONF ?= 1
LITTLE_ENDIAN_CONF ?= 1
ASCON_INSTR ?= 1
PERFORMANCE_TEST ?= 0
RISCY_HIGH_PERFORMANCE ?= 0
TARGET_CFLAG ?= -DRISCY
ASCON_CFLAGS ?= -DMULTIROUND_CONF=$(MULTIROUND_CONF) -DLITTLE_ENDIAN_CONF=$(LITTLE_ENDIAN_CONF) -DASCON_INSTR=$(ASCON_INSTR) -DPERFORMANCE_TEST=$(PERFORMANCE_TEST) -DRISCY_HIGH_PERFORMANCE=$(RISCY_HIGH_PERFORMANCE)

ARCH = rv32im
MAINFILE ?= main.c
RESULTFILE := ${MAINFILE:.c=}
SRCS = ../common/util.c ascon_decrypt.c isap.c ascon_encrypt_asm_be.c ascon_encrypt_asm_le.c ascon_encrypt_c_hw.c ascon_encrypt_c_sw.c ascon_hash_asm_be.c ascon_hash_asm_le.c ascon_hash_c_hw.c ascon_hash_c_sw.c $(MAINFILE)
SINGLEROMSIZE = 8192

CC = riscv32-unknown-elf-gcc

OBJCOPY ?= $(subst gcc,objcopy,$(wordlist 1,1,$(CC)))
OBJDUMP ?= $(subst gcc,objdump,$(wordlist 1,1,$(CC)))

LINKER_SCRIPT ?= ../link.ld
CRT ?= ../crt0.S
CFLAGS_SPIKE ?= -march=$(ARCH) $(PROGRAM_CFLAGS) $(ASCON_CFLAGS)
CFLAGS_HW ?= -march=$(ARCH) -mabi=ilp32 -static -mcmodel=medany \
	-fvisibility=hidden -nostdlib -nostartfiles $(PROGRAM_CFLAGS) $(TARGET_CFLAG) $(ASCON_CFLAGS)

OBJS := ${SRCS:.c=.o}
OBJS_HW := ${OBJS} ${CRT:.S=.o}
DEPS = $(OBJS_HW:%.o=%.d)

RESULTFILES = $(PROGRAM).vmem $(PROGRAM).bin $(PROGRAM).dis

.PHONY: all
all: spike $(RESULTFILE).elf $(RESULTFILE).vmem $(RESULTFILE).bin $(RESULTFILE).dis

spike: $(RESULTFILE)_spike.elf $(RESULTFILE)_spike.dis

$(RESULTFILE)_spike.elf:
	$(CC) -DSPIKE $(CFLAGS_SPIKE) $(SRCS) -o $@

$(RESULTFILE).elf: $(OBJS_HW) $(LINKER_SCRIPT)
	#-ffunction-sections -Wl,-Map,linkerTarget.map
	$(CC) $(CFLAGS_HW) -T $(LINKER_SCRIPT) $(OBJS_HW) -o $@

%.dis: %.elf
	$(OBJDUMP) -SD $^ > $@

 # Note: this target requires the srecord package to be installed.
# XXX: This could be replaced by objcopy once
# https://sourceware.org/bugzilla/show_bug.cgi?id=19921
# is merged.
# XXX: Currently the start address 0x10000000 is hardcoded. It could/should be
# read from the elf file, but is lost in the bin file.
# Switching to objcopy will resolve that as well.
%.vmem: %.bin
	srec_cat $^ -binary -offset 0x0000 -byte-swap 4 -o $@ -Intel
	python3 ../convert.py $@ $(ROM_FILE) | split -d --additional-suffix=.patt -l $(SINGLEROMSIZE) - ../rom-

%.bin: %.elf
	$(OBJCOPY) -O binary $^ $@

%.o: %.c
	$(CC) $(CFLAGS_HW) -MMD -c $(INCS) -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS_HW) -MMD -c $(INCS) -o $@ $<

.PHONY: clean
clean:
	$(RM) -f *.o *.d *.elf

.PHONY: distclean
distclean: clean
	$(RM) -f *.vmem *.bin *.dis
