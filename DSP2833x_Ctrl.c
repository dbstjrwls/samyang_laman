
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

Timer HeliumTimer;
Timer PremixTimer;
Timer RefillTimer;

HeliumSeqType HeliumSeq = HELIUM_STATE_IDLE;
PremixSeqType PremixSeq = PREMIX_STATE_IDLE;
RefillSeqType RefillSeq = REFILL_STATE_IDLE;

BOOL laser_trigger_flag = 0;        // 0 : 이면 laser trigger flag를 생성
BOOL IsPeakRcvComplete = 0;
BOOL beam_shutter_end = 0;
BOOL trig_gen_cmd = 0;          // 0: laser fire trigger off, 1: laser fire trigger 생성.
BOOL shutter_status = 1;		// Default Logic High
BOOL Laser_Pwr_Complete = 0;        // 초기 5분 대기 하였으면 TRUE
BOOL sync_trig = 0;
BOOL sync_iccd = 0;
BOOL sync_emccd = 0;

char c_flag = 0;
char c_flag_old = 0;
char inter_lock = 0;
char EX5_Laser_Fault = 0;

//unsigned int dt_freq = 0;
//unsigned int laser_trigger_count = 0;
unsigned int rev_emeter_cnt = 0;
unsigned int rev_sync_cnt = 0;
unsigned int peak_trigger_cnt = 0;
unsigned int sync_trigger_cnt = 0;
//unsigned int Laser_Fault_Cnt = 0;
unsigned int sec_cnt = 0;
unsigned int pulse_count = 0;

unsigned int DETECT_PERIOD = 10;

LONG trigger_index = 0;
unsigned char trigger_state = 0;

LONG energy_index = 0;
float energy_meter = 0.0f;
float hv_peak = 0.0f;

void Fault_Flag_Init()
{

}

void Sync_Check(void)
{
	//char c_flag = 0;
	//char c_flag_old = 0;
	//char flag = 0;
	//char flag_old = 0;

	if(sync_trig == 0) {
		//c_flag = TRIGGER_SYNC();
		c_flag = sync_trigger_flag;
	}

	if(sync_iccd == 1)
		c_flag = FIRE_ICCD_STATUS();

	if(sync_emccd == 1)
		c_flag = FIRE_EMCCD_STATUS();

	if(c_flag == 1)
		TRIG_ICCD_HIGH();	// BYPASS TRIGGER

	else TRIG_ICCD_LOW();

	if((c_flag_old == 0) && (c_flag == 1)) {
		LED2_ON();
		//rev_sync_cnt++;
	}
	else {
		LED2_OFF();
	}
	c_flag_old = c_flag;
	//sync_trigger_flag = 0;
}

void Trigger_Gen(void)
{
	TRIG_EX5_HIGH();
	delay_ms(10);
	TRIG_EX5_LOW();
}

void Trigger_Gen_Repeat(BOOL gen)
{
	if(gen == 1) {
		TRIG_EX5_HIGH();
		//CM_TRIG_HIGH();
	}
	else {
		TRIG_EX5_LOW();
		//CM_TRIG_LOW();
	}
}

void Peak_Data_Conv(void)
{
	//IsPeakRcvComplete = 0;
	unsigned int i = 0;

	if(sync_trigger_flag == 1) {
		//EX5_HighVoltage_Set(cmd_laser_set_hv);	// Add Sequence
		//delay_ms(1);								// Add Sequence
		//EX5_HighVoltage_Init();					// Add Sequence

		//if(peak_trigger_flag == 1)
		//{
			delay_us(200);
			rsp_energy_index++;
/*
			Mean_Peak_t = 0;

			for(i = 0; i < 10; i++)
			{
				Mean_Peak_t += (float)ADC[PEAK_VALUE];
			}
			Mean_Peak_t = Mean_Peak_t /10;
*/
			//if(ADC_COMPLETE == 1)
			//{
				//Mean_Peak_t = Moving_Average((float)ADC[PEAK_VALUE]);
				Mean_Peak_t = (float)ADC[PEAK_VALUE];
				Mean_Peak = (Mean_Peak_t / 4096) * 3.0;

				IsPeakRcvComplete = 1;
				//ADC_COMPLETE = 0;
				PEAK_RST_HIGH();
				//PEAK_RST_LOW();
			//}

			//PEAK_RST_HIGH();
			//PEAK_RST_LOW();

			//IsPeakRcvComplete = 1;
			sync_trigger_flag = 0;
			peak_trigger_flag = 0;
		//}
		//PEAK_RST_LOW();
	}
	PEAK_RST_LOW();
}

void EX5_Laser_Init(void)
{
	CM_TRIG_LOW();
	MODE_SEL_LOW();
	PUMP_VALVE_CLOSE();
	MAIN_VALVE_CLOSE();

	RST_EX5_ENABLE();		// Low

	F_INJECTOR_CLOSE();
	PURGE_VALVE_CLOSE();
	FILL_VALVE_CLOSE();

	FAN_OFF();

	MOTOR2_OFF();
	VACCUM_PUMP_OFF();
	MOTOR1_OFF();
	CLRn_LOW();
	PEAK_RST_LOW();
}

void EX5_HighVoltage_Init(void)
{
	Write_AD5627_Data(0x38, 0x0001);
	I2C_Release();
	//delay_ms(100);
	Write_AD5627_Data(0x10, 0x000);
	I2C_Release();
}

// HV Peak : 0-5V (0-20kV)
void EX5_HighVoltage_Set(unsigned int num)
{
	Write_AD5627_Data(0x38, 0x0001);
	I2C_Release();
	//delay_ms(100);
	Write_AD5627_Data(0x10, num);
	I2C_Release();
}

void Beam_Shutter_Init(void)
{
	OPTO_ANOD_LOW();
	BEAM_15V_OFF();
	BEAM_10V_OFF();
}

void Beam_Shutter_On(void)
{
	OPTO_ANOD_HIGH();
	BEAM_15V_ON();
	delay_ms(100);
	BEAM_15V_OFF();
	BEAM_10V_ON();
	//delay_ms(5); // remark, 151119

	shutter_status = OPTO_COLT_STATUS();

	if(shutter_status == 0) {
		LED3_ON();
	}
	else {
		LED3_OFF();
	}
}

void Beam_Shutter_Off(void)
{
	OPTO_ANOD_LOW();
	BEAM_15V_OFF();
	BEAM_10V_OFF();
	shutter_status = OPTO_COLT_STATUS();

	if(shutter_status == 0) {
		LED3_ON();
	}
	else {
		LED3_OFF();
	}
}

void Laser_CCD_Trig_Init(void)
{
	TRIG_EX5_LOW();
	EXT_TRIG_LOW();
	TRIG_ICCD_LOW();
	TRIG_EMCCD_LOW();
}

void Shutter_Fault_Check()	// Default : High
{
	shutter_status = OPTO_COLT_STATUS();

	if(cmd_beam_shutter == 1) {
		if(shutter_status == 1) {
			rsp_dt_fault.flt_beam_shutter = 1;
		}
		else {
			rsp_dt_fault.flt_beam_shutter = 0;
		}
	}
	else if(cmd_beam_shutter == 0) {
		if(shutter_status == 0) {
			rsp_dt_fault.flt_beam_shutter = 1;
		}
		else {
			rsp_dt_fault.flt_beam_shutter = 0;
		}
	}
}

void EX5_LASER_STATUS(void)
{
	// 0 : Cover Open
	inter_lock = INT_LOCK_STATUS();

	// 0 : 레이저 (Key-Off)
	// 1 : 레이저 (Key-On)
	EX5_Laser_Fault = PWR_STATUS();
	if(EX5_Laser_Fault == 1) {
		//
	}
	else {
		//
	}
}

void System_Ctrl_Init()
{
	//Beam_Shutter_Init();		// Beam-Shutter Init
	EX5_HighVoltage_Init();		// Laser High-Voltage Init
	Laser_CCD_Trig_Init();		// Trigger Init
	Fault_Flag_Init();
}

void Monitoring(void)
{
	if(ADC_COMPLETE == 1) {
		ADC_Conversion();
		ADC_COMPLETE = 0;
	}

	if((cmd_beam_shutter == 1) && (beam_shutter_end == 0)) {
		Beam_Shutter_On();
		beam_shutter_end = 1;
	}
	else if((cmd_beam_shutter == 0) && (beam_shutter_end == 1)) {
		Beam_Shutter_Off();
		beam_shutter_end = 0;
	}
	Monitoring_RS422();
	//Monitoring_RS422C();
}

