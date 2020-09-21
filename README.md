This repo is part of the [Ascon-p Accelerator Meta Repo](https://github.com/Steinegger/riscv_asconp_accelerator) and holds the RISC-V proof-of-concept assembly implementation utilizing the Ascon-p instruction for Ascon, Ascon-Hash as well as the ISAP-rk function.  
The code can be run on the (enhanced cv32e40p RISC-V processor)[https://github.com/Steinegger/riscv_asconp_accelerator_core] or the [functional isa-sim model](https://github.com/Steinegger/riscv_asconp_accelerator_isa_sim).  

# Structure

This repo includes several different implemenations of Ascon and Ascon Hash that were used as a proof of concept and as a benchmarking plattform:  
**Ascon Encrypt:**
 + ascon/ascon_encrypt_asm_be.c: Assembly big-endian implementation of Ascon using the Ascon-p instruction.
 + ascon/ascon_encrypt_asm_le.c: Assembly little-endian implementation of Ascon using the Ascon-p instruction with the hardware instruction performing the endianess swap.
 + ascon/ascon_encrypt_c_hw.c: Reference C-implementation with the Ascon-p round function swapped for the hardware round function.
 + ascon/ascon_encrypt_c_sw.c Reference C-implementation for comparision.

**Ascon Hash:**
 + ascon/ascon_hash_asm_be.c: Assembly big-endian implementation of Ascon-Hash using the Ascon-p instruction.
 + ascon/ascon_hash_asm_le.c: Assembly little-endian implementation of Ascon-Hash using the Ascon-p instruction with the hardware instruction performing the endianess swap.
 + ascon/ascon_hash_c_hw.c: Reference C-implementation with the Ascon-p round function swapped for the hardware round function.
 + ascon/ascon_hash_c_sw.c: Reference C-implementation for comparision.

**ISAP rk:**
 + ascon/isap.c: purely software isap-rk function and isap-rk assembly implementation using the Ascon-p instruction.

**ISAP**
 + isap/*: ISAP Opt32 reference implemenation in the same benchmarking setting as the Ascon implementation for comparision

**Misc**  
 + ascon/main.c: The default main-file to run and evaluate the selected implementation.
 + ascon/performance_main.c: Runs the encryption for a number of different lengths for AD and M and prints the resulting runtime as a CSV.
 + ascon/performance_hash_main.c: Runs the hash for a number of different lengths for AD and M and prints the resulting runtime as a CSV.
 + include/: holds routines, defines and types used for both implementations
 + common/util.c: holds the implementation for some custom printf like functions to output text on a custom asic testbench. If a different method for output is desired these funtions need to be reimplemented.

# Instructions
As a prerequisit you need a compiled RISC-V toolchain in your path.
The Ascon and ISAP folder contain a makefile that is used to compile the implementation for the HW core and spike. 
There are several defines that can overwritten to change the used implementation:
```
make all MAINFILE=main.c ASCON_CFLAGS="-DMULTIROUND_CONF=1 -DRISCY_HIGH_PERFORMANCE=0 -DLITTLE_ENDIAN_CONF=1 -DASCON_INSTR=1 -DPERFORMANCE_TEST=1"
```
**MAINFILE** an be used to specify the file with the main() function.  
**ASCON_CFLAGS** allows to specify the paramaters of the underlying hardware implementation.  
**ASCON_INSTR** can be 0, 1 or 2. 0 compiles for a CPU without the Ascon-p hardware instruction, 1 results in assembly implemenations. 2 for the reference software implemenation, but with a replaces round-function  
**LITTLE_ENDIAN_CONF** can be 0 or 1. If 0 the endian swap needs to be done in software, 1 specifies that the hardware performs the endianess swap during the Ascon-p instruction  
**MULTIROUND_CONF** specifies how many rounds of Ascon-p can be performed in hardware in a single cycle (range 0 to 8).  
**RISCY_HIGH_PERFORMANCE**: Utilizes hardware loops and load increment instructions of the cv32e40p processor to further increase the performance.  
**PERFORMANCE_TEST**: Allows to include more than one implementation in the binary (if set to 1), but reduces load time because of the larger binary size.  

## Spike
You can use `make spike` to generate isa-sim binaries only. Spike uses printf to output text.
To run the implementation you need the riscv-isa-sim and a proxy kernel. You can then run `_spike` suffixed elf files using:
```
spike --isa=rv32imafd $SDKBASE/toolchain/riscv32-unknown-elf/bin/pk ./main_spike.elf
```

## HW
`make all` generates isa-sim binaries as well as binaries for a RISC-V CPU. It should be noted that the makefile generated a system-verilog ROM file in <this_repo>/../sv/CustomRom_8192X32.sv, which can be used in a hardware design.
The binaries use custom output functions to print text (see the structure/misc secion for details.)

