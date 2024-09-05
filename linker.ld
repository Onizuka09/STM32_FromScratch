MEMORY{
  FLASH (rx) :ORIGIN =0x08000000,LENGTH =512K
  SRAM (rwx) :ORIGIN =0x20000000,LENGTH =96K 
}

SECTIONS 
{
  .text :
  {
    *(.isr_vector)
    *(.text)
    *(.rodata)
    . = ALIGN(4) ; 
    _etext = . ; 
  }>FLASH
  /*  data section  */   
  .data :
  {
    _sdata = . ; 
    *(.data)
    . = ALIGN(4) ; 
    _edata = . ; 
  }> SRAM AT> FLASH
  /* uninitialized data */ 
  .bss :
  {
    _sbss = . ;
  *(.bss) 
  *(COMMON)
  . = ALIGN(4) ; 
  _ebss = .; 

  }> SRAM

}
