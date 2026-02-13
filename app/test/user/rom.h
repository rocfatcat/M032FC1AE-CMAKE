#ifndef __ROM_H__
#define __ROM_H__

#define IROM2_SECTION __attribute__((section(".irom2_text"), used))

#define IROM2_DATA_SECTION //__attribute__((section(".rodata_irom2"), used))

#define IROM2_DATA8P_SECTION //__attribute__((section(".rodata_ptr8"), used))

#define IROM2_DATA32P_SECTION //__attribute__((section(".rodata_ptr32"), used))
#define IROM2_USBINFO_PTR_DATA_SECTION //__attribute__((section(".rodata_irom2.ptr_usbinfo"), used))

void UART_WriteHex(UART_T *uart, uint32_t value, uint8_t digits);
#endif // __ROM_H__