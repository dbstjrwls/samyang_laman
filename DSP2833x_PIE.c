
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"
#include "math.h"
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"
#include "IO.h"
#include "Uart.h"
#include "I2c.h"
#include "Pwm.h"
#include "Timer.h"
#include "global.h"

//#include "SolidLaser_UartProtocolProcess.h"

unsigned long timeBase = 0;

unsigned int xint3_cnt = 0;		// XINT3
unsigned int xint4_cnt = 0;		// XINT4
unsigned int xint5_cnt = 0;		// XINT5
unsigned int xint6_cnt = 0;		// XINT6

const struct PIE_VECT_TABLE PieVectTableInit = {
      PIE_RESERVED,  	// 0  Reserved space
      PIE_RESERVED,  	// 1  Reserved space
      PIE_RESERVED,  	// 2  Reserved space
      PIE_RESERVED,  	// 3  Reserved space
      PIE_RESERVED,  	// 4  Reserved space
      PIE_RESERVED,  	// 5  Reserved space
      PIE_RESERVED,  	// 6  Reserved space
      PIE_RESERVED,  	// 7  Reserved space
      PIE_RESERVED,  	// 8  Reserved space
      PIE_RESERVED,  	// 9  Reserved space
      PIE_RESERVED,  	// 10 Reserved space
      PIE_RESERVED,  	// 11 Reserved space
      PIE_RESERVED,  	// 12 Reserved space

// Non-Peripheral Interrupts
      INT13_ISR,     	// XINT13 or CPU-Timer 1
      INT14_ISR,     	// CPU-Timer2
      DATALOG_ISR,   	// Datalogging interrupt
      RTOSINT_ISR,   	// RTOS interrupt
      EMUINT_ISR,    	// Emulation interrupt
      NMI_ISR,       	// Non-maskable interrupt
      ILLEGAL_ISR,   	// Illegal operation TRAP
	  USER1_ISR,
	  USER2_ISR,
	  USER3_ISR,
	  USER4_ISR,
      USER5_ISR,     	// User Defined trap 5
      USER6_ISR,     	// User Defined trap 6
      USER7_ISR,     	// User Defined trap 7
      USER8_ISR,     	// User Defined trap 8
      USER9_ISR,     	// User Defined trap 9
      USER10_ISR,    	// User Defined trap 10
      USER11_ISR,    	// User Defined trap 11
      USER12_ISR,    	// User Defined trap 12

// Group 1 PIE Vectors
      SEQ1INT_ISR,     	// 1.1 ADC
      SEQ2INT_ISR,     	// 1.2 ADC
      rsvd_ISR,        	// 1.3
      XINT1_ISR,       	// 1.4
      XINT2_ISR,       	// 1.5
      ADCINT_ISR,      	// 1.6 ADC
      TINT0_ISR,       	// 1.7 Timer 0
      WAKEINT_ISR,     	// 1.8 WD, Low Power

// Group 2 PIE Vectors
      EPWM1_TZINT_ISR, 	// 2.1 EPWM-1 Trip Zone
      EPWM2_TZINT_ISR, 	// 2.2 EPWM-2 Trip Zone
      EPWM3_TZINT_ISR, 	// 2.3 EPWM-3 Trip Zone
      EPWM4_TZINT_ISR, 	// 2.4 EPWM-4 Trip Zone
      EPWM5_TZINT_ISR, 	// 2.5 EPWM-5 Trip Zone
      EPWM6_TZINT_ISR, 	// 2.6 EPWM-6 Trip Zone
      rsvd_ISR,        	// 2.7
      rsvd_ISR,        	// 2.8

// Group 3 PIE Vectors
      EPWM1_INT_ISR,   	// 3.1 EPWM-1 Interrupt
      EPWM2_INT_ISR,   	// 3.2 EPWM-2 Interrupt
      EPWM3_INT_ISR,   	// 3.3 EPWM-3 Interrupt
      EPWM4_INT_ISR,   	// 3.4 EPWM-4 Interrupt
      EPWM5_INT_ISR,   	// 3.5 EPWM-5 Interrupt
      EPWM6_INT_ISR,   	// 3.6 EPWM-6 Interrupt
      rsvd_ISR,        	// 3.7
      rsvd_ISR,        	// 3.8

// Group 4 PIE Vectors
      ECAP1_INT_ISR,   	// 4.1 ECAP-1
      ECAP2_INT_ISR,   	// 4.2 ECAP-2
      ECAP3_INT_ISR,   	// 4.3 ECAP-3
      ECAP4_INT_ISR,   	// 4.4 ECAP-4
      ECAP5_INT_ISR,   	// 4.5 ECAP-5
      ECAP6_INT_ISR,   	// 4.6 ECAP-6
      rsvd_ISR,        	// 4.7
      rsvd_ISR,        	// 4.8

// Group 5 PIE Vectors
      EQEP1_INT_ISR,   	// 5.1 EQEP-1
      EQEP2_INT_ISR,   	// 5.2 EQEP-2
      rsvd_ISR,        	// 5.3
      rsvd_ISR,        	// 5.4
      rsvd_ISR,        	// 5.5
      rsvd_ISR,        	// 5.6
      rsvd_ISR,        	// 5.7
      rsvd_ISR,        	// 5.8

// Group 6 PIE Vectors
      SPIRXINTA_ISR,   	// 6.1 SPI-A
      SPITXINTA_ISR,   	// 6.2 SPI-A
      MRINTA_ISR,      	// 6.3 McBSP-A
      MXINTA_ISR,      	// 6.4 McBSP-A
      MRINTB_ISR,      	// 6.5 McBSP-B
      MXINTB_ISR,      	// 6.6 McBSP-B
      rsvd_ISR,        	// 6.7
      rsvd_ISR,        	// 6.8

// Group 7 PIE Vectors
      DINTCH1_ISR,     	// 7.1  DMA channel 1
      DINTCH2_ISR,     	// 7.2  DMA channel 2
      DINTCH3_ISR,     	// 7.3  DMA channel 3
      DINTCH4_ISR,     	// 7.4  DMA channel 4
      DINTCH5_ISR,     	// 7.5  DMA channel 5
      DINTCH6_ISR,     	// 7.6  DMA channel 6
      rsvd_ISR,        	// 7.7
      rsvd_ISR,        	// 7.8

// Group 8 PIE Vectors
      I2CINT1A_ISR,    	// 8.1  I2C
      I2CINT2A_ISR,    	// 8.2  I2C
      rsvd_ISR,        	// 8.3
      rsvd_ISR,        	// 8.4
      SCIRXINTC_ISR,   	// 8.5  SCI-C
      SCITXINTC_ISR,   	// 8.6  SCI-C
      rsvd_ISR,        	// 8.7
      rsvd_ISR,        	// 8.8

// Group 9 PIE Vectors
      SCIRXINTA_ISR,   	// 9.1 SCI-A
      SCITXINTA_ISR,   	// 9.2 SCI-A
      SCIRXINTB_ISR,   	// 9.3 SCI-B
      SCITXINTB_ISR,   	// 9.4 SCI-B
      ECAN0INTA_ISR,   	// 9.5 eCAN-A
      ECAN1INTA_ISR,   	// 9.6 eCAN-A
      ECAN0INTB_ISR,   	// 9.7 eCAN-B
      ECAN1INTB_ISR,   	// 9.8 eCAN-B

// Group 10 PIE Vectors
      rsvd_ISR,        	// 10.1
      rsvd_ISR,        	// 10.2
      rsvd_ISR,        	// 10.3
      rsvd_ISR,        	// 10.4
      rsvd_ISR,        	// 10.5
      rsvd_ISR,        	// 10.6
      rsvd_ISR,        	// 10.7
      rsvd_ISR,        	// 10.8

// Group 11 PIE Vectors
      rsvd_ISR,        	// 11.1
      rsvd_ISR,        	// 11.2
      rsvd_ISR,        	// 11.3
      rsvd_ISR,        	// 11.4
      rsvd_ISR,        	// 11.5
      rsvd_ISR,        	// 11.6
      rsvd_ISR,        	// 11.7
      rsvd_ISR,        	// 11.8

// Group 12 PIE Vectors
      XINT3_ISR,       	// 12.1
      XINT4_ISR,       	// 12.2
      XINT5_ISR,       	// 12.3
      XINT6_ISR,       	// 12.4
      XINT7_ISR,       	// 12.5
      rsvd_ISR,        	// 12.6
      LVF_ISR,         	// 12.7
      LUF_ISR,         	// 12.8
};

