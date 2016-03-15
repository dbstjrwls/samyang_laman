
#include "DSP2833x_Device.h"     	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   	// DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"

void Set_Digital_Output(BYTE d)
{
	LONG tmp;
	
	tmp = GpioDataRegs.GPADAT.all;
	tmp &= 0xffff0fff;
	tmp |= ((((LONG)(d & 0xff)) << 12) & 0x0000f000);
	GpioDataRegs.GPADAT.all = tmp;
}

void SET_TRIGGER_DLY(BYTE d)
{
	LONG tmp;
	
	tmp = GpioDataRegs.GPBDAT.all;
	tmp &= 0xffff00ff;
	tmp |= ((((LONG)(d & 0xff)) << 8) & 0x0000ff00);
	GpioDataRegs.GPBDAT.all = tmp; 
}

BYTE Get_Digital_Input(void)
{
	LONG tmp;
	BYTE d;
	
	tmp = GpioDataRegs.GPBDAT.all;
	d = (BYTE)((tmp >> 22) & 0xf);

	return d;
}

void BEAM_15V_ON(void)
{
	GpioDataRegs.GPASET.bit.GPIO0 = TRUE;
}

void BEAM_15V_OFF(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO0 = TRUE;
}

void BEAM_10V_ON(void)
{
	GpioDataRegs.GPASET.bit.GPIO2 = TRUE;
}

void BEAM_10V_OFF(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIO2 = TRUE;
}

char TRIGGER_SYNC(void)
{
	BOOL trigger_sync = 0;
	trigger_sync = GpioDataRegs.GPADAT.bit.GPIO10;
	return(trigger_sync);
}

char INT_LOCK_STATUS(void)
{
	BOOL int_lock = 0;
	int_lock = GpioDataRegs.GPADAT.bit.GPIO11;
	return(int_lock);
}

char PWR_STATUS(void)
{
	BOOL pwr_status = 0;
	pwr_status = GpioDataRegs.GPADAT.bit.GPIO12;
	return(pwr_status);
}

char OPTO_COLT_STATUS(void)
{
	BOOL opto_colt = 0;
	opto_colt = GpioDataRegs.GPADAT.bit.GPIO13;
	return(opto_colt);
}

char FIRE_ICCD_STATUS(void)
{
	BOOL fire_iccd = 0;
	fire_iccd = GpioDataRegs.GPADAT.bit.GPIO14;
	return(fire_iccd);
}

char FIRE_EMCCD_STATUS(void)
{
	BOOL fire_emccd = 0;
	fire_emccd = GpioDataRegs.GPADAT.bit.GPIO15;
	return(fire_emccd);
}

//----------------------------------------------------------------------------------------
void CM_TRIG_HIGH(void)
{
	GpioDataRegs.GPBSET.bit.GPIO48 = TRUE;
	rsp_laser_ctrl_line.ls_trigger = 1;
}

void CM_TRIG_LOW(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO48 = TRUE;
	rsp_laser_ctrl_line.ls_trigger = 0;
}

void MODE_SEL_HIGH(void) // Immed
{
	GpioDataRegs.GPBSET.bit.GPIO49 = TRUE;
	rsp_laser_ctrl_line.ls_mode = 1;
}

void MODE_SEL_LOW(void)	 // Command
{
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = TRUE;
	rsp_laser_ctrl_line.ls_mode = 0;
}

void PUMP_VALVE_CLOSE(void)	// SOL_#1, Closed
{
	GpioDataRegs.GPBCLEAR.bit.GPIO50 = TRUE;
	rsp_laser_ctrl_line.ls_sol_1 = 0;
}

void PUMP_VALVE_OPEN(void)	// Open
{
	GpioDataRegs.GPBSET.bit.GPIO50 = TRUE;
	rsp_laser_ctrl_line.ls_sol_1 = 1;
}

void MAIN_VALVE_CLOSE(void)	// SOL_V3
{
	GpioDataRegs.GPBCLEAR.bit.GPIO51 = TRUE;
	rsp_laser_ctrl_line.ls_sol_3 = 0;
}

void MAIN_VALVE_OPEN(void)
{
	GpioDataRegs.GPBSET.bit.GPIO51 = TRUE;
	rsp_laser_ctrl_line.ls_sol_3 = 1;
}

void RST_EX5_ENABLE(void)	// Low (Reset)	       
{
	GpioDataRegs.GPBCLEAR.bit.GPIO52 = TRUE;
	rsp_laser_ctrl_line.ls_reset = 0;
}

void RST_EX5_DISENABLE(void)	// High
{
	GpioDataRegs.GPBSET.bit.GPIO52 = TRUE;
	rsp_laser_ctrl_line.ls_reset = 1;
}

void F_INJECTOR_CLOSE(void)	// Low	       
{
	GpioDataRegs.GPBCLEAR.bit.GPIO53 = TRUE;
	rsp_laser_ctrl_line.ls_flu_injector = 0;
}

void F_INJECTOR_OPEN(void)	// High
{
	GpioDataRegs.GPBSET.bit.GPIO53 = TRUE;
	rsp_laser_ctrl_line.ls_flu_injector = 1;
}

void PURGE_VALVE_CLOSE(void)	// SOL_#4
{
	GpioDataRegs.GPBCLEAR.bit.GPIO54 = TRUE;
	rsp_laser_ctrl_line.ls_sol_4 = 0;
}

