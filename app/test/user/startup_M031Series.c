/* startup_ldrom.c */

#include <stdint.h>

/* 從鏈接腳本獲取符號 */
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _etext;
extern uint32_t _sbss, _ebss;

/* 主函數聲明 */
extern int main(void);

/* 異常處理函數聲明 (弱符號，避免 -nostdlib 報錯) */
void Default_Handler(void);
void Reset_Handler(void);

/* ----------------------------------------------------- */
/* 向量表 (Vector Table)                  */
/* ----------------------------------------------------- */

__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = 
{
  (void (*)(void))((uint32_t)&_estack), /* 初始堆棧指針 (強制轉換地址為函數指針類型) */
  Reset_Handler,        /* 0x0000_0004: 復位處理函數 */
  Default_Handler,      /* 0x0000_0008: NMI */
  Default_Handler,      /* 0x0000_000C: Hard Fault */
  /* ... 更多異常和中斷處理函數 (這裡我們全部指向 Default_Handler) ... */
};

/* ----------------------------------------------------- */
/* 復位處理函數 (Reset Handler)           */
/* ----------------------------------------------------- */

void Reset_Handler(void)
{
  uint32_t *pSrc, *pDest;

  /* 1. 複製 .data 段 (從 LDROM 到 SRAM) */
  pSrc = &_etext;
  pDest = &_sdata;

  while (pDest < &_edata) {
    *pDest++ = *pSrc++;
  }

  /* 2. 清除 .bss 段 (在 SRAM 中填充 0) */
  pDest = &_sbss;
  while (pDest < &_ebss) {
    *pDest++ = 0;
  }
  
  /* 3. 執行用戶提供的系統初始化 ASM 代碼 */
  __asm volatile (
    /* Unlock Register (M032 NVT_ISP_RegAddr = 0x40000100) */
    "ldr r0, =0x40000100 \n"
    "ldr r1, =0x59       \n"
    "str r1, [r0]        \n"
    "ldr r1, =0x16       \n"
    "str r1, [r0]        \n"
    "ldr r1, =0x88       \n"
    "str r1, [r0]        \n"
    
    /* Init POR (M032 POR_CTRL_RegAddr = 0x40000024) */
    "ldr r0, =0x40000024 \n"
    "ldr r1, =0x00005AA5 \n"
    "str r1, [r0]        \n"
    
    /* Init LDO_RDY (M032 LDO_RDY_RegAddr = 0x40000280) */
    "ldr r0, =0x40000280 \n"
    "ldr r1, =0x00000001 \n"
    "str r1, [r0]        \n"
    
    /* Lock register (M032 NVT_ISP_RegAddr = 0x40000100) */
    "ldr r0, =0x40000100 \n"
    "ldr r1, =0          \n"
    "str r1, [r0]        \n"
    
    : /* 無輸出 */
    : /* 無輸入 */
    : "r0", "r1", "memory" /* clobber list: 告訴編譯器 r0, r1 和 memory 被修改了 */
  );
  #ifndef __NO_SYSTEM_INIT
	SystemInit();
  #endif
  /* 3. 跳轉到 main 函數 */
  main();

  /* 如果 main 返回，進入無限循環 */
  while(1);
}

/* ----------------------------------------------------- */
/* 默認異常處理函數 (Default Handler)     */
/* ----------------------------------------------------- */

void Default_Handler(void)
{
  while(1); /* 進入無限循環 */
}