//---------------------------------------------------------------------------
// InitPieVectTable:
//---------------------------------------------------------------------------
// This function initializes the PIE vector table to a known state.
// This function must be executed after boot time.
//
void InitPieVectTable(void)
{
	int16 i;
	Uint32 *Source = (void *) &PieVectTableInit;
	Uint32 *Dest = (void *) &PieVectTable;

	EALLOW;
	for(i = 0; i < 128; i++)
		*Dest++ = *Source++;
	EDIS;

	// Enable the PIE Vector Table
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
}

//---------------------------------------------------------------------------
// InitPieCtrl:
//---------------------------------------------------------------------------
// This function initializes the PIE control registers to a known state.
//
void InitPieCtrl(void)
{
    // Disable Interrupts at the CPU level:
    DINT;

    // Disable the PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;

	// Clear all PIEIER registers:
	PieCtrlRegs.PIEIER1.all = 0;
	PieCtrlRegs.PIEIER2.all = 0;
	PieCtrlRegs.PIEIER3.all = 0;
	PieCtrlRegs.PIEIER4.all = 0;
	PieCtrlRegs.PIEIER5.all = 0;
	PieCtrlRegs.PIEIER6.all = 0;
	PieCtrlRegs.PIEIER7.all = 0;
	PieCtrlRegs.PIEIER8.all = 0;
	PieCtrlRegs.PIEIER9.all = 0;
	PieCtrlRegs.PIEIER10.all = 0;
	PieCtrlRegs.PIEIER11.all = 0;
	PieCtrlRegs.PIEIER12.all = 0;

	// Clear all PIEIFR registers:
	PieCtrlRegs.PIEIFR1.all = 0;
	PieCtrlRegs.PIEIFR2.all = 0;
	PieCtrlRegs.PIEIFR3.all = 0;
	PieCtrlRegs.PIEIFR4.all = 0;
	PieCtrlRegs.PIEIFR5.all = 0;
	PieCtrlRegs.PIEIFR6.all = 0;
	PieCtrlRegs.PIEIFR7.all = 0;
	PieCtrlRegs.PIEIFR8.all = 0;
	PieCtrlRegs.PIEIFR9.all = 0;
	PieCtrlRegs.PIEIFR10.all = 0;
	PieCtrlRegs.PIEIFR11.all = 0;
	PieCtrlRegs.PIEIFR12.all = 0;
}

