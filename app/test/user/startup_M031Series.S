/**************************************************************************//**
 * @file     startup_m031series.s
 * @version  V2.00
 * @brief    CMSIS Cortex-M0 Core Device Startup File for M031 (modified)
 *
 * @note     Custom minimal version without _start, calls main directly.
 ******************************************************************************/

    .syntax unified
    .arch   armv6-m

    .section .stack
    .align  3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x00000200
#endif
    .globl  __StackTop
    .globl  __StackLimit
__StackLimit:
    .space  Stack_Size
    .size   __StackLimit, . - __StackLimit
__StackTop:
    .size   __StackTop, . - __StackTop

    .section .heap
    .align  3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0x00000100
#endif
    .globl  __HeapBase
    .globl  __HeapLimit
__HeapBase:
    .if Heap_Size
    .space  Heap_Size
    .endif
    .size   __HeapBase, . - __HeapBase
__HeapLimit:
    .size   __HeapLimit, . - __HeapLimit

    .section .vectors
    .align  2
    .globl  __Vectors
__Vectors:
    .long   __StackTop            /* Top of Stack */
    .long   Reset_Handler         /* Reset Handler */
    .long   NMI_Handler           /* NMI Handler */
    .long   HardFault_Handler     /* Hard Fault Handler */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   SVC_Handler           /* SVCall Handler */
    .long   0                     /* Reserved */
    .long   0                     /* Reserved */
    .long   PendSV_Handler        /* PendSV Handler */
    .long   SysTick_Handler       /* SysTick Handler */

    /* External interrupts */
    .long   BOD_IRQHandler        /*  0: BOD                        */
    .long   WDT_IRQHandler        /*  1: WDT                        */
    .long   EINT024_IRQHandler    /*  2: EINT0                      */
    .long   EINT135_IRQHandler    /*  3: EINT1                      */
    .long   GPABGH_IRQHandler     /*  4: GPAB                       */
    .long   GPCDEF_IRQHandler     /*  5: GPCDEF                     */
    .long   PWM0_IRQHandler       /*  6: PWM0                       */
    .long   PWM1_IRQHandler       /*  7: PWM1                       */
    .long   TMR0_IRQHandler       /*  8: TIMER0                     */
    .long   TMR1_IRQHandler       /*  9: TIMER1                     */
    .long   TMR2_IRQHandler       /* 10: TIMER2                     */
    .long   TMR3_IRQHandler       /* 11: TIMER3                     */
    .long   UART02_IRQHandler     /* 12: UART02                     */
    .long   UART13_IRQHandler     /* 13: UART13                     */
    .long   SPI0_IRQHandler       /* 14: SPI0                       */
    .long   QSPI0_IRQHandler      /* 15: QSPI0                      */
    .long   ISP_IRQHandler        /* 16: Reserved                   */
    .long   UART57_IRQHandler     /* 17: UART57                     */
    .long   I2C0_IRQHandler       /* 18: I2C0                       */
    .long   I2C1_IRQHandler       /* 19: I2C1                       */
    .long   BPWM0_IRQHandler      /* 20: BPWM0                      */
    .long   BPWM1_IRQHandler      /* 21: BPWM1                       */
    .long   USCI01_IRQHandler     /* 22: USCI01                     */
    .long   USBD_IRQHandler       /* 23: USBD                       */
    .long   Default_Handler       /* 24: Reserved                   */
    .long   ACMP01_IRQHandler     /* 25: ACMP01                     */
    .long   PDMA_IRQHandler       /* 26: PDMA                       */
    .long   UART46_IRQHandler     /* 27: UART46                     */
    .long   PWRWU_IRQHandler      /* 28: PWRWU                      */
    .long   ADC_IRQHandler        /* 29: ADC                        */
    .long   CKFAIL_IRQHandler     /* 30: CLK Fail Detect            */
    .long   RTC_IRQHandler        /* 31: RTC                        */

    .size   __Vectors, . - __Vectors

    .text
    .thumb
    .thumb_func
    .align  2
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:

    /* Copy .data section from flash to RAM */
    ldr r1, =__etext
    ldr r2, =__data_start__
    ldr r3, =__data_end__
    subs    r3, r2
    ble .L_loop1_done
.L_loop1:
    subs    r3, #4
    ldr r0, [r1, r3]
    str r0, [r2, r3]
    bgt .L_loop1
.L_loop1_done:

    /* Clear .bss section */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    movs    r0, 0
    subs    r2, r1
    ble .L_loop3_done
.L_loop3:
    subs    r2, #4
    str r0, [r1, r2]
    bgt .L_loop3
.L_loop3_done:

    /* Unlock Register */
    ldr r0, =0x40000100
    ldr r1, =0x59
    str r1, [r0]
    ldr r1, =0x16
    str r1, [r0]
    ldr r1, =0x88
    str r1, [r0]

    /* Init POR */
    ldr r0, =0x40000024
    ldr r1, =0x00005AA5
    str r1, [r0]

    /* Init LDO_RDY */
    ldr r0, =0x40000280
    ldr r1, =0x00000001
    str r1, [r0]

    /* Lock register */
    ldr r0, =0x40000100
    ldr r1, =0
    str r1, [r0]

#ifndef __NO_SYSTEM_INIT
    bl  SystemInit
#endif

    /* Directly call main instead of _start */
    bl  main
    bx  lr

    .pool
    .size   Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    b   .
    .size   Default_Handler, . - Default_Handler

    /* Define default weak IRQ handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler  NMI_Handler
    def_irq_handler  HardFault_Handler
    def_irq_handler  SVC_Handler
    def_irq_handler  PendSV_Handler
    def_irq_handler  SysTick_Handler
    def_irq_handler  BOD_IRQHandler
    def_irq_handler  WDT_IRQHandler
    def_irq_handler  EINT024_IRQHandler
    def_irq_handler  EINT135_IRQHandler
    def_irq_handler  GPABGH_IRQHandler
    def_irq_handler  GPCDEF_IRQHandler
    def_irq_handler  PWM0_IRQHandler
    def_irq_handler  PWM1_IRQHandler
    def_irq_handler  TMR0_IRQHandler
    def_irq_handler  TMR1_IRQHandler
    def_irq_handler  TMR2_IRQHandler
    def_irq_handler  TMR3_IRQHandler
    def_irq_handler  UART02_IRQHandler
    def_irq_handler  UART13_IRQHandler
    def_irq_handler  SPI0_IRQHandler
    def_irq_handler  QSPI0_IRQHandler
    def_irq_handler  ISP_IRQHandler
    def_irq_handler  UART57_IRQHandler
    def_irq_handler  I2C0_IRQHandler
    def_irq_handler  I2C1_IRQHandler
    def_irq_handler  BPWM0_IRQHandler
    def_irq_handler  BPWM1_IRQHandler
    def_irq_handler  USCI01_IRQHandler
    def_irq_handler  USBD_IRQHandler
    def_irq_handler  ACMP01_IRQHandler
    def_irq_handler  PDMA_IRQHandler
    def_irq_handler  UART46_IRQHandler
    def_irq_handler  PWRWU_IRQHandler
    def_irq_handler  ADC_IRQHandler
    def_irq_handler  CKFAIL_IRQHandler
    def_irq_handler  RTC_IRQHandler

    .end
