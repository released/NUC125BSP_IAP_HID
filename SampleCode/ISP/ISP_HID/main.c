/******************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief
 * @brief    Demonstrate how to update chip flash data through USB HID interface
 *           between chip USB device and PC.
 *           Nuvoton NuMicro ISP Porgramming Tool is also required in this
 *           sample code to connect with chip USB device and assign update file
 *           of Flash.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2016 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "targetdev.h"
#include "hid_transfer.h"

#define CRYSTAL_LESS    1
#define HIRC_AUTO_TRIM  (SYS_IRCTCTL_REFCKSEL_Msk | 0x2);   /* Use USB signal to fine tune HIRC 48MHz */
#define TRIM_INIT       (SYS_BASE+0x110)
#define TRIM_THRESHOLD  16      /* Each value is 0.125%, max 2% */

#if CRYSTAL_LESS
    static volatile uint32_t s_u32DefaultTrim, s_u32LastTrim;
#endif

void USB_trim_polling(void)
{
#if CRYSTAL_LESS

    /* Start USB trim function if it is not enabled. */
    if ((SYS->IRCTCTL & SYS_IRCTCTL_FREQSEL_Msk) != 0x2)
    {
        /* Start USB trim only when USB signal arrived */
        if (USBD->INTSTS & USBD_INTSTS_SOFIF_Msk)
        {
            /* Clear SOF */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);

            /* Enable USB clock trim function */
            SYS->IRCTCTL = HIRC_AUTO_TRIM;
        }
    }

    /* Disable USB Trim when any error found */
    if (SYS->IRCTISTS & (SYS_IRCTISTS_CLKERRIF_Msk | SYS_IRCTISTS_TFAILIF_Msk))
    {
        /* Last TRIM */
        M32(TRIM_INIT) = s_u32LastTrim;

        /* Disable USB clock trim function */
        SYS->IRCTCTL = 0;

        /* Clear trim error flags */
        SYS->IRCTISTS = SYS_IRCTISTS_CLKERRIF_Msk | SYS_IRCTISTS_TFAILIF_Msk;

        /* Clear SOF */
        USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);

    }

    /* Check trim value whether it is over the threshold */
    if ((M32(TRIM_INIT) > (s_u32DefaultTrim + TRIM_THRESHOLD)) || (M32(TRIM_INIT) < (s_u32DefaultTrim - TRIM_THRESHOLD)))
    {
        /* Write updated value */
        M32(TRIM_INIT) = s_u32LastTrim;
    }
    else
    {
        /* Backup trim value */
        s_u32LastTrim =  M32(TRIM_INIT);
    }

#endif
}

void USB_trim_default(void)
{
    #if CRYSTAL_LESS
    /* Backup default trim value */
    s_u32DefaultTrim = M32(TRIM_INIT);
    s_u32LastTrim = s_u32DefaultTrim;

    /* Clear SOF */
    USBD_CLR_INT_FLAG(USBD_INTSTS_SOFIF_Msk);
    #endif
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
	UART_SetTimeoutCnt(UART0, 20);

	UART0->FIFO &= ~UART_FIFO_RFITL_4BYTES;
	UART0->FIFO |= UART_FIFO_RFITL_8BYTES;
	
	// UART_ENABLE_INT(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk);
	
    // NVIC_EnableIRQ(UART0_IRQn);

	// #if (_debug_log_UART_ == 1)	//debug	
	LDROM_DEBUG("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	LDROM_DEBUG("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());	
	LDROM_DEBUG("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	LDROM_DEBUG("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	LDROM_DEBUG("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	LDROM_DEBUG("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());
	// #endif	
}

/*--------------------------------------------------------------------------*/
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable Internal HIRC 48 MHz clock */
    CLK->PWRCTL |= CLK_PWRCTL_HIRCEN;

    /* Waiting for Internal RC clock ready */
    while (!(CLK->STATUS & CLK_STATUS_HIRCSTB_Msk));


#if (CRYSTAL_LESS)
    /* Switch HCLK clock source to Internal HIRC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Select module clock source */
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL3_USBDSEL_HIRC, CLK_CLKDIV0_USB(1));
#else
    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_48MHZ);

    /* Select module clock source */
    CLK_SetModuleClock(USBD_MODULE, CLK_CLKSEL3_USBDSEL_PLL, CLK_CLKDIV0_USB(2));
#endif


    /* Enable module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);

    /* Select module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC_DIV2, CLK_CLKDIV0_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL = SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD;

}

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Unlock write-protected registers */
    SYS_UnlockReg();
    /* Init system and multi-funcition I/O */
    SYS_Init();

    /* Enable FMC ISP */
    CLK_EnableModuleClock(ISP_MODULE);
    // Enable FMC and APROM update
    //
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();

    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);

    UART0_Init();

    while (DetectPin == 0)
    {

        /* Open USB controller */
        USBD_Open(&gsInfo, HID_ClassRequest, NULL);
        /*Init Endpoint configuration for HID */
        HID_Init();
        /* Start USB device */
        USBD_Start();
        /* Enable USB device interrupt */
        NVIC_EnableIRQ(USBD_IRQn);

        USB_trim_default();

        USB_trim_polling();

        while (DetectPin == 0)
        {
            if (bUsbDataReady == TRUE)
            {
                WDT->CTL &= ~(WDT_CTL_WDTEN_Msk | WDT_CTL_ICEDEBUG_Msk);
                WDT->CTL |= (WDT_TIMEOUT_2POW18 | WDT_CTL_RSTEN_Msk);
                ParseCmd((uint8_t *)usb_rcvbuf, EP3_MAX_PKT_SIZE);
                EP2_Handler();
                bUsbDataReady = FALSE;
            }
        }

        goto _APROM;
    }

    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);

_APROM:
    LDROM_DEBUG("Jump to <APPLICATION>\r\n");
    while(!UART_IS_TX_EMPTY(UART0));

    #if 1
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();

    /* Mask all interrupt before changing VECMAP to avoid wrong interrupt handler fetched */
    __set_PRIMASK(1);    
    FMC_SetVectorPageAddr(APROM_APPLICATION_START);

    SYS_ResetCPU();

    #else
    outpw(&SYS->RSTSTS, 3);//clear bit
    outpw(&FMC->ISPCTL, inpw(&FMC->ISPCTL) & 0xFFFFFFFC);
    outpw(&SCB->AIRCR, (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ));
    #endif

    /* Trap the CPU */
    while (1);
}



/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/

