

#ifndef DSK2833x_DEFINE_H
#define DSK2833x_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef char				s8;
typedef int 				s16;
typedef long				s32;
typedef long long			s64;
typedef unsigned char		u8;
typedef unsigned int		u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;
typedef float				f32;
typedef long double			f64;

#define BOOL						char
#define BYTE						unsigned char
#define WORD						unsigned int
#define LONG						unsigned long
#define LLONG						unsigned long long
#define PBYTE(X)					*(volatile BYTE *)(X)
#define PWORD(X)					*(volatile WORD *)(X)
#define PLONG(X)					*(volatile LONG *)(X)
#define PLLONG(X)					*(volatile LLONG *)(X)
#define NOP							asm("	NOP")
#define NULL        				(void *)0
#define	HI_BYTE(val)				((val >> 8) & 0xff)
#define	LO_BYTE(val)				(val & 0xff)
#define TRUE						1
#define FALSE						0
#define HIGH						1
#define LOW							0
#define PI							3.141592
#define	CPUCLK						150000000L		// CPU Main Clock
#define SYSCLK  	    			(CPUCLK/2)
#define SYSCLK_us					(75)
#define	QUADPCLK					(CPUCLK/4)		// Peripheral Low Speed Clock

#define I2C_SLAVE_ADDR      		0x54
#define I2C_AD5627_ADDR     		0x0E
#define I2C_HT01_ADDR       		0x28

// EEPROM_ADDRESS
#define ADDR_EEPROM					10

// CPUCLK/2 = 75M, SYSCLKOUT / 2 = 37.5M,
// T_tbclk = 1/37.5M = 0.0267uS

#define mV							3000.0
#define RESOLUTION					4096.0
#define DIG2VOLT					(mV / RESOLUTION)
#define ADCH						8
#define AD_VOLT_CALC				1.005
#define AD_CONV						(DIG2VOLT * AD_VOLT_CALC)
#define AD_UPDATE_PERIOD 			7	// 2013.9.5
#define TIMEDIV1					147
#define TIMEDIV2					173
#define SAMPLE_FREQ					(150000000.0 / TIMEDIV1 / TIMEDIV2)

#define RX_MASK						0x100l	// 0x103l
#define CAN_LENGTH	  				0x08

// Sec  = Secs % 60
// Min  = Secs / 60 % 60
// Hour = Secs / 3600

//----------------------------------------------------------------------------
// DETECT --> Spectro (Byte-Length)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DT_LENGTH_01_PERIOD			1
#define DT_LENGTH_02_VER			8
#define DT_LENGTH_11_CMD			1

#define DT_LENGTH_21_TRIGGER		5
#define DT_LENGTH_22_ENERGY			8
#define DT_LENGTH_23_LASER_SET		7
#define DT_LENGTH_24_LASER_CONFIG	2

#define DT_LENGTH_32_STATE			1
#define DT_LENGTH_41_STATUS_1		8
#define DT_LENGTH_42_STATUS_2		2
#define DT_LENGTH_81_FAULT			4

//----------------------------------------------------------------------------
// SPECTRO --> Detect
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SP_CMD_01_PERIOD			0x101l	// Periodic Sequence Number
#define SP_CMD_11_CMD				0x111l	// 제어명령
#define SP_CMD_21_TRIGGER			0x121l	// Laser-Trigger
#define SP_CMD_22_ENERGY			0x122l	// Laser-Energy
#define SP_CMD_23_LASER_SET			0x123l	// Laser-Set
#define SP_CMD_24_CONFIG			0x124l  // Laser-Configuration

#define LASER_SET					0x11
#define LASER_OPERATE				0x55
#define LASER_STOP					0xAA

//----------------------------------------------------------------------------
// DETECT --> Spectro
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define DT_RSP_01_PERIOD			0x201l
#define DT_RSP_02_VERSION			0x202l
#define DT_RSP_11_CMD				0x211l
#define DT_RSP_21_TRIGGER			0x221l
#define DT_RSP_22_ENERGY			0x222l
#define DT_RSP_23_LASER_SET			0x223l
#define DT_RSP_24_LASER_CONFIG		0x224l
#define DT_RSP_32_STATE				0x232l
#define DT_RSP_41_STATUS_1			0x241l
#define DT_RSP_42_STATUS_2			0x242l
#define DT_RSP_81_FAULT				0x281l

