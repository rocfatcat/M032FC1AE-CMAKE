.syntax unified
.arch armv6-m
.thumb


/* -------- Stack -------- */
.section .stack
.align 3
.equ Stack_Size, 0x00000200
.globl __StackTop
__StackTop:
    .space Stack_Size
__StackLimit:
    .size __StackTop, . - __StackTop

/* -------- Vector Table -------- */
.section .vectors, "a", %progbits
.align 2
.globl __Vectors
__Vectors:
    .long   __StackTop        /* Initial Stack Pointer */
    .long   Reset_Handler     /* Reset Handler */
    .long   Default_Handler   /* NMI */
    .long   Default_Handler   /* HardFault */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   Default_Handler   /* SVCall */
    .long   0                 /* Reserved */
    .long   0                 /* Reserved */
    .long   Default_Handler   /* PendSV */
    .long   Default_Handler   /* SysTick */

    /* External IRQs */
    .long Default_Handler     /*  0: BOD */
    .long Default_Handler     /*  1: WDT */
    .long Default_Handler     /*  2: EINT0/2/4 */
    .long Default_Handler     /*  3: EINT1/3/5 */
    .long Default_Handler     /*  4: GPAB */
    .long Default_Handler     /*  5: GPCDEF */
    .long Default_Handler     /*  6: PWM0 */
    .long Default_Handler     /*  7: PWM1 */
    .long Default_Handler     /*  8: TIMER0 */
    .long Default_Handler     /*  9: TIMER1 */
    .long Default_Handler     /* 10: TIMER2 */
    .long Default_Handler     /* 11: TIMER3 */
    .long Default_Handler     /* 12: UART02 */
    .long Default_Handler     /* 13: UART13 */
    .long Default_Handler     /* 14: SPI0 */
    .long Default_Handler     /* 15: QSPI0 */
    .long Default_Handler     /* 16: ISP */
    .long Default_Handler     /* 17: UART57 */
    .long Default_Handler     /* 18: I2C0 */
    .long Default_Handler     /* 19: I2C1 */
    .long Default_Handler     /* 20: BPWM0 */
    .long Default_Handler     /* 21: BPWM1 */
    .long Default_Handler     /* 22: USCI01 */
    .long USBD_IRQHandler     /* 23: USBD */
    .long Default_Handler     /* 24: Reserved */
    .long Default_Handler     /* 25: ACMP01 */
    .long Default_Handler     /* 26: PDMA */
    .long Default_Handler     /* 27: UART46 */
    .long Default_Handler     /* 28: PWRWU */
    .long Default_Handler     /* 29: ADC */
    .long Default_Handler     /* 30: CLK Fail Detect */
    .long Default_Handler     /* 31: RTC */

.size __Vectors, . - __Vectors

/* -------- Reset Handler -------- */
.text
.thumb_func
.globl Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Copy .data from Flash to RAM */
    ldr r0, =__etext
    ldr r1, =__data_start__
    ldr r2, =__data_end__
1:
    cmp r1, r2
    bcc 2f
    b   3f
2:
    ldr r3, [r0]
    str r3, [r1]
    adds r0, r0, #4
    adds r1, r1, #4
    b   1b

3:  /* Zero-fill .bss */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    movs r2, #0
4:
    cmp r0, r1
    bcc 5f
    b   6f
5:
    str r2, [r0]
    adds r0, r0, #4
    b   4b

6:
    /* Call SystemInit */
    bl  SystemInit

    /* Call main() */
    bl  main

    /* If main returns, loop forever */
7:  b 7b
.size Reset_Handler, . - Reset_Handler

/* -------- Default Handler -------- */
.thumb_func
.weak Default_Handler
.type Default_Handler, %function
Default_Handler:
    b .
.size Default_Handler, . - Default_Handler

/* -------- USB Handler (weak) -------- */
.thumb_func
.weak USBD_IRQHandler
.type USBD_IRQHandler, %function
USBD_IRQHandler:
    b .
.size USBD_IRQHandler, . - USBD_IRQHandler

/* -------- Symbols from linker script -------- */
.extern __etext
.extern __data_start__
.extern __data_end__
.extern __bss_start__
.extern __bss_end__
.extern SystemInit
.extern main