void PURGE_VALVE_OPEN(void)
{
	GpioDataRegs.GPBSET.bit.GPIO54 = TRUE;
	rsp_laser_ctrl_line.ls_sol_4 = 1;
}

void FILL_VALVE_CLOSE(void)	// SOL_#2, Low(Closed)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO55 = TRUE;
	rsp_laser_ctrl_line.ls_sol_2 = 0;
}

void FILL_VALVE_OPEN(void) // High(Open)
{
	GpioDataRegs.GPBSET.bit.GPIO55 = TRUE;
	rsp_laser_ctrl_line.ls_sol_2 = 1;
}

void FAN_ON(void)	// Low (ON)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO56 = TRUE;
	rsp_laser_ctrl_line.ls_cool_fan = 0;
}

void FAN_OFF(void)	// High (OFF)
{
	GpioDataRegs.GPBSET.bit.GPIO56 = TRUE;
	rsp_laser_ctrl_line.ls_cool_fan = 1;
}

void MOTOR2_ON(void)
{
	GpioDataRegs.GPBSET.bit.GPIO57 = TRUE;
	rsp_laser_ctrl_line.ls_motor_2 = 1;
}

void MOTOR2_OFF(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO57 = TRUE;
	rsp_laser_ctrl_line.ls_motor_2 = 0;
}

void VACCUM_PUMP_ON(void)
{
	GpioDataRegs.GPBSET.bit.GPIO58 = TRUE;
	rsp_laser_ctrl_line.ls_vac_pump = 1;
}

void VACCUM_PUMP_OFF(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO58 = TRUE;
	rsp_laser_ctrl_line.ls_vac_pump = 0;
}

void MOTOR1_ON(void)	// High
{
	GpioDataRegs.GPBSET.bit.GPIO59 = TRUE;
	rsp_laser_ctrl_line.ls_motor_1 = 1;
}

void MOTOR1_OFF(void)	// Low
{
	GpioDataRegs.GPBCLEAR.bit.GPIO59 = TRUE;
	rsp_laser_ctrl_line.ls_motor_1 = 0;
}

void CLRn_HIGH(void)
{
	GpioDataRegs.GPBSET.bit.GPIO60 = TRUE;
}

void CLRn_LOW(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO60 = TRUE;
}

void PEAK_RST_HIGH(void)
{
	GpioDataRegs.GPCSET.bit.GPIO64 = TRUE;
}

void PEAK_RST_LOW(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO64 = TRUE;
}

char PEAK_SYNC(void)
{
	BOOL peak_sync = 0;
	peak_sync = GpioDataRegs.GPCDAT.bit.GPIO66;
	return(peak_sync);
}

void TRIG_EX5_HIGH(void)
{
	GpioDataRegs.GPCSET.bit.GPIO76 = TRUE;
}

void TRIG_EX5_LOW(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO76 = TRUE;
}

void EXT_TRIG_HIGH(void)	// Low to High (Trigger)
{
	GpioDataRegs.GPCSET.bit.GPIO77 = TRUE;
}

void EXT_TRIG_LOW(void)		// Normal
{
	GpioDataRegs.GPCCLEAR.bit.GPIO77 = TRUE;
}

void TRIG_ICCD_HIGH(void)
{
	GpioDataRegs.GPCSET.bit.GPIO78 = TRUE;
}

void TRIG_ICCD_LOW(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO78 = TRUE;
}

void TRIG_EMCCD_HIGH(void)
{
	GpioDataRegs.GPCSET.bit.GPIO79 = TRUE;
}

void TRIG_EMCCD_LOW(void)
{
	GpioDataRegs.GPCCLEAR.bit.GPIO79 = TRUE;
}

void OPTO_ANOD_HIGH(void)
{
	GpioDataRegs.GPBSET.bit.GPIO34 = TRUE;
}

void OPTO_ANOD_LOW(void)
{
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = TRUE;
}

void LED1_ON(void)	// GPIO-38 - LED-3
{
	GpioDataRegs.GPBSET.bit.GPIO38 = TRUE;
}

void LED1_OFF(void)	// GPIO-38
{
	GpioDataRegs.GPBCLEAR.bit.GPIO38 = TRUE;
}

void LED1_TOGGLE(void) // GPIO-38
{
	GpioDataRegs.GPBTOGGLE.bit.GPIO38 = TRUE;
}

void LED2_ON(void)	// GPIO-35 - LED-2
{
	GpioDataRegs.GPBSET.bit.GPIO35 = TRUE;
}

void LED2_OFF(void)	// GPIO-35
{
	GpioDataRegs.GPBCLEAR.bit.GPIO35 = TRUE;
}

void LED3_ON(void)	// GPIO-36 - LED-1
{
	GpioDataRegs.GPBSET.bit.GPIO36 = TRUE;
}

void LED3_OFF(void)	// GPIO-36
{
	GpioDataRegs.GPBCLEAR.bit.GPIO36 = TRUE;
}

void LED4_ON(void)	// GPIO-37 - LED-4
{
	GpioDataRegs.GPBSET.bit.GPIO37 = TRUE;
}

void LED4_OFF(void)	// GPIO-37
{
	GpioDataRegs.GPBCLEAR.bit.GPIO37 = TRUE;
}