//-----------------------------------------------------------
// OPERATION STATUS
/*
#define ST_POWER_ON 				0x01
#define ST_START_ERROR				0x0E

#define ST_INIT_RUN					0x10
#define ST_INIT_COMPLETE			0x11
#define ST_INIT_ERROR				0x1E

#define ST_READY_RUN				0x20
#define ST_READY_COMPLETE			0x21
#define ST_READY_ERROR				0x2E

#define ST_OPERATE_RUN				0x30
#define ST_OPERATE_COMPLETE			0x31
#define ST_OPERATE_ERROR			0x3E

#define ST_CHECK_RUN				0x40
#define ST_CHECK_COMPLETE			0x41
#define ST_CHECK_ERROR				0x4E

#define ST_STOP_RUN					0xF0
#define ST_STOP_COMPLETE			0xF1
#define ST_STOP_ERROR				0xFE
*/

/*
typedef enum 
{
    ST_NONE_STATE = 0,
    ST_POWER_ON = 0x01,
    ST_START_ERROR = 0x0E,

    ST_INIT_RUN = 0x10,
    ST_INIT_COMPLETE = 0x11,
    ST_INIT_ERROR = 0x1E,

    ST_READY_RUN = 0x20,
    ST_READY_COMPLETE = 0x21,
    ST_READY_ERROR = 0x2E,

    ST_OPERATE_RUN = 0x30,
    ST_OPERATE_COMPLETE = 0x31,
    ST_OPERATE_ERROR = 0x3E,

    ST_CHECK_RUN = 0x40,
    ST_CHECK_COMPLETE = 0x41,
    ST_CHECK_ERROR = 0x4E,

    ST_STOP_RUN = 0xF0,
    ST_STOP_COMPLETE = 0xF1,
    ST_STOP_ERROR = 0xFE
} E_DETECT_STATE;
*/
//-----------------------------------------------------------
// DEVICE ERROR

#define NONE_ERROR					0x00
#define EX5_RASER_ERROR				0xB1
#define BEAM_SHUTTER_ERROR			0xB2
#define EMETER_ERROR				0xB3
#define TRIGGER_ERROR				0xB4
#define CAN_ERROR					0xB5
//-----------------------------------------------------------

#define CAN_TX_PERIOD				10
#define CURR_INC_PERIOD				1000
#define TIME_SLOT		 			20

#define LASER_FILAMT				0
#define LASER_GASPRE				1
#define LASER_ENERGY				2
#define LASER_HVPEAK				3
#define LASER_ENMON					4
#define LASER_TEMP					5
#define CTRL_TEMP					6
#define PEAK_VALUE					7

#define ADC_CH8						8
#define ADC_CH9						9
#define ADC_CH10					10
#define ADC_CH11					11
#define ADC_CH12					12
#define ADC_CH13					13
#define ADC_CH14					14
#define ADC_CH15					15

#define	BAUDRATE					115200L	//230400L
#define	BAUDRATE_C					115200L
#define	BRR_VAL						(QUADPCLK/(8*BAUDRATE)-1)	// BaudRate-Register Value
#define	BRR_VAL_C					(QUADPCLK/(8*BAUDRATE_C)-1)	// BaudRate-Register Value
#define	DEBUG_BUF_SIZE				500	//1000

#define LF							0x0A	// Line Feed Character
#define CR							0x0D	// Carrage Return Character
#define	BACK 						0x08
#define	ESC  						0x1B
#define	SPACE 						0x20
#define	END 						0xFF
#define	NUL 						0x00
#define EOI 						-10000
#define DOT							0x2E
#define	IN_NUM 						30	//10

#define BIAS_V						5.0			// Bias-5V
#define R_FACTOR					0.6			// Divider 3k/(2k+3k)
#define B_VALUE						3970		// Sensor b-value
#define B_VALUE_IGBT 				3411		// IGBT b-value
#define T_25						298.15		// 25, Kelvin
#define LN2							0.693147f	// LN2 Value define

#define ADCLO						0			// ADC offset calib

