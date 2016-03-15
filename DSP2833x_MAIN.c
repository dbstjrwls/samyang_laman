//----------------------------------------------------------------------------------------------------
// Project :
// Company :
// Data : 20160314b - peak detect 기능 등등 기능 다시 추가.
//----------------------------------------------------------------------------------------------------
#include "DSP2833x_Device.h"     		// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   		// DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"
#include "math.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "FPU.h"
#include "Main.h"
#include "Adc.h"
#include "Can.h"
#include "Ctrl.h"“
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"
#include "Global.h"

//#define TEST_IN_DST

void Do_While_loop();

interrupt void INT13_ISR(void);
interrupt void INT14_ISR(void);
interrupt void XINT1_ISR(void);
interrupt void SCITXINTA_ISR(void);
interrupt void SCIRXINTA_ISR(void);
interrupt void SCITXINTC_ISR(void);
interrupt void SCIRXINTC_ISR(void);
interrupt void ADCINT_ISR(void);

struct CPUTIMER_VARS CpuTimer0;
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;

// IIR Filter Coef
float FS;
float FC_H;				// CutOff Frequency (1KHz)
float FC_L;
double FC_D;
float Omega_H;
float Omega_L;
double Omega_D;
float Alpha_H;
float Alpha_L;
double Alpha_D;
float Betha_H;
float Betha_L;
double Betha_D;

unsigned int first_adc = 0;

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_period (0x101), Periodic_Sequence_number 명령
unsigned int cmd_period_seq_num = 0;

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_dt_cmd (0x111), 제어 명령
BOOL cmd_init_start = 0;					// 초기화 시작 명령
BOOL cmd_beam_shutter = 0;					// 빔셔터 동작 명령
BOOL cmd_operate = 0;						// 제어 동작 명령
BOOL cmd_diag = 0;							// 자체진단 동작 명령
BOOL cmd_stop = 0;							// 종료 동작 명령
BOOL cmd_req_state = 0;						// 상태정보 요청 명령
BOOL cmd_req_version = 0;					// 버전정보 요청 명령

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_trigger (0x121), Excimer Laser Trigger
unsigned long cmd_trigger_index = 0;
unsigned int cmd_trigger_state = 0;

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_energy (0x122), Excimer Laser Energy
unsigned long cmd_energy_index = 0; 		// 탐지 트리거 index (Rsp)
unsigned int cmd_energy_meter = 0;			// 탐지 에너지 측정값 (Rsp)
unsigned int cmd_energy_hv_peak = 0;		// 탐지 Hv-Peak (Rsp)

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_set (0x123), Excimer Laser Set
unsigned int cmd_laser_set_action = 0; 		// 레이저 설정(설정, 동작, 정지)
unsigned int cmd_laser_set_hv = 0;			// 레이저 High-Voltage 설정
unsigned int cmd_laser_set_period = 0;		// 레이저 트리거 주기 설정
unsigned int cmd_laser_set_repeat = 0;		// 레이저 트리거 횟수 설정

//----------------------------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_config (0x124), Excimer Laser Config
LASER_CTRL_LINE laser_ctrl_line;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_period (0x201), Periodic_Sequence_number 응답
unsigned int rsp_period_seq_num = 0;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_version (0x202), 버전
unsigned int d_version_1 = 1;
unsigned int d_version_2 = 1;
unsigned int d_release_1 = 1;
unsigned int d_release_2 = 1;
unsigned int d_year = 16;
unsigned int d_month = 3;
unsigned int d_date = 9;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_cmd (0x211), 제어 명령 응답
BOOL rsp_init_start = 0;					// 초기화 시작 응답
BOOL rsp_beam_shutter = 0;					// 빔셔터 동작 응답
BOOL rsp_operate = 0;						// 제어 동작 응답
BOOL rsp_diag = 0;							// 자체진단 동작 응답
BOOL rsp_stop = 0;							// 종료동작 응답
BOOL rsp_req_state = 0;						// 상태정보 요청 응답
BOOL rsp_req_version = 0;					// 버전정보 요청 응답

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_trigger (0x221)
unsigned long rsp_trigger_index = 0;
unsigned int rsp_trigger_state = 0;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_energy (0x222)
unsigned long rsp_energy_index = 0; 		// 탐지 트리거 index
Uint32 rsp_energy_meter = 0.0f;				// 탐지 에너지 측정값
unsigned int rsp_energy_hv_peak = 0;		// 탐지 Hv-Peak

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_laser_set (0x223)
unsigned int rsp_laser_set_action = 0; 		// 레이저 설정(설정, 동작, 정지)
unsigned int rsp_laser_set_hv = 0;			// 레이저 High-Voltage 설정
unsigned int rsp_laser_set_period = 0;		// 레이저 트리거 주기 설정
unsigned int rsp_laser_set_repeat = 0;		// 레이저 트리거 횟수 설정

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_laser_config (0x224)
LASER_CTRL_LINE rsp_laser_ctrl_line = {0,};

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_status_1 (0x241)
unsigned int rsp_standby_time = 0;
unsigned int rsp_gas_pressure = 0;
float rsp_ls_temp = 0.0f;
float rsp_filament_m = 0.0f;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_status_2 (0x242)
unsigned int ctrl_bd_temp = 0;

//----------------------------------------------------------------------------------------------------
// Detect -> Spectro
// dt_fault (0x281)
DETECT_ERROR_TYPE rsp_dt_fault = {0,};

//unsigned char detect_state = 0x00;
unsigned char Detect_State_Device = 0;
unsigned char Detect_Error_Device = 0;
unsigned char Detect_Error_State = 0;
unsigned char Detect_Error_Code = 0;

unsigned long total_detect_cnt = 0;
BOOL i2c_error = 1;

//----------------------------------------------------------------------------------------------------
void main(void)
{
	FS = SAMPLE_FREQ;
	FC_H = 10.0;		// Cutoff Frequency Hz
	FC_L = 75.0;		// Cutoff Frequency Hz
	FC_D = 0.01;		// Cutoff Frequency Hz

	Omega_H	= (tan((FC_H / FS) * PI));
	Omega_L = (tan((FC_L / FS) * PI));
	Omega_D = (tan((FC_D / FS) * PI));

	Alpha_H	= ((1.0 - Omega_H) / (1.0 + Omega_H));
	Alpha_L	= ((1.0 - Omega_L) / (1.0 + Omega_L));
	Alpha_D	= ((1.0 - Omega_D) / (1.0 + Omega_D));

	Betha_H	= (Omega_H / (1.0 + Omega_H));
	Betha_L = (Omega_L / (1.0 + Omega_L));
	Betha_D = (Omega_D / (1.0 + Omega_D));

	InitSysCtrl();
	Init_Gpio();
	DINT;			// Disable Interrupt

	// PIE 인터럽트를 활성화 하는 레지스터들로 구성
	// 모든 인터럽트 Disable 설정
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;

	// PIE Vector table 초기화 함수, 모든 인터럽트 비활성화
	InitPieVectTable();

	// Interrupt Vector 가 보호되어 있기 때문에, 'EALLOW'를 사용하여
	// 인터럽트 테이블에 인터럽트 서비스 루틴을 define 하고
	// 'EDIS'를 사용하여 다시 보호
	EALLOW;

	// 인터럽트 함수를 vector table에 저장
	PieVectTable.TINT0  = &TINT0_ISR;	// cpu_timer0
	PieVectTable.XINT13 = &INT13_ISR;	// cpu_timer1
	PieVectTable.TINT2 = &INT14_ISR;	// cpu_timer2
	EDIS;

	// FOR FLASH BURNING
	// MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();
	I2CA_Init();
	InitI2CGpio();

	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, TIMEDIV1, TIMEDIV2);		// 5.28kHz
	ConfigCpuTimer(&CpuTimer1, 150, 1000);  			// 1ms
	ConfigCpuTimer(&CpuTimer2, 150, 100000);			// 100ms timer

	CpuTimer0Regs.TCR.all = 0x4001;		// Timer0 : Control Register, Timer Interrupt Enable
	CpuTimer1Regs.TCR.all = 0x4001;
	CpuTimer2Regs.TCR.all = 0x4001;

	// IER, CPU interrupt enable register
	IER |= M_INT1;		// Timer0 Interrupt - CPU Int1 Enable
	IER |= M_INT13; 	// Timer1 Interrupt - CPU Int13 Enable
	IER |= M_INT14;		// Timer2 Interrupt - CPU Int14 Enable

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	init_can();
	Init_Adc_Mode();
	Init_SCI_UART_A();
	Init_SCI_UART_C();

	Init_External_Interrupt();
	EnableInterrupts();
	EINT; 						// enable interrupt
	ERTM; 						// Realtime interrupt DBGM Enable
	// EPwm5Regs.CMPA.half.CMPA = 21;
	Init_array();
	delay_ms(10);

	while(first_adc < SAMPLE_FREQ) {
		ADC_Conversion();
		first_adc++;
	}

	Beam_Shutter_Init();
	Laser_CCD_Trig_Init();
	EX5_Laser_Init();
	Fault_Flag_Init();
	CLRn_HIGH();
//	write_txbufc_string("\r\n_DETECT_CONTROLLER_\r\n");		// Debug_Command Error
//	write_txbufc_string("\r\n_LASER_MODULE_ICD_ \r\n");		// Debug_Command Error
	delay_ms(10);

#ifdef _TEST_IN_DST
cmd_init_start = TRUE;
#endif

	Do_While_loop();
}

void Do_While_loop()
{
    static volatile u16 state=0;

	while(1)
	{
        Shutter_Fault_Check();
		EX5_LASER_STATUS(); // 기능 없음
		Monitoring();
		Peak_Data_Conv();
		Sync_Check();
		Can_PROCESSING();

		/*
		switch(state)
		{
			//--------------------------------------------------------------------------------
			// INIT
			//--------------------------------------------------------------------------------
			case 0:
				System_Ctrl_Init();
		 		total_detect_cnt = Read_Uint32(ADDR_EEPROM);
				state = 1;
				break;

			case 1:
				if(cmd_init_start == 1)
				{
					if((rsp_dt_fault.flt_beam_shutter == 0) && (rsp_dt_fault.flt_laser_init == 0) && (Laser_Pwr_Complete == 1)) {
						SetDetectState(ST_INIT_COMPLETE);
						state = 2;
					}
					else if((rsp_dt_fault.flt_beam_shutter == 0) && (rsp_dt_fault.flt_laser_init == 0) && (Laser_Pwr_Complete == 0)) {
						SetDetectState(ST_INIT_RUN);
					}
					else {
						SetDetectState(ST_INIT_ERROR);
					}
					//detect_state = ST_INIT_COMPLETE;  	// Test_Mode
					//state = 2; 							// Test_Mode
				}
				else
					SetDetectState(ST_POWER_ON);
				break;

			//--------------------------------------------------------------------------------
			// READY
			//--------------------------------------------------------------------------------
			case 2:
				EX5_HighVoltage_Set(cmd_laser_set_hv);	// Set High-Voltage Parameter
				SetDetectState(ST_READY_COMPLETE);

				if((cmd_init_start == 1) && (cmd_operate == 1)) {	// START CONTROL
					state = 3;
				}
				break;

			//--------------------------------------------------------------------------------
			// START
			//--------------------------------------------------------------------------------
			case 3:
				if((cmd_init_start == 1) && (cmd_operate == 1)) {
					trig_gen_cmd = 1;
					RST_EX5_DISENABLE();
					MOTOR1_ON();
					SetDetectState(ST_OPERATE_COMPLETE);
				}
				else if((cmd_init_start == 1) && (cmd_operate == 0)) {
					state = 4;
					SetDetectState(ST_STOP_RUN);
				}
				break;

			//--------------------------------------------------------------------------------
			// STOP
			//--------------------------------------------------------------------------------
			case 4:
				EX5_HighVoltage_Init();
				RST_EX5_ENABLE();
				MOTOR1_OFF();
				Write_Uint32(ADDR_EEPROM, rsp_energy_index);
				I2C_Release();

				laser_trigger_flag = 1;		// Laser Trigger Repeat Flag
				trig_gen_cmd = 0;
				SetDetectState(ST_STOP_COMPLETE);
				state = 5;
				break;

			case 5:
				if((cmd_init_start == 1) && (cmd_operate == 1)) {
					laser_trigger_flag = 0;
					state = 2;
					SetDetectState(ST_OPERATE_RUN);
				}
				break;

			case 6:
				Helium_Vent_Seq();
				break;

			case 7:
				Premix_Vent_Seq();
				break;

			case 8:
				Refill_Seq();
				break;

			default:
				state = 0;
				break;
		}
		*/
	}
}

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

void InitFlash(void)
{
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   FlashRegs.FOPT.bit.ENPIPE = 1;

   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI.
   //Refer to the datasheet for the latest information.
#if CPU_FRQ_150MHZ
   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;

   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 8;
#endif

#if CPU_FRQ_100MHZ
   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;

   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;
#endif
   //                CAUTION
   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
   EDIS;

   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   asm(" RPT #7 || NOP");
}




