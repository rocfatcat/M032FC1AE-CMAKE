/* main.c (已修改為控制 PB.14) */

#include <stdint.h>
#include "M031Series.h "

/* ================================================= */
/* M032FC1AE 確切的寄存器定義 (PB.14)           */
/* ================================================= */

// /* 1. CLK 寄存器 */
// #define CLK_AHBCLK          (*(volatile uint32_t *)(CLK_BASE + 0x18)) 
// /* AHBCLK 寄存器 Bit 1: GPBRCKEN (啟用 Port B 時鐘) */
// #define CLK_AHBCLK_GPBRCKEN (1 << 1) 

// /* 2. GPIO Port B 寄存器 */
// #define GPIOB_BASE          (0x40045000UL) /* Port B 的基地址 */
// #define GPIOB_PMD           (*(volatile uint32_t *)(GPIOB_BASE + 0x00)) /* PMD 模式寄存器 */
// #define GPIOB_DOUT          (*(volatile uint32_t *)(GPIOB_BASE + 0x0C)) /* DOUT 數據輸出寄存器 */

// /* 3. PB.14 相關位定義 */
// /* PB.14 模式位: PMD 寄存器的 [29:28] 位 (14 * 2 = 28) */
// #define PB14_PMD_POS        (14 * 2)       
// #define PB14_PMD_MASK       (0b11 << PB14_PMD_POS) /* 清除 PB.14 模式的掩碼 */
// #define PB14_PMD_OUTPUT     (0b01 << PB14_PMD_POS) /* 設置為 Push-Pull Output (01) */
// #define PB14_PIN            (1 << 14)      /* PB.14 在 DOUT 寄存器中的位掩碼 */
extern void SystemInit(void);
/* ------------------ 延遲函數 ------------------ */
void simple_delay(unsigned int count)
{
    while (count--);
}

/* ------------------ 主程序 ------------------ */
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();

}

int main(void)
{
    /* 1. 啟用 GPIOB 的時鐘 */
//     CLK_AHBCLK |= CLK_AHBCLK_GPBRCKEN;

    /* 2. 配置 PB.14 為 Push-Pull 輸出模式 */
//     GPIOB_PMD = (GPIOB_PMD & ~PB14_PMD_MASK) | PB14_PMD_OUTPUT;
    SYS_Init();

    GPIO_SetMode(PB, BIT14, GPIO_MODE_QUASI);
    while (1) 
    {
        /* 設置 PB.14 為低電平 (通常 LED 點亮) */
        // GPIOB_DOUT &= ~PB14_PIN;
        PB14 ^= 1;
        simple_delay(200000); 

        /* 設置 PB.14 為高電平 (通常 LED 熄滅) */
        // GPIOB_DOUT |= PB14_PIN;
    }
}

/* ------------------ -nostdlib 必須提供的空函數 (極簡實現) ------------------ */
void *memcpy(void *dest, const void *src, unsigned int n) {
    char *dp = dest;
    const char *sp = src;
    while (n--) *dp++ = *sp++;
    return dest;
}

void *memset(void *s, int c, unsigned int n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void _exit(int status) { while(1); }
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }

