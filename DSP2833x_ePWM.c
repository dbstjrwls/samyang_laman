
#include "DSP2833x_28335_Define.h"
#include "DSP2833x_Device.h"     	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   	// DSP2833x Examples Include File
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"

void Init_PWM1(void);
void Init_PWM2(void);
void Init_PWM3(void);
void Init_PWM4(void);
void Init_ON_PWM(void);
void Init_CONV_PWM(void);

WORD PWM;

void Init_ePwm(void)
{
	// Stop all the TB clocks
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	Init_PWM1();
	Init_PWM2();
	Init_PWM3();
	Init_PWM4();
	Init_ON_PWM();
	Init_CONV_PWM();
	
	// Start all the timers synced
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;
	PieVectTable.EPWM1_INT = &EPWM1_INT_ISR;
	PieVectTable.EPWM2_INT = &EPWM2_INT_ISR;
	PieVectTable.EPWM3_INT = &EPWM3_INT_ISR;
	PieVectTable.EPWM4_INT = &EPWM4_INT_ISR;
	PieVectTable.EPWM5_INT = &EPWM5_INT_ISR;
	PieVectTable.EPWM6_INT = &EPWM6_INT_ISR;
    EDIS;

	// Enable CPU INT3 which is connected to EPWM1~6 INT
	IER |= M_INT3;

	// Enable EPWM INTn in the PIE: Group 3 interrupt 1~6
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx4 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx5 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx6 = 1;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_FREEZE; 
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_FREEZE; 
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_FREEZE; 
}

void Init_PWM1(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;	  		// Enable pull-up on GPIO4 (EPWM3A)
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;	  		// Enable pull-up on GPIO5 (EPWM3B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   		// Configure GPIO4 as EPWM3A
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   		// Configure GPIO1 as EPWM3B
	EDIS;

	// Setup TBCLK
	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 	// Count up
	EPwm1Regs.TBPRD = 0x00;        				// Set timer period//
	EPwm1Regs.TBCTL.bit.PHSEN = TB_ENABLE;    	// Disable phase loading
	EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm1Regs.TBCTR = 0x0000;       			// Clear counter
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;   	// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2
	EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       	// Phase is 0

	// Setup shadow register load on ZERO
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm1Regs.CMPA.half.CMPA = 0x00;    // Set compare A value //
	EPwm1Regs.CMPB = 0x00;         		// Set Compare B value //

	// Set actions
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CBU = AQ_CLEAR;
//	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;	
	
	// Set Dead-time
	EPwm1Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm1Regs.DBRED = 0x00;		//
	EPwm1Regs.DBFED = 0x00; 	//
	
	// Disable PWM-chopping function
	EPwm1Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;	// Select INT on CTR = PRD
	EPwm1Regs.ETSEL.bit.INTEN = 1;            	// Enable INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;       	// Generate INT on 1st event
}

void Init_PWM2(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;	  		// Enable pull-up on GPIO6 (EPWM4A)
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;	  		// Enable pull-up on GPIO7 (EPWM4B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   		// Configure GPIO6 as EPWM4A
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   		// Configure GPIO7 as EPWM4B
	EDIS;

	// Setup TBCLK
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 	// Count up
	EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;    	// enable phase loading
	EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   	// Clock ratio to SYSCLKOUT/2
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;   	// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2
	EPwm2Regs.TBPRD = 0x00;        				// Set timer period	 //
	EPwm2Regs.TBCTR = 0x0000;       			// Clear counter
	EPwm2Regs.TBPHS.half.TBPHS = 0x00; 			// Phase is 0 //

	// Setup shadow register load on ZERO
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm2Regs.CMPA.half.CMPA = 0x00;	// Set compare A value //
	EPwm2Regs.CMPB = 0x00;    			// Set Compare B value //

	// Set actions
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;	
	EPwm2Regs.AQCTLA.bit.CBU = AQ_CLEAR;	
//	EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
	
	// Set Dead-time
	EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm2Regs.DBRED = 0x00;		//
	EPwm2Regs.DBFED = 0x00;		//

	
	// Disable PWM-chopping function
	EPwm2Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;	// Select INT on ctr = prd
	EPwm2Regs.ETSEL.bit.INTEN = 1;            	// Enable INT
	EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;       	// Generate INT on 1st event
}