//----------------------------------------------------------------------------------------------------------------------
// HELIUM_VENT_SEQUENCE

BOOL Helium_Vent_Seq()
{
	switch(HeliumSeq)
	{
		case HELIUM_STATE_IDLE:
			// Init EX5 CONTROL LINES
			RST_EX5_ENABLE();
			F_INJECTOR_CLOSE();
			PURGE_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			FAN_OFF();				// '1' High
			MOTOR1_OFF();
			MOTOR2_OFF();
			MODE_SEL_LOW();			// Command Charge
			CM_TRIG_LOW();			// EX5 CONTROL LINES

			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			PURGE_VALVE_CLOSE();
			HeliumSeq = HELIUM_STATE_STS1;
			break;

		case HELIUM_STATE_STS1:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			PURGE_VALVE_CLOSE();
			HeliumSeq = HELIUM_STATE_STS2;
			timeBase = 0;			// Time_Base
			break;

		case HELIUM_STATE_STS2:		// 2.5_Secs
			VACCUM_PUMP_ON();
			PUMP_VALVE_OPEN();
			MAIN_VALVE_OPEN();
			PURGE_VALVE_CLOSE();

			if((HeliumSeq == HELIUM_STATE_STS2) && (timerCheck_mSec(&HeliumTimer, 2500))) {		// 2.5S
				timerClr(&HeliumTimer);
				HeliumSeq = HELIUM_STATE_STS3;
			}
			else {
				HeliumSeq = HELIUM_STATE_STS2;
			}

			break;

		case HELIUM_STATE_STS3:		// 10_minutes
			VACCUM_PUMP_ON();
			PUMP_VALVE_OPEN();
			MAIN_VALVE_CLOSE();
			PURGE_VALVE_OPEN();

			if((HeliumSeq == HELIUM_STATE_STS3) && (timerCheck_Sec(&HeliumTimer, 600))) {	// 10M
				timerClr(&HeliumTimer);
				HeliumSeq = HELIUM_STATE_STS4;
			}
			else {
				HeliumSeq = HELIUM_STATE_STS3;
			}

			break;

		case HELIUM_STATE_STS4:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			PURGE_VALVE_CLOSE();
			HeliumSeq = HELIUM_STATE_END;
			break;

		case HELIUM_STATE_END:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			PURGE_VALVE_CLOSE();

			// Check Gas_Pressure
			// Repeat Sequence
			//HeliumSeq = HELIUM_STATE_IDLE;
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// PREMIX_VENT_SEQUENCE

BOOL Premix_Vent_Seq()
{
	switch(PremixSeq)
	{
		case PREMIX_STATE_IDLE:
			// Init EX5 CONTROL LINES
			RST_EX5_ENABLE();
			F_INJECTOR_CLOSE();
			PURGE_VALVE_CLOSE();
			FAN_OFF();				// '1' High
			MOTOR1_OFF();
			MOTOR2_OFF();
			MODE_SEL_LOW();			// Command Charge
			CM_TRIG_LOW();			// EX5 CONTROL LINES

			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			PremixSeq = PREMIX_STATE_STS1;
			break;

		case PREMIX_STATE_STS1:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			PremixSeq = PREMIX_STATE_STS2;

			timeBase = 0;			// Time_Base
			break;

		case PREMIX_STATE_STS2:		// 2.5_Secs
			VACCUM_PUMP_ON();
			PUMP_VALVE_OPEN();
			MAIN_VALVE_OPEN();
			FILL_VALVE_CLOSE();

			if((PremixSeq == PREMIX_STATE_STS2) && (timerCheck_mSec(&PremixTimer, 2500))) {		// 2.5S
				timerClr(&PremixTimer);
				PremixSeq = PREMIX_STATE_STS3;
			}
			else {
				PremixSeq = PREMIX_STATE_STS2;
			}

			break;

		case PREMIX_STATE_STS3:		// 10_minutes
			VACCUM_PUMP_ON();
			PUMP_VALVE_OPEN();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_OPEN();

			if((PremixSeq == PREMIX_STATE_STS3) && (timerCheck_Sec(&PremixTimer, 600))) {	// 10M
				timerClr(&PremixTimer);
				PremixSeq = PREMIX_STATE_STS4;
			}
			else {
				PremixSeq = PREMIX_STATE_STS3;
			}

			break;

		case PREMIX_STATE_STS4:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			PremixSeq = PREMIX_STATE_END;
			break;

		case PREMIX_STATE_END:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();

			// Check Gas_Pressure
			// Repeat Sequence
			//PremixSeq = PREMIX_STATE_IDLE;
			break;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// REFILL_SEQUENCE

BOOL Refill_Seq()
{
	switch(RefillSeq)
	{
		case REFILL_STATE_IDLE:
			// Init EX5 CONTROL LINES
			RST_EX5_ENABLE();
			F_INJECTOR_CLOSE();
			PURGE_VALVE_CLOSE();
			FAN_OFF();				// '1' High
			MOTOR1_OFF();
			MOTOR2_OFF();
			MODE_SEL_LOW();			// Command Charge
			CM_TRIG_LOW();			// EX5 CONTROL LINES

			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			RefillSeq = REFILL_STATE_STS1;
			break;

		case REFILL_STATE_STS1:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			RefillSeq = REFILL_STATE_STS2;

			timeBase = 0;			// Time_Base
			break;

		case REFILL_STATE_STS2:		// 20sec
			VACCUM_PUMP_ON();
			PUMP_VALVE_OPEN();
			MAIN_VALVE_OPEN();
			FILL_VALVE_CLOSE();

			if((RefillSeq == REFILL_STATE_STS2) && (timerCheck_mSec(&RefillTimer, 20000))) {	// 20S
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS3;
			}
			else {
				RefillSeq = REFILL_STATE_STS2;
			}

			break;

		case REFILL_STATE_STS3:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_OPEN();
			FILL_VALVE_OPEN();

			if((RefillSeq == REFILL_STATE_STS3) && (timerCheck_mSec(&RefillTimer, 5000))) {		// 5S
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS4;
			}
			else {
				RefillSeq = REFILL_STATE_STS3;
			}

			break;

		case REFILL_STATE_STS4:
			VACCUM_PUMP_OFF();
			PUMP_VALVE_CLOSE();
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();

			if((RefillSeq == REFILL_STATE_STS4) && (timerCheck_mSec(&RefillTimer, 5000))) {		// 5S
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS5;
			}
			else {
				RefillSeq = REFILL_STATE_STS4;
			}

			break;

		case REFILL_STATE_STS5:		// Pulse (High)

			MAIN_VALVE_OPEN();
			FILL_VALVE_OPEN();

			if((RefillSeq == REFILL_STATE_STS5) && (timerCheck_mSec(&RefillTimer, 50))) {	// 50mS
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS6;
			}
			else {
				RefillSeq = REFILL_STATE_STS5;
			}

			break;

		case REFILL_STATE_STS6:		// Pulse (Low)

			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();

			if((RefillSeq == REFILL_STATE_STS6) && (timerCheck_mSec(&RefillTimer, 50))) {	// 50mS
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS5;

				pulse_count++;
				if(pulse_count == 6) {
					pulse_count = 0;
					RefillSeq = REFILL_STATE_STS7;
				}
			}
			else {
				RefillSeq = REFILL_STATE_STS6;
			}

			break;

		case REFILL_STATE_STS7:

			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			PUMP_VALVE_CLOSE();

			if((RefillSeq == REFILL_STATE_STS7) && (timerCheck_mSec(&RefillTimer, 1000))) {		// 1S
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_STS8;
			}
			else {
				RefillSeq = REFILL_STATE_STS7;
			}

			break;

		case REFILL_STATE_STS8:		// End_Pulse
			MAIN_VALVE_CLOSE();
			FILL_VALVE_OPEN();
			PUMP_VALVE_OPEN();

			if((RefillSeq == REFILL_STATE_STS8) && (timerCheck_mSec(&RefillTimer, 1000))) {		// 1S
				timerClr(&RefillTimer);
				RefillSeq = REFILL_STATE_END;
			}
			else {
				RefillSeq = REFILL_STATE_STS8;
			}

			break;

		case REFILL_STATE_END:
			MAIN_VALVE_CLOSE();
			FILL_VALVE_CLOSE();
			PUMP_VALVE_CLOSE();

			//RefillSeq = REFILL_STATE_IDLE;
			break;
	}
}

