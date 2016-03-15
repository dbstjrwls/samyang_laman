
#include "DSP2833x_28335_Define.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped to a load and
// run address using the linker cmd file.

#define STATUS_FAIL          0
#define STATUS_SUCCESS       1

//---------------------------------------------------------------------------
// InitSysCtrl:
//---------------------------------------------------------------------------
// This function initializes the System Control registers to a known state.
// - Disables the watchdog
// - Set the PLLCR for proper SYSCLKOUT frequency
// - Set the pre-scaler for the high and low frequency peripheral clocks
// - Enable the clocks to the peripherals

void InitSysCtrl(void)
{
   DisableDog();   				// Disable the watchdog

   // Initialize the PLL control: PLLCR and DIVSEL
   // DSP28_PLLCR and DSP28_DIVSEL are defined in DSP2833x_Examples.h
   InitPll(DSP28_PLLCR,DSP28_DIVSEL);
   InitPeripheralClocks();   	// Initialize the peripheral clocks
}

//---------------------------------------------------------------------------
// Example: ServiceDog:
//---------------------------------------------------------------------------
// This function resets the watchdog timer.
// Enable this function for using ServiceDog in the application
void ServiceDog(void)
{
    EALLOW;
	SysCtrlRegs.WDKEY = 0x0055;
  	SysCtrlRegs.WDKEY = 0x00AA;
	//SysCtrlRegs.WDCR = 0x0080;
    EDIS;
}

void ServiceDog_immediately(void)
{
    EALLOW;
	SysCtrlRegs.WDKEY = 0x0055;
	SysCtrlRegs.WDKEY = 0x00AA;
	SysCtrlRegs.WDKEY = 0x0032;
  	SysCtrlRegs.WDCR  = 0x0000;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: DisableDog:
//---------------------------------------------------------------------------
// This function disables the watchdog timer.
void DisableDog(void)
{
    EALLOW;
    SysCtrlRegs.WDCR= 0x0068;
    EDIS;
}

//---------------------------------------------------------------------------
// Example: InitPll:
//---------------------------------------------------------------------------
// This function initializes the PLLCR register.
void InitPll(Uint16 val, Uint16 divsel)
{
   // Make sure the PLL is not running in limp mode
   if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 0)
   {
      // Missing external clock has been detected
      // Replace this line with a call to an appropriate
      // SystemShutdown(); function.
      asm("        ESTOP0");
   }

   // DIVSEL MUST be 0 before PLLCR can be changed from
   // 0x0000. It is set to 0 by an external reset XRSn
   // This puts us in 1/4
   if (SysCtrlRegs.PLLSTS.bit.DIVSEL != 0)
   {
       EALLOW;
       SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
       EDIS;
   }

   // Change the PLLCR
   if (SysCtrlRegs.PLLCR.bit.DIV != val)
   {

      EALLOW;
      // Before setting PLLCR turn off missing clock detect logic
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
      SysCtrlRegs.PLLCR.bit.DIV = val;
      EDIS;

      // Optional: Wait for PLL to lock.
      // During this time the CPU will switch to OSCCLK/2 until
      // the PLL is stable.  Once the PLL is stable the CPU will
      // switch to the new PLL value.
      //
      // This time-to-lock is monitored by a PLL lock counter.
      //
      // Code is not required to sit and wait for the PLL to lock.
      // However, if the code does anything that is timing critical,
      // and requires the correct clock be locked, then it is best to
      // wait until this switching has completed.

      // Wait for the PLL lock bit to be set.

      // The watchdog should be disabled before this loop, or fed within
      // the loop via ServiceDog().

	  // Uncomment to disable the watchdog
      DisableDog();

      while(SysCtrlRegs.PLLSTS.bit.PLLLOCKS != 1)
      {
	      // Uncomment to service the watchdog
		  // ServiceDog();
      }

      EALLOW;
      SysCtrlRegs.PLLSTS.bit.MCLKOFF = 0;
      EDIS;
    }

    // If switching to 1/2
	if((divsel == 1)||(divsel == 2))
	{
		EALLOW;
	    SysCtrlRegs.PLLSTS.bit.DIVSEL = divsel;
	    EDIS;
	}

	// If switching to 1/1
	// * First go to 1/2 and let the power settle
	//   The time required will depend on the system, this is only an example
	// * Then switch to 1/1
	if(divsel == 3)
	{
		EALLOW;
	    SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
	    DELAY_US(50L);
	    SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;
	    EDIS;
    }
}

//--------------------------------------------------------------------------
// Example: InitPeripheralClocks:
//---------------------------------------------------------------------------
// This function initializes the clocks to the peripheral modules.
// First the high and low clock prescalers are set
// Second the clocks are enabled to each peripheral.
// To reduce power, leave clocks to unused peripherals disabled
//
// Note: If a peripherals clock is not enabled then you cannot
// read or write to the registers for that peripheral

void InitPeripheralClocks(void)
{
   EALLOW;
// HISPCP/LOSPCP prescale register settings, normally it will be set to default values
   SysCtrlRegs.HISPCP.all = 0x0001;
   SysCtrlRegs.LOSPCP.all = 0x0002;

// XCLKOUT to SYSCLKOUT ratio.  By default XCLKOUT = 1/4 SYSCLKOUT
   XintfRegs.XINTCNF2.bit.XTIMCLK = 1;	// XTIMCLK = SYSCLKOUT/2
   XintfRegs.XINTCNF2.bit.CLKMODE = 1;  // XCLKOUT = XTIMCLK/2
   XintfRegs.XINTCNF2.bit.CLKOFF = 0;  	// Enable XCLKOUT
	// Peripheral clock enables set for the selected peripherals.
	// If you are not using a peripheral leave the clock off
	// to save on power.
	// Note: not all peripherals are available on all 2833x derivates.
	// Refer to the datasheet for your particular device.
	// This function is not written to be an example of efficient code.
   SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;	// ADC

   // *IMPORTANT*
   // The ADC_cal function, which  copies the ADC calibration values from TI reserved
   // OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
   // Boot ROM. If the boot ROM code is bypassed during the debug process, the
   // following function MUST be called for the ADC to function according
   // to specification. The clocks to the ADC MUST be enabled before calling this
   // function.
   // See the device data manual and/or the ADC Reference
   // Manual for more information.

   ADC_cal();

   SysCtrlRegs.PCLKCR0.bit.I2CAENCLK = 1;   // I2C
   SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;   // SCI-A
   SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;   // SCI-B
   SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 1;   // SCI-C
   SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;   // SPI-A
   SysCtrlRegs.PCLKCR0.bit.MCBSPAENCLK = 1; // McBSP-A
   SysCtrlRegs.PCLKCR0.bit.MCBSPBENCLK = 1; // McBSP-B
   SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 1;  // eCAN-A
   SysCtrlRegs.PCLKCR0.bit.ECANBENCLK = 1;  // eCAN-B

   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;   // Disable TBCLK within the ePWM
   SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;  // ePWM1
   SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;  // ePWM2
   SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;  // ePWM3
   SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  // ePWM4
   SysCtrlRegs.PCLKCR1.bit.EPWM5ENCLK = 1;  // ePWM5
   SysCtrlRegs.PCLKCR1.bit.EPWM6ENCLK = 1;  // ePWM6
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;   // Enable TBCLK within the ePWM
/*
   SysCtrlRegs.PCLKCR1.bit.ECAP3ENCLK = 1;  // eCAP3
   SysCtrlRegs.PCLKCR1.bit.ECAP4ENCLK = 1;  // eCAP4
   SysCtrlRegs.PCLKCR1.bit.ECAP5ENCLK = 1;  // eCAP5
   SysCtrlRegs.PCLKCR1.bit.ECAP6ENCLK = 1;  // eCAP6
   SysCtrlRegs.PCLKCR1.bit.ECAP1ENCLK = 1;  // eCAP1
   SysCtrlRegs.PCLKCR1.bit.ECAP2ENCLK = 1;  // eCAP2
   SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 1;  // eQEP1
   SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 1;  // eQEP2
*/
   SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
   SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1; // CPU Timer 1
   SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // CPU Timer 2

   //SysCtrlRegs.PCLKCR3.bit.DMAENCLK = 1;     // DMA Clock
   SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;     // XTIMCLK
   SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;    // GPIO input clock

   EDIS;
}