void Init_PWM3(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;	  		// Enable pull-up on GPIO4 (EPWM3A)
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;	  		// Enable pull-up on GPIO5 (EPWM3B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   		// Configure GPIO4 as EPWM3A
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   		// Configure GPIO1 as EPWM3B
	EDIS;

	// Setup TBCLK
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 	// Count up
	EPwm3Regs.TBPRD = 0x00;        				// Set timer period //
	EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;    	// Disable phase loading
	EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm3Regs.TBCTR = 0x0000;       			// Clear counter
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   	// Clock ratio to SYSCLKOUT/2
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV2;   	// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2
	EPwm3Regs.TBPHS.half.TBPHS = 0x00;			//
//	EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       	// Phase is 0

	// Setup shadow register load on ZERO
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm3Regs.CMPA.half.CMPA = 0x00;    // Set compare A value //
	EPwm3Regs.CMPB = 0x00;         		// Set Compare B value //

	// Set actions
	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CBU = AQ_CLEAR;	
//	EPwm3Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
	
	// Set Dead-time
	EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm3Regs.DBRED = 0x00;		//
	EPwm3Regs.DBFED = 0x00;		//
	
	// Disable PWM-chopping function
	EPwm3Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  	// Select INT on CTR = PRD
	EPwm3Regs.ETSEL.bit.INTEN = 1;            	// Enable INT
	EPwm3Regs.ETPS.bit.INTPRD = ET_1ST;       	// Generate INT on 1st event
}

void Init_PWM4(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;	  			// Enable pull-up on GPIO6 (EPWM4A)
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;	  			// Enable pull-up on GPIO7 (EPWM4B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   			// Configure GPIO6 as EPWM4A
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   			// Configure GPIO7 as EPWM4B
	EDIS;

	// Setup TBCLK
	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 		// Count up
	EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;    		// enable phase loading
	EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   		// Clock ratio to SYSCLKOUT/2
	EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV2;   		// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2
	EPwm4Regs.TBPRD = 0x00;        					// Set timer period //
	EPwm4Regs.TBCTR = 0x0000;       				// Clear counter
	EPwm4Regs.TBPHS.half.TBPHS = 0x00;				//
//	EPwm4Regs.TBPHS.half.TBPHS = 0x000a;      		// Phase is 0

	// Setup shadow register load on ZERO
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm4Regs.CMPA.half.CMPA = 0x00;    // Set compare A value //
	EPwm4Regs.CMPB = 0x00;         		// Set Compare B value //

	// Set actions
	EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;	
	EPwm4Regs.AQCTLA.bit.CBU = AQ_CLEAR;
//	EPwm4Regs.AQCTLA.bit.ZRO = AQ_CLEAR;
	
	// Set Dead-time
	EPwm4Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm4Regs.DBRED = 0x00;	//
	EPwm4Regs.DBFED = 0x00;	//

	// Disable PWM-chopping function
	EPwm4Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  // Select INT on ctr = prd
	EPwm4Regs.ETSEL.bit.INTEN = 1;             // Enable INT
	EPwm4Regs.ETPS.bit.INTPRD = ET_1ST;        // Generate INT on 1st event
}

void Init_ON_PWM(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO8 = 0;	  	// Enable pull-up on GPIO8 (EPWM5A)
	GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;	  	// Enable pull-up on GPIO9 (EPWM5B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;   	// Configure GPIO8 as EPWM5A
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;   	// Configure GPIO5 as EPWM5B
	EDIS;

	// Setup TBCLK
	EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 		// Count up
	EPwm5Regs.TBPRD = 0x00;         				// Set timer period
	EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;   		// Disable phase loading
	EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm5Regs.TBCTR = 0x0000;       				// Clear counter
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   		// Clock ratio to SYSCLKOUT/2
	EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV2;   		// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2

	// Setup shadow register load on ZERO
	EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm5Regs.CMPA.half.CMPA = 0;    		// Set compare A value

	// Set actions
	EPwm5Regs.AQCTLA.bit.PRD = AQ_SET;		// Clear PWM1A on event A, up count
	EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // Set PWM1A on Zero
	EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;      // Set PWM1A on Zero
//	EPwm5Regs.AQCTLA.bit.ZRO = AQ_CLEAR;    // Set PWM1A on Zero
//	EPwm5Regs.AQCTLB.bit.ZRO = ;      		// Set PWM1A on Zero
//	EPwm5Regs.AQCTLB.bit.CAU = ;			// Clear PWM1A on event A, up count

	// Set Dead-time
	EPwm5Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm5Regs.DBRED = 0x00; 	//
	EPwm5Regs.DBFED = 0x00; 	//

	// Disable PWM-chopping function
	EPwm5Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;	// Select INT on CTR = PRD
	EPwm5Regs.ETSEL.bit.INTEN = 1;            	// Enable INT
	EPwm5Regs.ETPS.bit.INTPRD = ET_1ST;       	// Generate INT on 1st event
}