//---------------------------------------------------------------------------
// InitCpuTimers:
//---------------------------------------------------------------------------
// This function initializes all three CPU timers to a known state.
//
void InitCpuTimers(void)
{
    // CPU Timer 0
	// Initialize address pointers to respective timer registers:
	CpuTimer0.RegsAddr = &CpuTimer0Regs;
	// Initialize timer period to maximum:
	CpuTimer0Regs.PRD.all = 0xFFFFFFFF;
	// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer0Regs.TPR.all = 0;
	CpuTimer0Regs.TPRH.all = 0;
	// Make sure timer is stopped:
	CpuTimer0Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer0Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer0.InterruptCount = 0;

// CpuTimer 1 and CpuTimer2 are reserved for DSP BIOS & other RTOS
// Do not use these two timers if you ever plan on integrating
// DSP-BIOS or another realtime OS.
//
// Initialize address pointers to respective timer registers:
	CpuTimer1.RegsAddr = &CpuTimer1Regs;
	CpuTimer2.RegsAddr = &CpuTimer2Regs;
	// Initialize timer period to maximum:
	CpuTimer1Regs.PRD.all = 0xFFFFFFFF;
	CpuTimer2Regs.PRD.all = 0xFFFFFFFF;
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
	CpuTimer1Regs.TPR.all = 0;
	CpuTimer1Regs.TPRH.all = 0;
	CpuTimer2Regs.TPR.all = 0;
	CpuTimer2Regs.TPRH.all = 0;
    // Make sure timers are stopped:
	CpuTimer1Regs.TCR.bit.TSS = 1;
	CpuTimer2Regs.TCR.bit.TSS = 1;
	// Reload all counter register with period value:
	CpuTimer1Regs.TCR.bit.TRB = 1;
	CpuTimer2Regs.TCR.bit.TRB = 1;
	// Reset interrupt counters:
	CpuTimer1.InterruptCount = 0;
	CpuTimer2.InterruptCount = 0;
}

