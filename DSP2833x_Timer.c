
#include "DSP2833x_Device.h"     	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   	// DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"

#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"
#include "Ctrl.h"
#include "Timer.h"

//------------------------------------------------------------------------------------------
void timerIsr(void)
{
	timeBase++;
}

//------------------------------------------------------------------------------------------
void timerClr(Timer *pTimer)
{
	pTimer->wTick = 0;
	pTimer->wCount = 0;

	timeBase = 0;
}

//------------------------------------------------------------------------------------------
BOOL timerCheck_mSec(Timer *pTimer, unsigned long wLimit)
{
	pTimer->wCount = timeBase;
	if(pTimer->wCount >= wLimit) 
	{	
		pTimer->wCount = 0;
		return 1;				
	}
	return 0;
}

//------------------------------------------------------------------------------------------
BOOL timerCheck_Sec(Timer *pTimer, unsigned long wLimit)
{
	pTimer->wCount = timeBase;
	if((pTimer->wCount / 1000) >= wLimit)
	{	
		pTimer->wCount = 0;
		return 1;				
	}
	return 0;
}





