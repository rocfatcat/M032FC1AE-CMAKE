/* m032_ldrom.ld */

MEMORY
{
  /* 2KB LDROM for code and read-only data */
  LDROM (rx) : ORIGIN = 0x00100000, LENGTH = 2K 

  /* 4KB SRAM for variables, stack, and heap (if used) */
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 4K 
}

/* 頂層符號定義 */
ENTRY(Reset_Handler)

SECTIONS
{
  /* * 1. .text (Code) 段
   * 包含向量表、啟動代碼和 main 函數。
   * 必須放在 LDROM 的起始位置。
   */
  .text :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))    /* 向量表 */
    *(.text)                /* 程式碼 */
    *(.text*)
    *(.rodata)              /* 只讀數據 */
    *(.rodata*)
    . = ALIGN(4);
    _etext = .;
  } > LDROM

  /* * 2. .data (已初始化數據) 段
   * 數據存放在 SRAM，但初始值（LMA）存放在 LDROM 緊隨 .text 之後。
   */
  .data : AT (_etext)
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > SRAM

  /* 3. .bss (未初始化數據) 段
   * 僅在 SRAM 中保留空間，不需要初始值。
   */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss; /* 為了啟動代碼清除 BSS */
    *(.bss)
    *(.bss*)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } > SRAM
  
  /* 4. 堆棧 (Stack)
   * 放在 SRAM 的頂部。
   */
  . = ALIGN(4);
  _end = .;
  _estack = ORIGIN(SRAM) + LENGTH(SRAM); /* SRAM 頂部地址 */
}