//---------------------------------------------------------------------------
// ConfigCpuTimer:
//---------------------------------------------------------------------------
// This function initializes the selected timer to the period specified
// by the "Freq" and "Period" parameters. The "Freq" is entered as "MHz"
// and the period in "uSeconds". The timer is held in the stopped state
// after configuration.
//
void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq, float Period)
{
	Uint32 	temp;

	// Initialize timer period:
	Timer->CPUFreqInMHz = Freq;
	Timer->PeriodInUSec = Period;
	temp = (long) (Freq * Period);
	Timer->RegsAddr->PRD.all = temp;

	// Set pre-scale counter to divide by 1 (SYSCLKOUT):
	Timer->RegsAddr->TPR.all = 0;
	Timer->RegsAddr->TPRH.all = 0;

	// Initialize timer control register:
	Timer->RegsAddr->TCR.bit.TSS = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
	Timer->RegsAddr->TCR.bit.TRB = 1;      // 1 = reload timer
	Timer->RegsAddr->TCR.bit.SOFT = 0;
	Timer->RegsAddr->TCR.bit.FREE = 0;     // Timer Free Run Disabled
	Timer->RegsAddr->TCR.bit.TIE = 1;      // 0 = Disable/ 1 = Enable Timer Interrupt

	// Reset interrupt counter:
	Timer->InterruptCount = 0;
}

//-------------------------------------------------------------------------------------------------
// Connected to INT13 of CPU (use MINT13 mask):
// Note CPU-Timer1 is reserved for TI use, however XINT13  ISR can be used by the user.

interrupt void INT13_ISR(void)     // INT13 or CPU-Timer1  --> 1ms Period
{
    static volatile u16 dt_freq = 0;
    static volatile u16 laser_trigger_count = 0;
    static volatile u16 trig_gen_cnt = 0;
	static volatile u16 CAN_PERIOD_CHECK = 0;
	static volatile u16 Laser_Fault_Cnt = 0;
    static volatile BOOL p_width_flag = 1;
    static unsigned led_cnt = 0;

	timerIsr();

	dt_freq = cmd_laser_set_period;     // can에서 수신.

	if(CAN_CH == 1)
	{
		if(CAN_PERIOD_CHECK >= 10)	// 10ms
		{
			CAN_TX_INT_FLAG = 1;
			CAN_PERIOD_CHECK = 0;
		}
		else CAN_PERIOD_CHECK++;
	}
/*
	//-------------------------------------------------------------------------------------------------
	if(EX5_Laser_Fault == 1 && Laser_Pwr_Complete == 0) {	// START-UP EX-5 LASER
		Laser_Fault_Cnt++;
		if(Laser_Fault_Cnt >= 1000) {		// 60000
			Laser_Fault_Cnt = 0;
			sec_cnt++;
			if(sec_cnt >= 300) {			// 5
				Laser_Pwr_Complete = 1;
			}
		}
	}
	else if(EX5_Laser_Fault == 1 && Laser_Pwr_Complete == 1)
	{
		//
	}
	else {
		Laser_Fault_Cnt = 0;
		sec_cnt = 0;
		Laser_Pwr_Complete = 0;
	}
*/

	if(++led_cnt >= 1000)
	{
		led_cnt = 0;
		LED1_TOGGLE();
 	}

	if((p_width_flag == 0) && (!laser_trigger_flag))
	{
		Trigger_Gen_Repeat(1);
		//CM_TRIG_HIGH();
		if(trig_gen_cnt == 1)		// 5
			p_width_flag = 1;		// 1 -> 0
	}
	//-------------------------------------------------------------------------------------------------
	//if(trig_gen_cmd && !laser_trigger_flag && detect_state == ST_OPERATE_COMPLETE)
    if(trig_gen_cmd && !laser_trigger_flag && (GetDetectState() == ST_OPERATE_COMPLETE))
	{
		++trig_gen_cnt;
		if(trig_gen_cnt >= dt_freq)			// ex) 10Hz(100ms)
		{
			rsp_trigger_state = 1;          // trigger 발생상태

			//rsp_trigger_index++;
			laser_trigger_count++;

			trig_gen_cnt = 0;
			p_width_flag = 0;				// High Period

			//Trigger_Gen_Repeat(1);
			//CM_TRIG_HIGH();

			if(cmd_laser_set_repeat > 0)	// repeat == 0 (don't operation)
			{
				if(laser_trigger_count > cmd_laser_set_repeat)	// End Trigger Repeat Operation
				{
					laser_trigger_count = 0;
					laser_trigger_flag = 1;
				}
			}
			if(laser_trigger_flag == 0)
				Trigger_Gen_Repeat(1);
			else
				Trigger_Gen_Repeat(0);
		}
		else
		{
			if(p_width_flag == 1)
			{
				//rsp_trigger_state = 0;
				Trigger_Gen_Repeat(0);
				//CM_TRIG_LOW();
			}
		}
	}
	//else if(trig_gen_cmd && laser_trigger_flag)
	if(trig_gen_cmd == 0 || laser_trigger_flag == 1)
	{
		p_width_flag = 1;
		rsp_trigger_state = 0;
		trig_gen_cnt = 0;
		laser_trigger_count = 0;
		Trigger_Gen_Repeat(0);
	}
	else
	{
		//rsp_trigger_state = 0;
		//trig_gen_cnt = 0;
		//laser_trigger_count = 0;
		//Trigger_Gen_Repeat(0);
	}
}