#define	DEBUG_TX_STOP				ScibRegs.SCICTL2.bit.TXINTENA = 0

#define BIT_MASK					(bit)		(1 << (bit))
#define GetBit						(val, bit)	(((val) & BIT_MASK(bit)) >> (bit))
#define SetBit						(val, bit)	(val  |=  BIT_MASK(bit))
#define ClearBit					(val, bit)	(val  &= ~BIT_MASK(bit))
#define ToggleBit					(val, bit)	(val  ^=  BIT_MASK(bit))
#define bit_is_set					(val, bit)	(val  &   BIT_MASK(bit))
#define bit_is_clear				(val, bit)	(~val &   BIT_MASK(bit))

#define BIT0_POS    				0
#define BIT1_POS    				1
#define BIT2_POS    				2
#define BIT3_POS    				3
#define BIT4_POS    				4
#define BIT5_POS    				5
#define BIT6_POS    				6
#define BIT7_POS    				7
#define BIT8_POS    				8
#define BIT9_POS    				9
#define BIT10_POS   				10
#define BIT11_POS   				11
#define BIT12_POS   				12
#define BIT13_POS   				13
#define BIT14_POS   				14
#define BIT15_POS   				15

#define BIT0_MASK    				0x0001
#define BIT1_MASK    				0x0002
#define BIT2_MASK    				0x0004
#define BIT3_MASK    				0x0008
#define BIT4_MASK    				0x0010
#define BIT5_MASK    				0x0020
#define BIT6_MASK    				0x0040
#define BIT7_MASK    				0x0080
#define BIT8_MASK    				0x0100
#define BIT9_MASK    				0x0200
#define BIT10_MASK   				0x0400
#define BIT11_MASK   				0x0800
#define BIT12_MASK   				0x1000
#define BIT13_MASK   				0x2000
#define BIT14_MASK   				0x4000
#define BIT15_MASK   				0x8000

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_28335_GlobalFunc.h"

typedef union
{
	Uint16	w[2];
	Uint32	l;
}L_TYPE;

typedef union
{
	Uint16	w[16];
	Uint32	l[8];
}H_TYPE;

// 예약어-구조체 태그
// struct card 형의 정의 (변수 선언 아님)
typedef struct {
	BOOL e15;
	BOOL e16;
	BOOL e17;
	BOOL e18;
	BOOL e23;
	BOOL e24;
	BOOL e31;
	BOOL e51;
	BOOL e52;
	BOOL e53;
	BOOL e54;
	BOOL e55;
	BOOL e61;
	BOOL e62;
	BOOL e63;
	BOOL e64;
} LDM42_ERROR_CODE;

typedef struct {
	BOOL flt_laser_init;			//  0
	BOOL flt_laser_config;			//  1
	BOOL flt_laser_operate;			//  2
	BOOL flt_beam_shutter;			//  3
	BOOL flt_emeter_operate;		//  4
	BOOL flt_laser_trig;			//  5
	BOOL flt_laser_temp;			//  6
	BOOL flt_laser_gas_pressure;	//  7
	BOOL flt_laser_filament_m;		//  8
	BOOL flt_laser_hv_peak;			//  9
	BOOL flt_laser_emeter;			// 10
	BOOL flt_laser_period_config;	// 11
} DETECT_ERROR_TYPE;        // 0: OK, 1:Fault

typedef struct {
	BOOL ls_reset;
	BOOL ls_trigger;
	BOOL ls_mode;					// Low : Command / High : Immed
	BOOL ls_flu_injector;			// Low : Closed / High : Open
	BOOL ls_cool_fan;
	BOOL ls_sol_1;					// Pump out		(Low : Closed / High : Open)
	BOOL ls_sol_2;					// Fill 		(Low : Closed / High : Open)
	BOOL ls_sol_3;					// Main			(Low : Closed / High : Open)
	BOOL ls_sol_4;					// Buffer/Purge
	BOOL ls_motor_1;				// Low : Off / High : On
	BOOL ls_motor_2;				// Low : Off / High : On
	BOOL ls_vac_pump;				// Low : Off / High : On
} LASER_CTRL_LINE;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif // end of DSK2833x_DEFINE_H definition
