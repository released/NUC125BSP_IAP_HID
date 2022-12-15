/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "NuMicro.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
// extern volatile uint32_t BitFlag;

// typedef enum{
// 	flag_start = 0 ,

// 	flag_timer_period_1000ms ,
// 	flag_systick_period_1000ms ,

// 	flag_error ,		
// 	flag_DEFAULT	
// }Flag_Index;

struct flag_8bit
{
	unsigned char bit0:1 ;
	unsigned char bit1:1 ;
	unsigned char bit2:1 ;
	unsigned char bit3:1 ;
	unsigned char bit4:1 ;
	unsigned char bit5:1 ;
	unsigned char bit6:1 ;
	unsigned char bit7:1 ;
};

struct flag_16bit
{
	unsigned int bit0:1 ;
	unsigned int bit1:1 ;
	unsigned int bit2:1 ;
	unsigned int bit3:1 ;
	unsigned int bit4:1 ;
	unsigned int bit5:1 ;
	unsigned int bit6:1 ;
	unsigned int bit7:1 ;

	unsigned int bit8:1 ;
	unsigned int bit9:1 ;
	unsigned int bit10:1 ;
	unsigned int bit11:1 ;
	unsigned int bit12:1 ;
	unsigned int bit13:1 ;
	unsigned int bit14:1 ;
	unsigned int bit15:1 ;	
};

struct flag_32bit
{
	unsigned long bit0:1 ;
	unsigned long bit1:1 ;
	unsigned long bit2:1 ;
	unsigned long bit3:1 ;
	unsigned long bit4:1 ;
	unsigned long bit5:1 ;
	unsigned long bit6:1 ;
	unsigned long bit7:1 ;

	unsigned long bit8:1 ;
	unsigned long bit9:1 ;
	unsigned long bit10:1 ;
	unsigned long bit11:1 ;
	unsigned long bit12:1 ;
	unsigned long bit13:1 ;
	unsigned long bit14:1 ;
	unsigned long bit15:1 ;	

	unsigned long bit16:1 ;
	unsigned long bit17:1 ;
	unsigned long bit18:1 ;
	unsigned long bit19:1 ;
	unsigned long bit20:1 ;
	unsigned long bit21:1 ;
	unsigned long bit22:1 ;
	unsigned long bit23:1 ;

	unsigned long bit24:1 ;
	unsigned long bit25:1 ;
	unsigned long bit26:1 ;
	unsigned long bit27:1 ;
	unsigned long bit28:1 ;
	unsigned long bit29:1 ;
	unsigned long bit30:1 ;
	unsigned long bit31:1 ;
};

/*_____ D E F I N I T I O N S ______________________________________________*/
#define _debug_log_UART_					(1)


#define PROJECT_01
// #define PROJECT_02

/*_____ M A C R O S ________________________________________________________*/
// #define BitFlag_ON(flag)							(BitFlag|=flag)
// #define BitFlag_OFF(flag)							(BitFlag&=~flag)
// #define BitFlag_READ(flag)							((BitFlag&flag)?1:0)
// #define ReadBit(bit)								(uint32_t)(1<<bit)

// #define is_flag_set(idx)							(BitFlag_READ(ReadBit(idx)))
// #define set_flag(idx,en)							( (en == 1) ? (BitFlag_ON(ReadBit(idx))) : (BitFlag_OFF(ReadBit(idx))))

#define set_BIT7(x)        							(x|=0x80)
#define set_BIT6(x)        							(x|=0x40)
#define set_BIT5(x)        							(x|=0x20)
#define set_BIT4(x)        							(x|=0x10)
#define set_BIT3(x)        							(x|=0x08)
#define set_BIT2(x)        							(x|=0x04)
#define set_BIT1(x)        							(x|=0x02)
#define set_BIT0(x)        							(x|=0x01)
                                  
#define clr_BIT7(x)        							(x&=0x7F)
#define clr_BIT6(x)        							(x&=0xBF)
#define clr_BIT5(x)        							(x&=0xDF)
#define clr_BIT4(x)        							(x&=0xEF)
#define clr_BIT3(x)        							(x&=0xF7)
#define clr_BIT2(x)        							(x&=0xFB)
#define clr_BIT1(x)        							(x&=0xFD)
#define clr_BIT0(x)        							(x&=0xFE)


//MACRO : Swap Integers Macro
#ifndef SWAP
#define SWAP(a, b)     								{(a) ^= (b); (b) ^= (a); (a) ^= (b);}
#endif

#ifndef MAX
#define MAX(a,b) 									(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) 									(((a) < (b)) ? (a) : (b))
#endif

//16 --> 8 x 2
#ifndef HIBYTE
#define HIBYTE(v1)              					((uint8_t)((v1)>>8))                      		//v1 is uint16_t
#endif

#ifndef LOBYTE
#define LOBYTE(v1)              					((uint8_t)((v1)&0xFF))
#endif

//8 x 2 --> 16
#ifndef MAKEWORD
#define MAKEWORD(v1,v2)         					((((uint16_t)(v1))<<8)+(uint16_t)(v2))      //v1,v2 is uint8_t
#endif

//8 x 4 --> 32
#ifndef MAKELONG
#define MAKELONG(v1,v2,v3,v4)   					(uint32_t)((v1<<32)+(v2<<16)+(v3<<8)+v4)  //v1,v2,v3,v4 is uint8_t
#endif

#ifndef SIZEOF
#define SIZEOF(a) 									(sizeof(a) / sizeof(a[0]))
#endif

#ifndef ENDOF
#define ENDOF(a) 									((a) + SIZEOF(a))
#endif

#ifndef CLEAR
#define CLEAR(x) 									(memset(&(x), 0, sizeof (x)))
#endif

#ifndef ABS
#define ABS(X)  									((X) > 0 ? (X) : -(X)) 
#endif

#ifndef UNUSED
#define UNUSED(x) 									( (void)(x) )
#endif

/*_____ F U N C T I O N S __________________________________________________*/
void reset_buffer(void *dest, unsigned int val, unsigned int size);
void copy_buffer(void *dest, void *src, unsigned int size);
void dump_buffer(uint8_t *pucBuff, int nBytes);
void dump_buffer_hex(uint8_t *pucBuff, int nBytes);