// Note CPU-Timer2 is reserved for TI use.
interrupt void INT14_ISR(void)     // CPU-Timer2   1ms period
{
	LaserControlHandler_100ms();
//	Laser_TX_Test();
}

interrupt void DATALOG_ISR(void)   // Datalogging interrupt
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void RTOSINT_ISR(void)   // RTOS interrupt
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void EMUINT_ISR(void)    // Emulation interrupt
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void NMI_ISR(void)       // Non-maskable interrupt
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void ILLEGAL_ISR(void)   // Illegal operation TRAP
{
	Debug_cmd_flag = 0;
	Debug_cmd_flag_c = 0;

	for(;;)
	{
		if(ADC_COMPLETE  == 1)
		{
			ADC_COMPLETE = 0;
		}
		Monitoring();
		Can_PROCESSING();
		delay_us(30);
	}
}

interrupt void USER1_ISR(void)     // User Defined trap 1
{
	asm ("      ESTOP0");
	for(;;);
}

interrupt void USER2_ISR(void)     // User Defined trap 2
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER3_ISR(void)     // User Defined trap 3
{
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER4_ISR(void)     // User Defined trap 5
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER5_ISR(void)     // User Defined trap 5
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER6_ISR(void)     // User Defined trap 6
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER7_ISR(void)     // User Defined trap 7
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER8_ISR(void)     // User Defined trap 8
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER9_ISR(void)     // User Defined trap 9
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER10_ISR(void)    // User Defined trap 10
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER11_ISR(void)    // User Defined trap 11
{
  // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

interrupt void USER12_ISR(void)     // User Defined trap 12
{
 // Insert ISR Code here

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// -----------------------------------------------------------
// PIE Group 1 - MUXed into CPU INT1
// -----------------------------------------------------------

// INT1.1
interrupt void SEQ1INT_ISR(void)   //SEQ1 ADC
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code

  asm ("      ESTOP0");
  for(;;);
}

// INT1.2
interrupt void SEQ2INT_ISR(void)  //SEQ2 ADC
{

  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code

  asm("	  ESTOP0");
  for(;;);
}

// INT1.3 - Reserved

// INT1.4
interrupt void XINT1_ISR(void)
{
	peak_trigger_flag = 1;
	peak_trigger_cnt++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// INT1.5
interrupt void  XINT2_ISR(void)
{
  //asm ("      ESTOP0");
  //for(;;);
  	sync_trigger_flag = 1;
	sync_trigger_cnt++;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// INT1.6
interrupt void ADCINT_ISR(void)	// ADC
{
	ADC[LASER_FILAMT] = (AdcRegs.ADCRESULT0 >> 4 ) & 0x0fff;
	ADC[LASER_GASPRE] = (AdcRegs.ADCRESULT1 >> 4 ) & 0x0fff;
	ADC[LASER_ENERGY] = (AdcRegs.ADCRESULT2 >> 4 ) & 0x0fff;
	ADC[LASER_HVPEAK] = (AdcRegs.ADCRESULT3 >> 4 ) & 0x0fff;
	ADC[LASER_ENMON] = (AdcRegs.ADCRESULT4 >> 4 ) & 0x0fff;
	ADC[LASER_TEMP] = (AdcRegs.ADCRESULT5 >> 4 ) & 0x0fff;
	ADC[CTRL_TEMP] = (AdcRegs.ADCRESULT6 >> 4 ) & 0x0fff;
	ADC[PEAK_VALUE] = (AdcRegs.ADCRESULT7 >> 4 ) & 0x0fff;

	if(TEST_TIMER_Start == 1)
		ADC_COUNT ++;

	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1; 	// Clear INT SEQ1 bit
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // Acknowledge interrupt to PIE

	ADC_COMPLETE = 1;
}

// INT1.7
interrupt void  TINT0_ISR(void)      // CPU-Timer 0
{
	ADC_START();
  // To receive more interrupts from this PIE group, acknowledge this interrupt
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// INT1.8
interrupt void  WAKEINT_ISR(void)    // WD, LOW Power
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// -----------------------------------------------------------
// PIE Group 2 - MUXed into CPU INT2
// -----------------------------------------------------------

// INT2.1
interrupt void EPWM1_TZINT_ISR(void)    // EPWM-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.2
interrupt void EPWM2_TZINT_ISR(void)    // EPWM-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.3
interrupt void EPWM3_TZINT_ISR(void)    // EPWM-3
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.4
interrupt void EPWM4_TZINT_ISR(void)    // EPWM-4
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.5
interrupt void EPWM5_TZINT_ISR(void)    // EPWM-5
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.6
interrupt void EPWM6_TZINT_ISR(void)   // EPWM-6
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT2.7 - Reserved
// INT2.8 - Reserved

// -----------------------------------------------------------
// PIE Group 3 - MUXed into CPU INT3
// -----------------------------------------------------------

// INT 3.1
interrupt void EPWM1_INT_ISR(void)     // EPWM-1
{
	EPwm1Regs.CMPA.half.CMPA = 0x00;	//
	EPwm1Regs.CMPB = 0x00;
	EPwm1Regs.DBRED = 0x00;
	EPwm1Regs.DBFED = 0x00;

	//L1_INT_FLAG = 1;

   	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

   	// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.2
interrupt void EPWM2_INT_ISR(void)     // EPWM-2
{
	EPwm2Regs.CMPA.half.CMPA = 0x00;
	EPwm2Regs.CMPB = 0x00;
	EPwm2Regs.DBRED = 0x00;
	EPwm2Regs.DBFED = 0x00;

	// L2_INT_FLAG = 1;
   	// Clear INT flag for this timer
	EPwm2Regs.ETCLR.bit.INT = 1;

   	// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.3
interrupt void EPWM3_INT_ISR(void)
{
	EPwm3Regs.CMPA.half.CMPA = 0x00;	//
	EPwm3Regs.CMPB = 0x00;	//
	EPwm3Regs.DBRED = 0x00;
	EPwm3Regs.DBFED = 0x00;

	// R1_INT_FLAG = 1;

   	// Clear INT flag for this timer
	EPwm3Regs.ETCLR.bit.INT = 1;

   	// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.4
interrupt void EPWM4_INT_ISR(void)    // EPWM-4  6,7   A340V_G2, A340V_G4
{
	EPwm4Regs.CMPA.half.CMPA = 0x00;
	EPwm4Regs.CMPB = 0x00;	//
	EPwm4Regs.DBRED = 0x00;
	EPwm4Regs.DBFED = 0x00;

	//R2_INT_FLAG = 1;

   	// Clear INT flag for this timer
	EPwm4Regs.ETCLR.bit.INT = 1;

   	// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.5T
interrupt void EPWM5_INT_ISR(void)    // EPWM-5A/B  8,9
{
	EPwm5Regs.ETCLR.bit.INT = 1; // Clear INT flag for this timer

// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.6
interrupt void EPWM6_INT_ISR(void)    // EPWM-6A/B, 10,11
{
	EPwm6Regs.CMPA.half.CMPA = PWM;
	EPwm6Regs.ETCLR.bit.INT = 1; // Clear INT flag for this timer

   	// Acknowledge this interrupt to receive more interrupts from group 3
   	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

// INT3.7 - Reserved
// INT3.8 - Reserved
// -----------------------------------------------------------
// PIE Group 4 - MUXed into CPU INT4
// -----------------------------------------------------------

// INT 4.1
interrupt void ECAP1_INT_ISR(void)    // ECAP-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.2
interrupt void ECAP2_INT_ISR(void)    // ECAP-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.3
interrupt void ECAP3_INT_ISR(void)    // ECAP-3
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.4
interrupt void ECAP4_INT_ISR(void)     // ECAP-4
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.5
interrupt void ECAP5_INT_ISR(void)     // ECAP-5
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT4.6
interrupt void ECAP6_INT_ISR(void)     // ECAP-6
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}
// INT4.7 - Reserved
// INT4.8 - Reserved

// -----------------------------------------------------------
// PIE Group 5 - MUXed into CPU INT5
// -----------------------------------------------------------

// INT 5.1
interrupt void EQEP1_INT_ISR(void)    // EQEP-1
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT5.2
interrupt void EQEP2_INT_ISR(void)    // EQEP-2
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT5.3 - Reserved
// INT5.4 - Reserved
// INT5.5 - Reserved
// INT5.6 - Reserved
// INT5.7 - Reserved
// INT5.8 - Reserved

// -----------------------------------------------------------
// PIE Group 6 - MUXed into CPU INT6
// -----------------------------------------------------------

// INT6.1
interrupt void SPIRXINTA_ISR(void)    // SPI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.2
interrupt void SPITXINTA_ISR(void)     // SPI-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.3
interrupt void MRINTB_ISR(void)     // McBSP-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT6.4
interrupt void MXINTB_ISR(void)     // McBSP-B
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT6.5
interrupt void MRINTA_ISR(void)     // McBSP-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.6
interrupt void MXINTA_ISR(void)     // McBSP-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT6.7 - Reserved
// INT6.8 - Reserved

// -----------------------------------------------------------
// PIE Group 7 - MUXed into CPU INT7
// -----------------------------------------------------------

// INT7.1
interrupt void DINTCH1_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.2
interrupt void DINTCH2_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.3
interrupt void DINTCH3_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.4
interrupt void DINTCH4_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.5
interrupt void DINTCH5_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.6
interrupt void DINTCH6_ISR(void)     // DMA
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
   asm ("      ESTOP0");
   for(;;);
}

// INT7.7 - Reserved
// INT7.8 - Reserved

// -----------------------------------------------------------
// PIE Group 8 - MUXed into CPU INT8
// -----------------------------------------------------------

// INT8.1
interrupt void I2CINT1A_ISR(void)     // I2C-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT8.2
interrupt void I2CINT2A_ISR(void)     // I2C-A
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}

// INT8.3 - Reserved
// INT8.4 - Reserved

//----------------------------------------------------------------------------
// INT8.5
interrupt void SCIRXINTC_ISR(void)     // SCI-C
{
    u8 bData = ScicRegs.SCIRXBUF.all;

	SCICProtocolFunction(bData);
	// Acknowledge this interrupt to recieve more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

// INT8.6
interrupt void SCITXINTC_ISR(void)     // SCI-C
{
  // Insert ISR Code here

  // To receive more interrupts from this PIE group, acknowledge this interrupt
  // PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

  // Next two lines for debug only to halt the processor here
  // Remove after inserting ISR Code
  asm ("      ESTOP0");
  for(;;);
}
//----------------------------------------------------------------------------

// INT8.7 - Reserved
// INT8.8 - Reserved

// -----------------------------------------------------------
// PIE Group 9 - MUXed into CPU INT9
// -----------------------------------------------------------

//----------------------------------------------------------------------------
// INT9.1  UART RX
interrupt void SCIRXINTA_ISR(void)     	// SCI-A
{
	//asm ("      ESTOP0");
	//for(;;);
	UART_INTERRUPT = 1;
	rxd = SciaRegs.SCIRXBUF.all;
	// Acknowledge this interrupt to recieve more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// INT9.2
interrupt void SCITXINTA_ISR(void)     	// SCI-A
{
	asm ("      ESTOP0");
	for(;;);
}

//----------------------------------------------------------------------------
// INT9.3
interrupt void SCIRXINTB_ISR(void)
{
	//
}

// INT9.4
interrupt void SCITXINTB_ISR(void)
{
	asm ("      ESTOP0");
	for(;;);
}
//----------------------------------------------------------------------------

// INT9.5
interrupt void ECAN0INTA_ISR(void)  	// eCAN-A
{
	if(ECanaRegs.CANRMP.bit.RMP0 == 1)
	{
		if(!CAN_rx_flag)
		{
			CAN_rx_flag = TRUE;
			CAN_rx_id = ECanaMboxes.MBOX0.MSGID.bit.STDMSGID;
			CAN_rx_length = ECanaMboxes.MBOX0.MSGCTRL.bit.DLC;
			CAN_rx_low_data = ECanaMboxes.MBOX0.MDL.all;
			CAN_rx_high_data = ECanaMboxes.MBOX0.MDH.all;

			if(CAN_CH_FLAG == 0)
			{
				Can_Process_RX();
			}
			CAN_rx_flag = FALSE;
			ECanaRegs.CANRMP.bit.RMP0 = 1; // Clear receive pending flag
		}
	}
	// Acknowledge this interrupt to receive more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// INT9.6
interrupt void ECAN1INTA_ISR(void)  	// eCAN-A
{
 	if(ECanaRegs.CANTA.bit.TA1 == 1)
	{
		ECanaRegs.CANTA.bit.TA1 = 1;  	// Clear transmit-acknowledge pending flag
	}
	// Acknowledge this interrupt to receive more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// INT9.7
interrupt void ECAN0INTB_ISR(void)  // eCAN-B
{
	if(ECanbRegs.CANRMP.bit.RMP0 == 1)
	{
		if(!CAN_rx_flag)
		{
			CAN_rx_flag = TRUE;
			CAN_rx_id = ECanbMboxes.MBOX0.MSGID.bit.STDMSGID;
			CAN_rx_length = ECanbMboxes.MBOX0.MSGCTRL.bit.DLC;
			CAN_rx_low_data = ECanbMboxes.MBOX0.MDL.all;
			CAN_rx_high_data = ECanbMboxes.MBOX0.MDH.all;

			if(CAN_CH_FLAG == 1)
			{
				Can_Process_RX();
			}
			CAN_rx_flag = FALSE;
			ECanbRegs.CANRMP.bit.RMP0 = 1; // Clear receive pending flag
		}
	}
	// Acknowledge this interrupt to receive more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// INT9.8
interrupt void ECAN1INTB_ISR(void)  // eCAN-B
{
 	if(ECanbRegs.CANTA.bit.TA1 == 1)
	{
		ECanbRegs.CANTA.bit.TA1 = 1;  // Clear transmit-acknowledge pending flag
	}
	// Acknowledge this interrupt to receive more interrupts from group 9
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

// -----------------------------------------------------------
// PIE Group 10 - MUXed into CPU INT10
// -----------------------------------------------------------

// INT10.1 - Reserved
// INT10.2 - Reserved
// INT10.3 - Reserved
// INT10.4 - Reserved
// INT10.5 - Reserved
// INT10.6 - Reserved
// INT10.7 - Reserved
// INT10.8 - Reserved

// -----------------------------------------------------------
// PIE Group 11 - MUXed into CPU INT11
// -----------------------------------------------------------

// INT11.1 - Reserved
// INT11.2 - Reserved
// INT11.3 - Reserved
// INT11.4 - Reserved
// INT11.5 - Reserved
// INT11.6 - Reserved
// INT11.7 - Reserved
// INT11.8 - Reserved

// -----------------------------------------------------------
// PIE Group 12 - MUXed into CPU INT12
// -----------------------------------------------------------

// INT12.1
interrupt void XINT3_ISR(void)  // External Interrupt
{
	//asm ("      ESTOP0");
	//for(;;);
	xint3_cnt++;

	// Acknowledge this interrupt to receive more interrupts from group 12
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// INT12.2
interrupt void XINT4_ISR(void)  // External Interrupt
{
	//asm ("      ESTOP0");
	//for(;;);
	xint4_cnt++;

	// Acknowledge this interrupt to receive more interrupts from group 12
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// INT12.3
interrupt void XINT5_ISR(void)  // External Interrupt
{
	//asm ("      ESTOP0");
	//for(;;);
	xint5_cnt++;

	// Acknowledge this interrupt to receive more interrupts from group 12
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// INT12.4
interrupt void XINT6_ISR(void)  // External Interrupt
{
	//asm ("      ESTOP0");
	//for(;;);
	xint6_cnt++;

	// Acknowledge this interrupt to receive more interrupts from group 12
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}

// INT12.5
interrupt void XINT7_ISR(void)  // External Interrupt
{
	asm ("      ESTOP0");
	for(;;);
}

// INT12.6 - Reserved
// INT12.7
interrupt void LVF_ISR(void)  // Latched overflow
{
	asm ("      ESTOP0");
	for(;;);
}

// INT12.8
interrupt void LUF_ISR(void)  // Latched underflow
{
	asm ("      ESTOP0");
	for(;;);
}

//---------------------------------------------------------------------------
// Catch All Default ISRs:
interrupt void PIE_RESERVED(void)  // Reserved space.  For test.
{
	asm ("      ESTOP0");
	for(;;);
}

interrupt void rsvd_ISR(void)      // For test
{
	asm ("      ESTOP0");
	for(;;);
}

//===========================================================================
// End of file.
//===========================================================================