void Init_CONV_PWM(void)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	EALLOW;
	// Initialize GPIO for ePWM1 output pin
	// Enable internal pull-up for the selected pins
	GpioCtrlRegs.GPAPUD.bit.GPIO10 = 0;	  		// Enable pull-up on GPIO10 (EPWM6A)
	GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;	  		// Enable pull-up on GPIO11 (EPWM6B)
    
	// Configure GPIO regs for ePWM1 output
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;   		// Configure GPIO10 as EPWM6A
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;   		// Configure GPIO11 as EPWM6B
	EDIS;

	// Setup TBCLK
	EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; 		// Count up
	EPwm6Regs.TBPRD = 0x00;        					// Set timer period//
	EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;   		// Disable phase loading
	EPwm6Regs.TBCTL.bit.PRDLD = TB_SHADOW;
	EPwm6Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm6Regs.TBCTR = 0x0000;       				// Clear counter
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   		// Clock ratio to SYSCLKOUT/2
	EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV2;   		// Clock ratio to TBCLK = SYSCLKOUT / (HSPCLKDIV * CLKDIV) = SYSCLKOUT / 2

	// Setup shadow register load on ZERO
	EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm6Regs.CMPA.half.CMPA = 10;	// Set compare A value
	EPwm6Regs.CMPB = 10;           	// Set Compare B value

	// Set actions
	EPwm6Regs.AQCTLA.bit.PRD = AQ_SET;		// Clear PWM1A on event A, up count
	EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR;    // Set PWM1A on Zero
	EPwm6Regs.AQCTLA.bit.ZRO = AQ_SET;      // Set PWM1A on Zero
//	EPwm6Regs.AQCTLB.bit.PRD = AQ_SET;		// Clear PWM1A on event A, up count
//	EPwm6Regs.AQCTLB.bit.ZRO = AQ_CLEAR;    // Set PWM1A on Zero
//	EPwm6Regs.AQCTLB.bit.CAU = AQ_SET;		// Clear PWM1A on event A, up count

	// Set Dead-time
	EPwm6Regs.DBCTL.bit.IN_MODE = DBA_ALL;
	EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
	EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
	EPwm6Regs.DBRED	= 0x00;		//
	EPwm6Regs.DBFED = 0x00;		//

	// Disable PWM-chopping function
	EPwm6Regs.PCCTL.bit.CHPEN = 0;
	 
	// Interrupt where we will change the Compare Values
	EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;   // Select INT on ctr = prd
	EPwm6Regs.ETSEL.bit.INTEN = 1;             // Enable INT
	EPwm6Regs.ETPS.bit.INTPRD = ET_1ST;        // Generate INT on 1st event

// 	ADC interrupt

//	EPwm6Regs.ETSEL.bit.SOCAEN = 0x1;   // Select INT on ctr = prd
//	EPwm6Regs.ETSEL.bit.SOCASEL = 0x1;	// Select INT on ctr = prd
//
//	EPwm6Regs.ETFRC.bit.SOCA = 0x1;     // Select INT on ctr = prd

//	EPwm6Regs.ETPS.bit.SOCAPRD = 0x1;   // Select INT on ctr = prd
//	EPwm6Regs.ETPS.bit.SOCACNT = 0x3;   // Select INT on ctr = prd

//	EPwm6Regs.ETSEL.bit.SOCBEN = 0x1;   // Select INT on ctr = prd
//	EPwm6Regs.ETSEL.bit.SOCBSEL = 0x5;  // Select INT on ctr = prd

//	EPwm6Regs.ETFRC.bit.SOCB = 0x1;     // Select INT on ctr = prd

//	EPwm6Regs.ETPS.bit.SOCBPRD = 0x1;   // Select INT on ctr = prd
//	EPwm6Regs.ETPS.bit.SOCBCNT = 0x1;   // Select INT on ctr = prd
}