// Initialize ADC operation mode
void Init_Adc_Mode(void)
{
	// Initialize ADC Power-up sequence
	// *IMPORTANT*
	// The ADC_cal function, which  copies the ADC calibration values from TI reserved
	// OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs automatically in the
	// Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.
	
	// 아래 주석 처리한 부분은 LDC_SysCtrl.c 파일에서 InitPeripheralClocks() 함수를 통해 실행됨 
	/*
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;
	*/

	AdcRegs.ADCTRL3.all = 0x00E0; // Power up bandgap/reference/ADC circuits
	DELAY_US(5000L);         	  // Delay 5ms before converting ADC channels

	// Set ADC core clock prescaler
	// 12.5MHz = HSPCLK/((ADCTRL3.ADCCLKPS * 2)*(ADCTRL1.bit.CPS + 1)) = (150MHz/2)/6
	//            75           3                      0                  = 75 / (3*2)
	// 12.5 / (15+1) = 781.250 kHZ conversion rate

	// 75MHz / (15 * 2) * (2) = 75 / 60 = 1.25MHz
	// 1.25 MHz / (15+1) = 78.125 KHz
//	AdcRegs.ADCTRL1.bit.CPS = 1;
//	AdcRegs.ADCTRL3.bit.ADCCLKPS = 15;

	// 75MHz / (3 * 2) * (1) = 75 / 6 = 12.5MHz
	// 12.5 MHz / (15+1) = 781.250 KHz
	AdcRegs.ADCTRL1.bit.CPS = 0;
	AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;

	// 2010.10.5
	// 75MHz / (15 * 2) * (1) = 75 / 30 = 2.5MHz
	// 2.5 MHz / (15+1) = 156.25 KHz
//	AdcRegs.ADCTRL1.bit.CPS = 0;
//	AdcRegs.ADCTRL3.bit.ADCCLKPS = 0xf;

	AdcRegs.ADCTRL1.bit.ACQ_PS = 0xf; // Acquisition window size
	
	// Set internal reference selection
	// F28335의 내부 ADC Reference를 사용하지 않고 외부 ADC Reference를 쓰는 경우에는
	// 아래의 값을 사용하는 Referecne Voltage에 따라 설정하십시요. 
//	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x01; 	// 외부 ADC Referecne(2.048 V) 사용
	AdcRegs.ADCREFSEL.bit.REF_SEL = 0x00; 	// 내부 ADC Referecne 사용	
	// Setup 1 conv's on SEQ1
	AdcRegs.ADCMAXCONV.all = 0xf; 			// Autoconversion mode, 16 channel ADC
	
	// Set cascaded mode
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;  		// Sequencer override Enable
	
	// Select start-stop mode
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;  		// 0 : SOC signal Start, 1: Continuous Conversion


	// Set ADC input channel selection sequence	
	AdcRegs.ADCCHSELSEQ1.all = 0x3210; 	// ADC IN 3 ~ 0 -> CONV 3 ~ 0
	AdcRegs.ADCCHSELSEQ2.all = 0x7654; 	// ADC IN 7 ~ 4 -> CONV 7 ~ 4
	AdcRegs.ADCCHSELSEQ3.all = 0xBA98; 	// ADC IN 11 ~ 8 -> CONV 11 ~ 8
	AdcRegs.ADCCHSELSEQ4.all = 0xFEDC; 	// ADC IN 15 ~ 12 -> CONV 15 ~ 12

	// Enable SEQ1 interrupt (every EOS)
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;
	// ADD  by ngsw 2010.10.5
//	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; // 		
	
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;
	PieVectTable.ADCINT = &ADCINT_ISR;
	EDIS;

	// Enable ADCINT in PIE
	IER |= M_INT1; // Enable CPU Interrupt 1

	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
	
	// SOC trigger for sequencer1 by software
	AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}

void Init_Gpio(void)
{
	EALLOW;
	// PORTA(GPIO0 ~ 31), GPAMUX1(GPIO0 ~ GPIO31)
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;		// ENA_15V 	   (OUT)
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;		// TP 		   (OUT)
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;		// ENA_10V 	   (OUT)
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;		// TP 		   (OUT)
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;		// TP 		   (OUT)
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;	// TRIG_SYNC   (IN)
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0; 	// INT_LOCK    (IN)
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;	// PWR_STATUS  (IN)
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0; 	// OPTO_COLT   (IN)
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;	// FIRE_ICCD   (IN)
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0; 	// FIRE_EMCCD  (IN)

	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0; 	// NC
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0;	// NC
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;	// NC
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;	// NC
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;	// NC
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;	// I2C_ROM_WP (OUT)
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0;

	// PORTB(GPIO32 ~ 63), GPBMUX1(GPIO32 ~ GPIO47)
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;	// OPTO_ANOD (OUT)
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 0;	// (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;	// (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 0;	// (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0;	// (OUT)

	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0;	// DLY_7     (OUT) 
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0;	// DLY_6     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;	// DLY_5     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0;	// DLY_4     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0;	// DLY_3     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0;	// DLY_2     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0;	// DLY_1     (OUT)
	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0;	// DLY_0     (OUT)

	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;	// D_CM_TRIG (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;	// D_MOD_SEL (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;	// D_SOL_V1  (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;	// D_SOL_V3  (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;	// D_RST_EX5 (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;	// D_F_INJECT(OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0;	// D_SOL_V4  (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0;	// D_SOL_V2  (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0;	// D_FAN     (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;	// D_MTR2    (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;	// D_VAC_PUMP(OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;	// D_MTR1    (OUT)
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;	// D_CLRn    (OUT)

	// PORTC(GPIO64 ~ 87), GPCMUX1(GPIO64 ~ GPIO79)
	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;	// D_PEAK_RESET (OUT)	
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;	// NC	
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;	// D_PEAK_SYNC	(IN)
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;	// NC
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 0;	// NC	
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 0;	// NC

	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 0;	// D_TRIG_EX5	(OUT)
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 0;	// D_EXT_TRIG	(OUT)
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 0;	// D_TRIG_ICCD	(OUT)
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 0;	// D_TRIG_EMCCD	(OUT)
	//-------------------------------------------------------------------------------
	// 1 : OUT, 0 : IN
	GpioCtrlRegs.GPADIR.bit.GPIO0  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO1  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO2  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO3  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO4  = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;

	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO35 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO36 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO37 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO38 = 1;

	GpioCtrlRegs.GPBDIR.bit.GPIO40 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO41 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO43 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;

	GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO53 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO55 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;

	GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO66 = 0;
	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO69 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO71 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO76 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO77 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO78 = 1;
	GpioCtrlRegs.GPCDIR.bit.GPIO79 = 1;

	//------------------------------------------------------------------------------
	// Set Input Qualification(3 Sample, 255 sampling period)
	// Input Qualification Time : 2 x 2 x 255 x 6.67ns = 6.8 us

/* 
	GpioCtrlRegs.GPAQSEL1.bit.GPIO2 = 1;	// 3 Sample
	GpioCtrlRegs.GPBQSEL1.bit.GPIO41 = 1;
*/	
	
	//Remark, 15-11-30
	GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 1;

	// GPAQSEL1(0~15), GPAQSEL2(16~32)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO10 = 1;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 1;
	GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 1;

	// Set the sampling period for GPIO
	// GPIO0~7   : GPACTRL[QUALPRD0]
	// GPIO8~15  : GPACTRL[QUALPRD1]
	// GPIO16~23 : GPACTRL[QUALPRD2]
	// GPIO24~32 : GPACTRL[QUALPRD3]
	GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0x10;	//0x10;
	GpioCtrlRegs.GPACTRL.bit.QUALPRD3 = 0x10;	//0x10;
	
/*
	GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0x10; 
	GpioCtrlRegs.GPBCTRL.bit.QUALPRD1 = 0x10;

	GpioCtrlRegs.GPAPUD.bit.GPIO2  = 0;		// Enable Pull-up
	GpioCtrlRegs.GPBPUD.bit.GPIO41 = 0;		// Enable Pull-up
*/
	EDIS;
}

void Init_External_Interrupt(void)
{
	// Set GPIO Interrupt Select
	EALLOW; 
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 24;		// 도면상 GPIO66으로 되어 있지만, GPIO66은 Interrupt 기능이 없는 pin이므로, GPIO24로 jumper를 날려서 사용.
	GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = 10;

	PieVectTable.XINT1 = &XINT1_ISR;
	PieVectTable.XINT2 = &XINT2_ISR;
	EDIS;

	// Set XINT Control Register
	XIntruptRegs.XINT1CR.bit.POLARITY = 1;	// 1: Rising Edge, 0: Falling Edge
	XIntruptRegs.XINT2CR.bit.POLARITY = 1;	// 1: Rising Edge, 0: Falling Edge

	XIntruptRegs.XINT1CR.bit.ENABLE = 1;	// Enable External Interrupt  1
	XIntruptRegs.XINT2CR.bit.ENABLE = 1;	// Enable External Interrupt  1

	IER |= M_INT1;							// Enable CPU int which is connected to XINT1
	IER |= M_INT2;							// Enable CPU int which is connected to XINT2

	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;		// Enable XINT1 in the PIE: Group 1 interrupt 4
	PieCtrlRegs.PIEIER1.bit.INTx5 = 1;		// Enable XINT1 in the PIE: Group 1 interrupt 5
}

/*
void Init_External_Interrupt(void)
{
	// Set GPIO Interrupt Select
	EALLOW; 
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 24;
	PieVectTable.XINT1 = &XINT1_ISR;
	EDIS;

	// Set XINT Control Register(Falling Edge)
	XIntruptRegs.XINT1CR.bit.POLARITY = 0;  // Falling Edge
	XIntruptRegs.XINT1CR.bit.ENABLE = 1;	// Enable External Interrupt  1
	IER |= M_INT1;							// Enable CPU int which is connected to XINT1
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;		// Enable XINT1 in the PIE: Group 1 interrupt 1
}
*/

/*
void Init_External_Interrupt(void)
{
	// Set GPIO Interrupt Select
	EALLOW;
	GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 22; // 54 - 32
	GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 23; // 55 - 32
	GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 24; // 56 - 32;
	GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 25; // 57 - 32;

	PieVectTable.XINT3 = &XINT3_ISR;
	PieVectTable.XINT4 = &XINT4_ISR;
	PieVectTable.XINT5 = &XINT5_ISR;
	PieVectTable.XINT6 = &XINT6_ISR;
	EDIS;

	// Set XINT Control Register(Falling Edge)
	XIntruptRegs.XINT3CR.bit.POLARITY = 0;
	XIntruptRegs.XINT4CR.bit.POLARITY = 0;
	XIntruptRegs.XINT5CR.bit.POLARITY = 0;
	XIntruptRegs.XINT6CR.bit.POLARITY = 0;

	// Enable External Interrupt
	XIntruptRegs.XINT3CR.bit.ENABLE = 1;
	XIntruptRegs.XINT4CR.bit.ENABLE = 1;
	XIntruptRegs.XINT5CR.bit.ENABLE = 1;
	XIntruptRegs.XINT6CR.bit.ENABLE = 1;

	IER |= M_INT12;	// Enable CPU int12 which is connected to XINT3/4/5/6

	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;	// Enable XINT3 in the PIE: Group 12 interrupt 1
	PieCtrlRegs.PIEIER12.bit.INTx2 = 1;	// Enable XINT4 in the PIE: Group 12 interrupt 2
	PieCtrlRegs.PIEIER12.bit.INTx3 = 1;	// Enable XINT5 in the PIE: Group 12 interrupt 3
	PieCtrlRegs.PIEIER12.bit.INTx4 = 1;	// Enable XINT6 in the PIE: Group 12 interrupt 4
}
*/

//---------------------------------------------------------------------------
// EnableInterrupts: 
//---------------------------------------------------------------------------
// This function enables the PIE module and CPU interrupts
void EnableInterrupts()
{
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;    	// Enable the PIE
	PieCtrlRegs.PIEACK.all = 0xFFFF;  		// Enables PIE to drive a pulse into the CPU 
    EINT;									// Enable Interrupts at the CPU level 
}

//---------------------------------------------------------------------------
// Example: CsmUnlock:
//---------------------------------------------------------------------------
// This function unlocks the CSM. User must replace 0xFFFF's with current
// password for the DSP. Returns 1 if unlock is successful.

/*
Uint16 CsmUnlock()
{
    volatile Uint16 temp;

    // Load the key registers with the current password. The 0xFFFF's are dummy
	// passwords.  User should replace them with the correct password for the DSP.

    EALLOW;
    CsmRegs.KEY0 = 0xFFFF;
    CsmRegs.KEY1 = 0xFFFF;
    CsmRegs.KEY2 = 0xFFFF;
    CsmRegs.KEY3 = 0xFFFF;
    CsmRegs.KEY4 = 0xFFFF;
    CsmRegs.KEY5 = 0xFFFF;
    CsmRegs.KEY6 = 0xFFFF;
    CsmRegs.KEY7 = 0xFFFF;
    EDIS;

    // Perform a dummy read of the password locations
    // if they match the key values, the CSM will unlock

    temp = CsmPwl.PSWD0;
    temp = CsmPwl.PSWD1;
    temp = CsmPwl.PSWD2;
    temp = CsmPwl.PSWD3;
    temp = CsmPwl.PSWD4;
    temp = CsmPwl.PSWD5;
    temp = CsmPwl.PSWD6;
    temp = CsmPwl.PSWD7;

    // If the CSM unlocked, return succes, otherwise return
    // failure.
    if (CsmRegs.CSMSCR.bit.SECURE == 0) return STATUS_SUCCESS;
    else return STATUS_FAIL;
}
*/

