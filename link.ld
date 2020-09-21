OUTPUT_ARCH(riscv)

 /* required to correctly link newlib */
GROUP( -lc -lgloss -lgcc -lsupc++ )

 SEARCH_DIR(.)
__DYNAMIC  =  0;

 MEMORY
{
    rom         : ORIGIN = 0x00000000, LENGTH = 0x20000 /* 128 kB */
    ram         : ORIGIN = 0x00100000, LENGTH = 0x10000 /* 64 kB */
}

 /* Stack information variables */
_min_stack      = 0x2000;   /* 8K - minimum stack space to reserve */
_stack_start   = ORIGIN(ram) + LENGTH(ram);

 /* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

 SECTIONS
{
    .vectors :
    {
        . = ALIGN(4);
        KEEP(*(.vectors))
    } > rom

     .text : {
        . = ALIGN(4);
        _stext = .;
        *(.text)
        *(.text.*)
        _etext  =  .;
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        *(.ctors)
        LONG(0)
        __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        *(.dtors)
        LONG(0)
        __DTOR_END__ = .;
        *(.lit)
        *(.shdata)
        . = ALIGN(4);
        _endtext = .;
    }  > rom

     .rodata : {
        . = ALIGN(4);
        *(.srodata);
        *(.rodata);
        *(.rodata.*);
        *(.rela.dyn*);
    } > rom
/*
     .shbss :
    {
        . = ALIGN(4);
        *(.shbss)
    } > ram */

    __data_init_start = .;

     .data : AT (__data_init_start) {
        . = ALIGN(4);
        sdata  =  .;
        _sdata  =  .;
        *(.data);
        *(.data.*)
        edata  =  .;
        _edata  =  .;
    } > ram

     .bss :
    {
        . = ALIGN(4);
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        _bss_end = .;
    } > ram

     /* ensure there is enough room for stack */
    .stack (NOLOAD): {
        . = ALIGN(4);
        . = . + _min_stack ;
        . = ALIGN(4);
        stack = . ;
        _stack = . ;
    } > ram

     .stab  0 (NOLOAD) :
    {
        [ .stab ]
    }

     .stabstr  0 (NOLOAD) :
    {
        [ .stabstr ]
    }
}
