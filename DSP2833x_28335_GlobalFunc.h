
#ifndef DSK2833x_GLOBALFUNC_H
#define DSK2833x_GLOBALFUNC_H

#ifdef __cplusplus
extern "C" {
#endif

#define CS0_START_ADDR		0x004000
#define CS0_END_ADDR		0x004FFF
#define CS0_SIZE			0x001000

#define CS6_START_ADDR		0x100000
#define CS6_END_ADDR		0x1FFFFF
#define CS6_SIZE			0x100000

#define CS7_START_ADDR		0x200000
#define CS7_END_ADDR		0x2FFFFF
#define CS7_SIZE			0x100000

// Micro-Seconds Delay Function
// TI SDK 1.10 / DSP2833x_usDelay.asm / DELAY_US
#define delay_us(us)		DELAY_US	(us)	 
#define delay_ms(ms)		DELAY_US	(ms*1000)

extern char asc2hex			(char asc);  	// ASCII Data --> HEX Data
extern char hex2asc			(char hex);  	// HEX Data --> ASCII Data
WORD 		hex2_to_decimal	(BYTE b);		// 1BYTE HEX Value --> Decimal Value
LONG 		hex4_to_decimal	(WORD w);		// 2BYTE HEX Value --> Decimal Value

#define EXT_BUS_ADC			PWORD(0x004000)
#define EXT_BUS_NVRAM		PWORD(0x004100)

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // 