//-------------------------------------------------------------------------
// TJ
void Init_SCI_UART_A(void)
{
    // Note: Clocks were turned on to the SCI-A peripheral
    // in the InitSysCtrl() function
    SciaRegs.SCIFFTX.all = 0x8000;		// FIFO reset
 	SciaRegs.SCIFFCT.all = 0x4000;		// Clear ABD(Auto baud bit)
 	SciaRegs.SCICCR.all = 0x0007;  		// 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol
	SciaRegs.SCICTL1.all = 0x0003; 		// enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE

	SciaRegs.SCICTL2.bit.RXBKINTENA = 1;// RX/BK INT ENA=1,
//	SciaRegs.SCICTL2.bit.TXINTENA = 1;	// TX INT ENA=1,

    SciaRegs.SCIHBAUD = BRR_VAL >> 8;	// High Value
    SciaRegs.SCILBAUD = BRR_VAL & 0xff;	// Low Value
	SciaRegs.SCICTL1.all = 0x0023; 		// Relinquish SCI from Reset  
    
	// User specific functions, Reassign vectors (optional), Enable Interrupts:
    // Initialize SCI_A RX interrupt
    // Reassign SCI_A RX ISR. 
    // Reassign the PIE vector for RXAINT to point to a different ISR then
    // the shell routine found in DSP28_DefaultIsr.c.
    // This is done if the user does not want to use the shell ISR routine
    // but instead wants to use their own ISR.  This step is optional:
    
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.SCIRXINTA = &SCIRXINTA_ISR;
 //   PieVectTable.SCITXINTA = &SCITXINTA_ISR;
   
    /* Enable internal pull-up for the selected pins */
	// Pull-ups can be enabled or disabled disabled by the user.  
	// This will enable the pullups for the specified pins.
	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0; // Enable pull-up for GPIO28 (SCIRXDA)
	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	// Enable pull-up for GPIO29 (SCITXDA)

	/* Set qualification for selected pins to asynch only */
	// Inputs are synchronized to SYSCLKOUT by default.  
	// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)

	/* Configure SCI-A pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be SCI functional pins.
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
	EDIS;       // This is needed to disable write to EALLOW protected registers

    // Enable CPU INT9 for SCI-A
	IER |= M_INT9;
	
    // Enable SCI-A RX INT in the PIE: Group 9 interrupt 1
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;

    // Enable SCI-A TX INT in the PIE: Group 9 interrupt 2
	PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
}

//-------------------------------------------------------------------------
void Init_SCI_UART_B(void)
{
    // Note: Clocks were turned on to the SCI-B peripheral
    // in the InitSysCtrl() function
    ScibRegs.SCIFFTX.all = 0x8000;		// FIFO reset
 	ScibRegs.SCIFFCT.all = 0x4000;		// Clear ABD(Auto baud bit)
 	ScibRegs.SCICCR.all = 0x0007;  		// 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol
	ScibRegs.SCICTL1.all = 0x0003; 		// enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE
	ScibRegs.SCICTL2.bit.RXBKINTENA = 1;// RX/BK INT ENA=1,
//	ScibRegs.SCICTL2.bit.TXINTENA = 1;	// TX INT ENA=1,

    ScibRegs.SCIHBAUD = BRR_VAL >> 8;	// High Value
    ScibRegs.SCILBAUD = BRR_VAL & 0xff;	// Low Value
	ScibRegs.SCICTL1.all = 0x0023; 		// Relinquish SCI from Reset  
    
	// User specific functions, Reassign vectors (optional), Enable Interrupts:
   // Initialize SCI_A RX interrupt
    // Reassign SCI_A RX ISR. 
    // Reassign the PIE vector for RXAINT to point to a different ISR then
    // the shell routine found in DSP28_DefaultIsr.c.
    // This is done if the user does not want to use the shell ISR routine
    // but instead wants to use their own ISR.  This step is optional:
    
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.SCIRXINTB = &SCIRXINTB_ISR;
//    PieVectTable.SCITXINTB = &SCITXINTB_ISR;
   
    /* Enable internal pull-up for the selected pins */
	// Pull-ups can be enabled or disabled disabled by the user.  
	// This will enable the pullups for the specified pins.
	GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0; // Enable pull-up for GPIO15 (SCIRXDB)
	GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;	// Enable pull-up for GPIO14 (SCITXDB)

	/* Set qualification for selected pins to asynch only */
	// Inputs are synchronized to SYSCLKOUT by default.  
	// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)

	/* Configure SCI-B pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be SCI functional pins.
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation
	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation

//	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;
//	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;

	GpioDataRegs.GPASET.bit.GPIO16 = TRUE;
	GpioDataRegs.GPASET.bit.GPIO17 = TRUE;
	EDIS;       // This is needed to disable write to EALLOW protected registers

    // Enable CPU INT9 for SCI-B
	IER |= M_INT9;
	
    // Enable SCI-B RX INT in the PIE: Group 9 interrupt 3
	PieCtrlRegs.PIEIER9.bit.INTx3 = 1;

    // Enable SCI-B TX INT in the PIE: Group 9 interrupt 4
	PieCtrlRegs.PIEIER9.bit.INTx4 = 1;
}

//-------------------------------------------------------------------------
// Laser-Module(115200)
void Init_SCI_UART_C(void)
{
    // Note: Clocks were turned on to the SCI-B peripheral
    // in the InitSysCtrl() function
    ScicRegs.SCIFFTX.all = 0x8000;		// FIFO reset
 	ScicRegs.SCIFFCT.all = 0x4000;		// Clear ABD(Auto baud bit)
 	ScicRegs.SCICCR.all = 0x0007;  		// 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol
	ScicRegs.SCICTL1.all = 0x0003; 		// enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE
	ScicRegs.SCICTL2.bit.RXBKINTENA = 1;// RX/BK INT ENA=1,
//	ScicRegs.SCICTL2.bit.TXINTENA = 1;	// TX INT ENA=1,

    ScicRegs.SCIHBAUD = BRR_VAL_C >> 8;		// High Value
    ScicRegs.SCILBAUD = BRR_VAL_C & 0xff;	// Low Value
	ScicRegs.SCICTL1.all = 0x0023; 			// Relinquish SCI from Reset  
    
	// User specific functions, Reassign vectors (optional), Enable Interrupts:
   // Initialize SCI_A RX interrupt
    // Reassign SCI_A RX ISR. 
    // Reassign the PIE vector for RXAINT to point to a different ISR then
    // the shell routine found in DSP28_DefaultIsr.c.
    // This is done if the user does not want to use the shell ISR routine
    // but instead wants to use their own ISR.  This step is optional:
    
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.SCIRXINTC = &SCIRXINTC_ISR;
//    PieVectTable.SCITXINTB = &SCITXINTB_ISR;
   
    /* Enable internal pull-up for the selected pins */
	// Pull-ups can be enabled or disabled disabled by the user.  
	// This will enable the pullups for the specified pins.
	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0; // Enable pull-up for GPIO62 (SCIRXDC)
	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;	// Enable pull-up for GPIO63 (SCITXDC)

	/* Set qualification for selected pins to asynch only */
	// Inputs are synchronized to SYSCLKOUT by default.  
	// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

	/* Configure SCI-C pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be SCI functional pins.
	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 for SCIRXDC operation
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 for SCITXDC operation

//	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;
//	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;

	GpioDataRegs.GPBSET.bit.GPIO62 = TRUE;
	GpioDataRegs.GPBSET.bit.GPIO63 = TRUE;
	EDIS;       // This is needed to disable write to EALLOW protected registers

    // Enable CPU INT8 for SCI-C
	IER |= M_INT8;
	
    // Enable SCI-B RX INT in the PIE: Group 8 interrupt 5
	PieCtrlRegs.PIEIER8.bit.INTx5 = 1;

    // Enable SCI-C TX INT in the PIE: Group 8 interrupt 6
	PieCtrlRegs.PIEIER8.bit.INTx6 = 1;
}

//0x100000 -> 0x20000
void Init_Ext_Bus(void)
{
	// Make sure the XINTF clock is enabled
	SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;

	// Configure the GPIO for XINTF with a 16-bit data bus
	// A17 ~ A19 (not Address)
	// not use Ready Signal
	EALLOW;
	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7

	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
	GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16

	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

//	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;  // XREADY//
//	GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XRNW
	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

	GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 3;  // XZCS0
	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6
	
	// All Zones---------------------------------
	// Timing for all zones based on XTIMCLK = SYSCLKOUT
	XintfRegs.XINTCNF2.bit.XTIMCLK = 0;
	// Buffer up to 3 writes
	XintfRegs.XINTCNF2.bit.WRBUFF = 3;
	// XCLKOUT is enabled
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;
	// XCLKOUT = XTIMCLK
	XintfRegs.XINTCNF2.bit.CLKMODE = 0;
	
	// Zone 0
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone 0 write timing
	XintfRegs.XTIMING0.bit.XWRLEAD = 2;
	XintfRegs.XTIMING0.bit.XWRACTIVE = 3;
	XintfRegs.XTIMING0.bit.XWRTRAIL = 2;
	// Zone 0 read timing
	XintfRegs.XTIMING0.bit.XRDLEAD = 2;
	XintfRegs.XTIMING0.bit.XRDACTIVE = 3;
	XintfRegs.XTIMING0.bit.XRDTRAIL = 1;

	// don't double Zone 0 read/write lead/active/trail timing
	XintfRegs.XTIMING0.bit.X2TIMING = 0;

	// Zone 0 will not sample XREADY signal
	XintfRegs.XTIMING0.bit.USEREADY = 0;
	XintfRegs.XTIMING0.bit.READYMODE = 0;

	// Size must be either:
	// 0,1 = x32 or
	// 1,1 = x16 other values are reserved
	XintfRegs.XTIMING0.bit.XSIZE = 3;

	// Zone 6------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING6.bit.XWRLEAD = 3;
    XintfRegs.XTIMING6.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING6.bit.XWRTRAIL = 3;
    // Zone read timing
    XintfRegs.XTIMING6.bit.XRDLEAD = 3;
    XintfRegs.XTIMING6.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING6.bit.XRDTRAIL = 3;

    // double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING6.bit.X2TIMING = 1;

    // Zone will sample XREADY signal
    XintfRegs.XTIMING6.bit.USEREADY = 0;  
    XintfRegs.XTIMING6.bit.READYMODE = 0; 

    // Size must be either:
    // 0,1 = x32 or
    // 1,1 = x16 other values are reserved
    XintfRegs.XTIMING6.bit.XSIZE = 3;

	// Zone 7(Wiznet W3150A+)
	// When using ready, ACTIVE must be 1 or greater
	// Lead must always be 1 or greater
	// Zone 7 write timing
	XintfRegs.XTIMING7.bit.XWRLEAD = 3;
	XintfRegs.XTIMING7.bit.XWRACTIVE = 7;
	XintfRegs.XTIMING7.bit.XWRTRAIL = 2;
	// Zone 7 read timing
	XintfRegs.XTIMING7.bit.XRDLEAD = 3;
	XintfRegs.XTIMING7.bit.XRDACTIVE = 7;
	XintfRegs.XTIMING7.bit.XRDTRAIL = 2;

	// don't double Zone 7 read/write lead/active/trail timing
	XintfRegs.XTIMING7.bit.X2TIMING = 0;

	// Zone 7 will not sample XREADY signal
	XintfRegs.XTIMING7.bit.USEREADY = 0;
	XintfRegs.XTIMING7.bit.READYMODE = 0;

	// 1,1 = x16 data bus
	// 0,1 = x32 data bus
	// other values are reserved
	XintfRegs.XTIMING7.bit.XSIZE = 3;

	// Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles
    // when ever switching from Zone 7 to another Zone.
    // This will help avoid bus contention.
    XintfRegs.XBANK.bit.BANK = 3;
    XintfRegs.XBANK.bit.BCYC = 3;
	EDIS;

	//Force a pipeline flush to ensure that the write to
	//the last register configured occurs before returning.
	asm(" RPT #7 || NOP");
}

//===========================================================================
// End of file.
//===========================================================================
