
#include "DSP2833x_Device.h"     	// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   	// DSP2833x Examples Include File
#include "DSP2833x_28335_Define.h"
#include "Main.h"
#include "Adc.h"
#include "Ctrl.h"
#include "IO.h"
#include "Pie.h"
#include "Uart.h"
#include "I2c.h"
#include "can.h"
#include "global.h"

void InitECanAGpio(void);
void InitECanA(void);				// Initialize eCAN-A module
void InitECanBGpio(void);
void InitECanB(void);				// Initialize eCAN-A module
void SendDataToECanA_Mailbox1(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox2(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox3(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox4(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox5(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox6(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox7(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox8(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanA_Mailbox9(long id, BYTE length, LONG low_data, LONG high_data);
void SendDataToECanB(long id, BYTE length, LONG low_data, LONG high_data);

void DETECT_PERIOD_201();          	// Can_Process_RX 응답		-> 완료.***********
void DETECT_VERSION_202();          // Can_Process_RX 조건부 응답	-> 완료.**********
void DETECT_COMMAND_211();      	// Can_Process_RX 응답			-> 완료.*********
void DETECT_TRIGGER_221();          // Can_Process_RX 조건부 응답 + 주기적 조건부 전송
void DETECT_ENERGY_222();           // 주기적 조건부 전송
void DETECT_LASER_SET_223();    	// Can_Process_RX 응답
void DETECT_LASER_CONFIG_224();    	// Can_Process_RX 응답 + 주기적 전송
void DETECT_STATE_232();           	// Can_Process_RX 조건부 응답 + 주기적 조건부 전송
void DETECT_STATUS1_241();         	// 주기적 전송.
void DETECT_STATUS2_242();         	// 주기적 전송.
void DETECT_FAULT_281();     		// 주기적 전송.

BOOL CAN_TX_INT_FLAG = 0;
BOOL CAN_TX_READY_FLAG = 0;
BOOL CAN_CH_FLAG = 0;
BOOL CAN_CH = 0;

BOOL CAN_rx_flag = FALSE;		// CAN-A New Data Check
LONG CAN_rx_id = 0;				// CAN-A Receive ID
BYTE CAN_rx_length = 0;   		// CAN-A Receive Data
LONG CAN_rx_low_data = 0;		// CAN-A Receive Low Word Data
LONG CAN_rx_high_data =	0;		// CAN-A Receive High Word Data
WORD CAN_PERIOD_CHECK = 0;

char Period_Count = 0;
unsigned int can_energy_cnt = 0;
unsigned char before_detect_state = 0;
Uint32 CAN_tx_low[10];
Uint32 CAN_tx_high[10];

BOOL m_bSetRcvdLaserSetCmd = FALSE;

void SetRcvdLaserSetCmd(void)
{
    m_bSetRcvdLaserSetCmd = TRUE;
}

BOOL IsRcvdLaserSetCmd(void)
{
    return m_bSetRcvdLaserSetCmd;
}


//-------------------------------------------------------------------------
// cmd_init_start	[초기화 시작 명령  ]
// cmd_beam_shutter	[빔셔터 동작 명령  ]
// cmd_operate		[제어 동작 명령    ]
// cmd_diag			[자체진단 동작 명령]
// cmd_stop			[종료 동작 명령    ]
// cmd_req_state	[상태정보 요청 명령]
// cmd_req_version	[버전정보 요청 명령]
//-------------------------------------------------------------------------

void Can_Process_RX(void)
{
	unsigned int word_index1 = 0;
	unsigned int word_index2 = 0;
	unsigned int word_index3 = 0;
	unsigned int word_index4 = 0;
	unsigned int word_index5 = 0;
	unsigned int word_index6 = 0;
	unsigned int word_index7 = 0;

	unsigned long long_index1 = 0;
	unsigned long long_index2 = 0;

	BOOL Comp_0 = 0;
	BOOL Comp_1 = 0;
	BOOL Comp_2 = 0;
	BOOL Comp_3 = 0;
	BOOL Comp_4 = 0;
	BOOL Comp_5 = 0;
	BOOL Comp_6 = 0;
//	BOOL Comp_7 = 0;
//	BOOL Comp_8 = 0;
//	BOOL Comp_9 = 0;
//	BOOL Comp_10 = 0;
//	BOOL Comp_11 = 0;
	BOOL Comp_12 = 0;

	if(CAN_rx_id == SP_CMD_01_PERIOD)
	{
		Comp_12 = (CAN_rx_low_data >> 24) & 0xff;
		cmd_period_seq_num = Comp_12;
		rsp_period_seq_num = cmd_period_seq_num;

		CAN_CH = 1;
		DETECT_PERIOD_201();
		SendDataToECanA_Mailbox1(DT_RSP_01_PERIOD, DT_LENGTH_01_PERIOD, CAN_tx_low[0], CAN_tx_high[0]);
	}
	else if(CAN_rx_id == SP_CMD_11_CMD)
	{
		Comp_0 = (CAN_rx_low_data >> 24) & 0x01;	// Bit-Index[0]
		Comp_1 = (CAN_rx_low_data >> 25) & 0x01;	// Bit-Index[1]
		Comp_2 = (CAN_rx_low_data >> 26) & 0x01;	// Bit-Index[2]
		Comp_3 = (CAN_rx_low_data >> 27) & 0x01;	// Bit-Index[3]
		Comp_4 = (CAN_rx_low_data >> 28) & 0x01;	// Bit-Index[4]
		Comp_5 = (CAN_rx_low_data >> 29) & 0x01;	// Bit-Index[5]
		Comp_6 = (CAN_rx_low_data >> 30) & 0x01;	// Bit-Index[6]

		cmd_init_start = Comp_0;
		cmd_beam_shutter = Comp_1;
		cmd_operate = Comp_2;
		cmd_diag = Comp_3;
		cmd_stop = Comp_4;
		cmd_req_state = Comp_5;
		cmd_req_version = Comp_6;

		rsp_init_start = cmd_init_start;
		rsp_beam_shutter = cmd_beam_shutter;
		rsp_operate = cmd_operate;
		rsp_diag = cmd_diag;
		rsp_stop = cmd_stop;
		rsp_req_state = cmd_req_state;
		rsp_req_version = cmd_req_version;

		if(cmd_init_start == 1)
		{
			Command_Start = 1;
		}
		else
		{
			Command_Start = 0;
		}

		DETECT_COMMAND_211();
		SendDataToECanA_Mailbox1(DT_RSP_11_CMD, DT_LENGTH_11_CMD, CAN_tx_low[0], CAN_tx_high[0]);

		if(cmd_req_state == 1)
		{
			DETECT_STATE_232();
			SendDataToECanA_Mailbox5(DT_RSP_32_STATE, DT_LENGTH_32_STATE, CAN_tx_low[5], CAN_tx_high[5]);
		}

		if(cmd_req_version == 1)
		{
			DETECT_VERSION_202();
			SendDataToECanA_Mailbox1(DT_RSP_02_VERSION, DT_LENGTH_02_VER, CAN_tx_low[0], CAN_tx_high[0]);
		}
	}
	else if(CAN_rx_id == SP_CMD_21_TRIGGER)
	{
		long_index1 |= ((CAN_rx_low_data >> 24) & 0xff);		// Trigger-Index(unsigned long)
		long_index1 |= ((CAN_rx_low_data >> 16) & 0xff) << 8;

		long_index1 |= ((CAN_rx_low_data >> 8) & 0xff) << 16;
		long_index1 |= ((CAN_rx_low_data >> 0) & 0xff) << 24;

		word_index1 = ((CAN_rx_high_data >> 24) & 0xff);		// Trigger-State

		cmd_trigger_index = long_index1;
		cmd_trigger_state = word_index1;

		if(cmd_trigger_state == 0)
		{
			rsp_trigger_index = cmd_trigger_index;		// save
			rsp_energy_index = cmd_trigger_index;
			rsp_trigger_state = 0;						// Rsp Trigger-Index

			DETECT_TRIGGER_221();
			SendDataToECanA_Mailbox9(DT_RSP_21_TRIGGER, DT_LENGTH_21_TRIGGER, CAN_tx_low[9], CAN_tx_high[9]);
		}
	}
	else if(CAN_rx_id == SP_CMD_22_ENERGY)
	{
		long_index2 |= ((CAN_rx_low_data >> 24) & 0xff);			// Energy-Index(unsigned long)
		long_index2 |= ((CAN_rx_low_data >> 16) & 0xff) << 8;

		long_index2 |= ((CAN_rx_low_data >> 8) & 0xff) << 16;
		long_index2 |= ((CAN_rx_low_data >> 0) & 0xff) << 24;

		word_index2 |= ((CAN_rx_high_data >> 24) & 0xff);			// Energy-Meter Value
		word_index2 |= ((CAN_rx_high_data >> 16) & 0xff) << 8;

		word_index3 |= ((CAN_rx_high_data >> 8) & 0xff);			// Energy-Hv-Peak
		word_index3 |= ((CAN_rx_high_data >> 0) & 0xff) << 8;

		cmd_energy_index = long_index2;
		cmd_energy_meter = word_index2;
		cmd_energy_hv_peak = word_index3;
	}
	else if(CAN_rx_id == SP_CMD_23_LASER_SET)
	{
		word_index4 = ((CAN_rx_low_data >> 24) & 0xff);			// Set Laser Action

		word_index5 |= ((CAN_rx_low_data >> 16) & 0xff);		// Set Laser Hv-Peak
		word_index5 |= ((CAN_rx_low_data >> 8) & 0xff) << 8;

		word_index6 |= ((CAN_rx_low_data >> 0) & 0xff);			// Set Laser Period
		word_index6 |= ((CAN_rx_high_data >> 24) & 0xff) << 8;

		word_index7 |= ((CAN_rx_high_data >> 16) & 0xff);		// Set Laser Repeat
		word_index7 |= ((CAN_rx_high_data >> 8) & 0xff) << 8;

		cmd_laser_set_action = word_index4;
		cmd_laser_set_hv = word_index5;
		cmd_laser_set_period = word_index6;
		cmd_laser_set_repeat = word_index7;

		DETECT_LASER_SET_223();
		SendDataToECanA_Mailbox1(DT_RSP_23_LASER_SET, DT_LENGTH_23_LASER_SET, CAN_tx_low[0], CAN_tx_high[0]);

		SetRcvdLaserSetCmd();
	}
	else if(CAN_rx_id == SP_CMD_24_CONFIG)
	{
		BOOL Comp_0 = 0;
		BOOL Comp_1 = 0;
		BOOL Comp_2 = 0;
		BOOL Comp_3 = 0;
		BOOL Comp_4 = 0;
		BOOL Comp_5 = 0;
		BOOL Comp_6 = 0;
		BOOL Comp_7 = 0;
		BOOL Comp_8 = 0;
		BOOL Comp_9 = 0;
		BOOL Comp_10 = 0;
		BOOL Comp_11 = 0;

		Comp_0 = (CAN_rx_low_data >> 24) & 0x01;	// Bit-Index[0]
		Comp_1 = (CAN_rx_low_data >> 25) & 0x01;	// Bit-Index[1]
		Comp_2 = (CAN_rx_low_data >> 26) & 0x01;	// Bit-Index[2]
		Comp_3 = (CAN_rx_low_data >> 27) & 0x01;	// Bit-Index[3]
		Comp_4 = (CAN_rx_low_data >> 28) & 0x01;	// Bit-Index[4]
		Comp_5 = (CAN_rx_low_data >> 29) & 0x01;	// Bit-Index[5]
		Comp_6 = (CAN_rx_low_data >> 30) & 0x01;	// Bit-Index[6]
		Comp_7 = (CAN_rx_low_data >> 31) & 0x01;	// Bit-Index[7]

		Comp_8 = (CAN_rx_low_data >> 16) & 0x01;	// Bit-Index[8]
		Comp_9 = (CAN_rx_low_data >> 17) & 0x01;	// Bit-Index[9]
		Comp_10 = (CAN_rx_low_data >> 18) & 0x01;	// Bit-Index[10]
		Comp_11 = (CAN_rx_low_data >> 19) & 0x01;	// Bit-Index[11]

		rsp_laser_ctrl_line.ls_reset = Comp_0;
		rsp_laser_ctrl_line.ls_trigger = Comp_1;
		rsp_laser_ctrl_line.ls_mode = Comp_2;
		rsp_laser_ctrl_line.ls_flu_injector = Comp_3;
		rsp_laser_ctrl_line.ls_cool_fan = Comp_4;
		rsp_laser_ctrl_line.ls_sol_1 = Comp_5;
		rsp_laser_ctrl_line.ls_sol_2 = Comp_6;
		rsp_laser_ctrl_line.ls_sol_3 = Comp_7;
		rsp_laser_ctrl_line.ls_sol_4 = Comp_8;
		rsp_laser_ctrl_line.ls_motor_1 = Comp_9;
		rsp_laser_ctrl_line.ls_motor_2 = Comp_10;
		rsp_laser_ctrl_line.ls_vac_pump = Comp_11;

		DETECT_LASER_CONFIG_224();
		SendDataToECanA_Mailbox1(DT_RSP_24_LASER_CONFIG, DT_LENGTH_24_LASER_CONFIG, CAN_tx_low[0], CAN_tx_high[0]);
	}
	else
	 return;
}

void Can_Process_TX(void)
{
    static volatile E_DETECT_STATE before_detect_state = ST_NONE_STATE;
    E_DETECT_STATE detect_state = GetDetectState();

    static volatile u8 oder = 0;

	if(oder >= 4) oder = 0;

	if(CAN_CH == 1)
	{
		switch(oder++)
		{
			case 0:
				DETECT_STATUS1_241();
				SendDataToECanA_Mailbox6(DT_RSP_41_STATUS_1, DT_LENGTH_41_STATUS_1, CAN_tx_low[6], CAN_tx_high[6]);
				break;

			case 1:
				DETECT_STATUS2_242();
				SendDataToECanA_Mailbox7(DT_RSP_42_STATUS_2, DT_LENGTH_42_STATUS_2, CAN_tx_low[7], CAN_tx_high[7]);
				break;

			case 2:
				DETECT_FAULT_281();
				SendDataToECanA_Mailbox8(DT_RSP_81_FAULT, DT_LENGTH_81_FAULT, CAN_tx_low[8], CAN_tx_high[8]);
				break;

			case 3:
				DETECT_LASER_CONFIG_224();
				SendDataToECanA_Mailbox2(DT_RSP_24_LASER_CONFIG, DT_LENGTH_24_LASER_CONFIG, CAN_tx_low[2], CAN_tx_high[2]);
				break;

			default:
				break;
		}

		if(before_detect_state != detect_state)
		{
			DETECT_STATE_232();
			SendDataToECanA_Mailbox5(DT_RSP_32_STATE, DT_LENGTH_32_STATE, CAN_tx_low[5], CAN_tx_high[5]);
			before_detect_state = detect_state;
		}
		if(rsp_trigger_state == 1)
		{
			rsp_trigger_index++;
			DETECT_TRIGGER_221();
			SendDataToECanA_Mailbox9(DT_RSP_21_TRIGGER, DT_LENGTH_21_TRIGGER, CAN_tx_low[9], CAN_tx_high[9]);
			rsp_trigger_state = 0;
		}
		if((IsPeakRcvComplete == 1) && (GetDetectState() == ST_OPERATE_COMPLETE))
		{
			DETECT_ENERGY_222();
			SendDataToECanA_Mailbox4(DT_RSP_22_ENERGY, DT_LENGTH_22_ENERGY, CAN_tx_low[4], CAN_tx_high[4]);
			IsPeakRcvComplete = 0;
		}
	}
}

void DETECT_PERIOD_201()
{
	Uint32 tmp = 0x00000000;

	CAN_tx_low[0] = 0x00000000;
	CAN_tx_high[0] = 0x00000000;

	tmp = 0x00000000;
	tmp = rsp_period_seq_num & 0xff;
	CAN_tx_low[0] = (tmp << 24);			// 0-Byte, Sequence-Number
}

void DETECT_VERSION_202()
{
	Uint32 tmp = 0x00000000;

	CAN_tx_low[0] = 0x00000000;
	CAN_tx_high[0] = 0x00000000;

	tmp = 0x00000000;
	tmp = d_version_1 & 0xff;
	CAN_tx_low[0] |= (tmp << 24);			// 0-Byte, version-1

	tmp = 0x00000000;
	tmp = d_version_2 & 0xff;
	CAN_tx_low[0] |= (tmp << 16);			// 1-Byte, version-2

	tmp = 0x00000000;
	tmp = d_release_1 & 0xff;
	CAN_tx_low[0] |= (tmp << 8);			// 2-Byte, release-1

	tmp = 0x00000000;
	tmp = d_release_2 & 0xff;
	CAN_tx_low[0] |= (tmp << 0);			// 3-Byte, release-2

	//----------------------------------------------------------------------------------------------
	tmp = 0x00000000;
	tmp = d_year & 0xff;
	CAN_tx_high[0] |= (tmp << 24);			// 4-Byte, year

	tmp = 0x00000000;
	tmp = ((d_year >> 8) & 0xff);
	CAN_tx_high[0] |= (tmp << 16);			// 5-Byte, year

	tmp = 0x00000000;
	tmp = d_month & 0xff;
	CAN_tx_high[0] |= (tmp << 8);			// 6-Byte, month

	tmp = 0x00000000;
	tmp = d_date & 0xff;
	CAN_tx_high[0] |= (tmp << 0);			// 7-Byte, date
}

void DETECT_COMMAND_211()
{
	Uint32 tmp = 0x00000000;
	CAN_tx_low[0] = 0x00000000;
	CAN_tx_high[0] = 0x00000000;

	//-------------------------------------------------------------
	// 제어명령 응답
	tmp = 0x00000000;
	tmp = rsp_init_start & 0x01;
	CAN_tx_low[0] |= (tmp << 24);	// 0-Byte(24)

	tmp = 0x00000000;
	tmp = rsp_beam_shutter & 0x01;
	CAN_tx_low[0] |= (tmp << 25);	// 0-Byte(25)

	tmp = 0x00000000;
	tmp = rsp_operate & 0x01;
	CAN_tx_low[0] |= (tmp << 26);	// 0-Byte(26)

	tmp = 0x00000000;
	tmp = rsp_diag & 0x01;
	CAN_tx_low[0] |= (tmp << 27);	// 0-Byte(27)

	tmp = 0x00000000;
	tmp = rsp_stop & 0x01;
	CAN_tx_low[0] |= (tmp << 28);	// 0-Byte(28)

	tmp = 0x00000000;
	tmp = rsp_req_state & 0x01;
	CAN_tx_low[0] |= (tmp << 29);	// 0-Byte(29)

	tmp = 0x00000000;
	tmp = rsp_req_version & 0x01;
	CAN_tx_low[0] |= (tmp << 30);	// 0-Byte(30)
}

void DETECT_TRIGGER_221()
{
	Uint32 tmp = 0x00000000;
	CAN_tx_low[9] = 0x00000000;
	CAN_tx_high[9] = 0x00000000;

	tmp = 0x00000000;
	tmp = rsp_trigger_index & 0xffffffff;
	CAN_tx_low[9] |= (tmp & 0xff) << 24;			// 0-Byte
	CAN_tx_low[9] |= ((tmp >> 8) & 0xff) << 16;		// 1-Byte
	CAN_tx_low[9] |= ((tmp >> 16) & 0xff) << 8;		// 2-Byte
	CAN_tx_low[9] |= ((tmp >> 24) & 0xff) << 0;		// 3-Byte

    tmp = 0x00000000;
	tmp = rsp_trigger_state & 0xff;
	CAN_tx_high[9] |= (tmp << 24);					// 4-Byte
}

void DETECT_ENERGY_222()
{
	Uint32 tmp = 0x00000000;
	CAN_tx_low[4] = 0x00000000;
	CAN_tx_high[4] = 0x00000000;

	tmp = 0x00000000;
	//rsp_energy_index = rsp_trigger_index;
	//---------------------------------------------------------
	tmp = rsp_energy_index;
	CAN_tx_low[4] |= (tmp & 0xff) << 24;			// 0-Byte
	CAN_tx_low[4] |= ((tmp >> 8) & 0xff) << 16;		// 1-Byte
	CAN_tx_low[4] |= ((tmp >> 16) & 0xff) << 8;		// 2-Byte
	CAN_tx_low[4] |= ((tmp >> 24) & 0xff) << 0;		// 3-Byte

    tmp = 0x00000000;
	rsp_energy_meter = 0x00000000;
	//---------------------------------------------------------
	rsp_energy_meter = (unsigned short)(Mean_Peak * 100.0);
	tmp = rsp_energy_meter;
	CAN_tx_high[4] |= (tmp & 0xff) << 24;			// 4-Byte
	CAN_tx_high[4] |= ((tmp >> 8) & 0xff) << 16;	// 5-Byte

	tmp = 0x00000000;
	//---------------------------------------------------------
	rsp_energy_hv_peak = cmd_energy_hv_peak;
	tmp = rsp_energy_hv_peak & 0xffff;
	CAN_tx_high[4] |= (tmp & 0xff) << 8;			// 6-Byte
	CAN_tx_high[4] |= ((tmp >> 8) & 0xff) << 0;		// 7-Byte
}

void DETECT_LASER_SET_223()
{
	Uint32 tmp = 0x00000000;
	CAN_tx_low[0] = 0x00000000;
	CAN_tx_high[0] = 0x00000000;

	tmp = rsp_laser_set_action & 0xff;
	CAN_tx_low[0] = (tmp << 24);					// 0-Byte

	tmp = 0x00000000;
	rsp_laser_set_hv = cmd_laser_set_hv;
	tmp = rsp_laser_set_hv & 0xffff;
	CAN_tx_low[0] |= (tmp & 0xff) << 16;			// 1-Byte
	CAN_tx_low[0] |= ((tmp >> 8) & 0xff) << 8;		// 2-Byte

	tmp = 0x00000000;
	rsp_laser_set_period = cmd_laser_set_period;
	tmp = rsp_laser_set_period & 0xffff;
	CAN_tx_low[0] |= (tmp & 0xff) << 0;				// 3-Byte
	CAN_tx_high[0] |= ((tmp >> 8) & 0xff) << 24;	// 4-Byte

	tmp = 0x00000000;
	rsp_laser_set_repeat = cmd_laser_set_repeat;
	tmp = rsp_laser_set_repeat & 0xffff;
	CAN_tx_high[0] |= (tmp & 0xff) << 16;			// 5-Byte
	CAN_tx_high[0] |= ((tmp >> 8) & 0xff) << 8;		// 6-Byte
}

void DETECT_LASER_CONFIG_224()
{
	Uint32 tmp = 0x00000000;

	CAN_tx_low[2] = 0x00000000;
	CAN_tx_high[2] = 0x00000000;

	//--------------------------------------------------------
	// 0-Byte
	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_reset & 0x01;
	CAN_tx_low[2] |= (tmp << 24);	// 0-Byte(24)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_trigger & 0x01;
	CAN_tx_low[2] |= (tmp << 25);	// 0-Byte(25)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_mode & 0x01;
	CAN_tx_low[2] |= (tmp << 26);	// 0-Byte(26)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_flu_injector & 0x01;
	CAN_tx_low[2] |= (tmp << 27);	// 0-Byte(27)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_cool_fan & 0x01;
	CAN_tx_low[2] |= (tmp << 28);	// 0-Byte(28)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_sol_1 & 0x01;
	CAN_tx_low[2] |= (tmp << 29);	// 0-Byte(29)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_sol_2 & 0x01;
	CAN_tx_low[2] |= (tmp << 30);	// 0-Byte(30)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_sol_3 & 0x01;
	CAN_tx_low[2] |= (tmp << 31);	// 0-Byte(31)

	//--------------------------------------------------------
	// 1-Byte
	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_sol_4 & 0x01;
	CAN_tx_low[2] |= (tmp << 16);	// 1-Byte(16)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_motor_1 & 0x01;
	CAN_tx_low[2] |= (tmp << 17);	// 1-Byte(17)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_motor_2 & 0x01;
	CAN_tx_low[2] |= (tmp << 18);	// 1-Byte(18)

	tmp = 0x00000000;
	tmp = rsp_laser_ctrl_line.ls_vac_pump & 0x01;
	CAN_tx_low[2] |= (tmp << 19);	// 1-Byte(19)
}

void DETECT_STATE_232()
{
	Uint32 tmp = 0x00000000;
	CAN_tx_low[5] = 0x00000000;
	CAN_tx_high[5] = 0x00000000;

	tmp = 0x00000000;
	tmp |= GetDetectState() & 0xff;
	CAN_tx_low[5] = (tmp << 24);	// 0-Byte
}

void DETECT_STATUS1_241()
{
	Uint32 STANDBY_TIME = 0x00000000;
	Uint32 GAS_PRESSURE = 0x00000000;
	Uint32 EX_LASER_TEMP = 0x00000000;
	Uint32 FILAMENT_M = 0x00000000;

	CAN_tx_low[6] = 0x00000000;
	CAN_tx_high[6] = 0x00000000;

	rsp_standby_time = sec_cnt;
	STANDBY_TIME = rsp_standby_time;

	CAN_tx_low[6] |= (STANDBY_TIME & 0xff) << 24;				// 0-Byte
	CAN_tx_low[6] |= ((STANDBY_TIME >> 8) & 0xff) << 16;		// 1-Byte

//	GAS_PRESSURE = (unsigned short)((AD_GASPRE_data / 5) * 6500);

	// y = (1.03 * x + 28.8)
	GAS_PRESSURE = (unsigned short)((AD_GASPRE_data * 1034) + 28.8);

	CAN_tx_low[6] |= (GAS_PRESSURE & 0xff) << 8;				// 2-Byte
	CAN_tx_low[6] |= ((GAS_PRESSURE >> 8) & 0xff) << 0;			// 3-Byte

	EX_LASER_TEMP = (unsigned short)(AD_TEMP_data * 10 + 10000);

	CAN_tx_high[6] |= (EX_LASER_TEMP & 0xff) << 24;				// 4-Byte
	CAN_tx_high[6] |= ((EX_LASER_TEMP >> 8) & 0xff) << 16;		// 5-Byte

	FILAMENT_M = (unsigned short)(AD_FILAMT_data * 10);

	CAN_tx_high[6] |= (FILAMENT_M & 0xff) << 8;					// 6-Byte
	CAN_tx_high[6] |= ((FILAMENT_M >> 8) & 0xff) << 0;			// 7-Byte
}

void DETECT_STATUS2_242()
{
	Uint32 tmp = 0x00000000;

	CAN_tx_low[7] = 0x00000000;
	CAN_tx_high[7] = 0x00000000;

	tmp = 0x00000000;
	tmp = (unsigned short)(AD_CONTROLTH_data * 10 + 10000);

	CAN_tx_low[7] |= (tmp & 0xff) << 24;			// 0-Byte
	CAN_tx_low[7] |= ((tmp >> 8) & 0xff) << 16;		// 1-Byte
}

void DETECT_FAULT_281()
{
	Uint32 tmp = 0x00000000;

	CAN_tx_low[8] = 0x00000000;
	CAN_tx_high[8] = 0x00000000;

	//--------------------------------------------------------
	// 0-Byte
	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_init & 0x01;
	CAN_tx_low[8] |= (tmp << 24);	// 0-Byte(24)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_config & 0x01;
	CAN_tx_low[8] |= (tmp << 25);	// 0-Byte(25)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_operate & 0x01;
	CAN_tx_low[8] |= (tmp << 26);	// 0-Byte(26)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_beam_shutter & 0x01;
	CAN_tx_low[8] |= (tmp << 27);	// 0-Byte(27)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_emeter_operate & 0x01;
	CAN_tx_low[8] |= (tmp << 28);	// 0-Byte(28)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_trig & 0x01;
	CAN_tx_low[8] |= (tmp << 29);	// 0-Byte(29)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_temp & 0x01;
	CAN_tx_low[8] |= (tmp << 30);	// 0-Byte(30)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_gas_pressure & 0x01;
	CAN_tx_low[8] |= (tmp << 31);	// 0-Byte(31)

	//--------------------------------------------------------
	// 1-Byte
	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_gas_pressure & 0x01;
	CAN_tx_low[8] |= (tmp << 16);	// 1-Byte(16)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_filament_m & 0x01;
	CAN_tx_low[8] |= (tmp << 17);	// 1-Byte(17)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_hv_peak & 0x01;
	CAN_tx_low[8] |= (tmp << 18);	// 1-Byte(18)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_emeter & 0x01;
	CAN_tx_low[8] |= (tmp << 19);	// 1-Byte(19)

	tmp = 0x00000000;
	tmp = rsp_dt_fault.flt_laser_period_config & 0x01;
	CAN_tx_low[8] |= (tmp << 20);	// 1-Byte(20)
}

void Can_PROCESSING(void)
{
	if( CAN_TX_INT_FLAG == 1 )
	{
		Can_Process_TX();
		CAN_TX_INT_FLAG = 0;
	}
}

//------------------------------------------------------------------------------------------------
// CAN-A MBOX1
void SendDataToECanA_Mailbox1(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS1;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS1;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME1 = 0;
	ECanaMboxes.MBOX1.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX1.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME1 = 1;

	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX1.MDL.all = low_data;
	ECanaMboxes.MBOX1.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS1;

	ECanaRegs.CANTRS.bit.TRS1 = 1;
}

void SendDataToECanA_Mailbox2(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS2;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS2;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME2 = 0;
	ECanaMboxes.MBOX2.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX2.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME2 = 1;

	ECanaMboxes.MBOX2.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX2.MDL.all = low_data;
	ECanaMboxes.MBOX2.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS2;

	ECanaRegs.CANTRS.bit.TRS2 = 1;
}

void SendDataToECanA_Mailbox3(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS3;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS3;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME3 = 0;
	ECanaMboxes.MBOX3.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX3.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME3 = 1;

	ECanaMboxes.MBOX3.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX3.MDL.all = low_data;
	ECanaMboxes.MBOX3.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS3;

	ECanaRegs.CANTRS.bit.TRS3 = 1;
}

void SendDataToECanA_Mailbox4(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS4;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS4;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME4 = 0;
	ECanaMboxes.MBOX4.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX4.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME4 = 1;

	ECanaMboxes.MBOX4.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX4.MDL.all = low_data;
	ECanaMboxes.MBOX4.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS4;

	ECanaRegs.CANTRS.bit.TRS4 = 1;
}

void SendDataToECanA_Mailbox5(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS5;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS5;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME5 = 0;
	ECanaMboxes.MBOX5.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX5.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME5 = 1;

	ECanaMboxes.MBOX5.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX5.MDL.all = low_data;
	ECanaMboxes.MBOX5.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS5;

	ECanaRegs.CANTRS.bit.TRS5 = 1;
}

void SendDataToECanA_Mailbox6(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS6;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS6;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME6 = 0;
	ECanaMboxes.MBOX6.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX6.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME6 = 1;

	ECanaMboxes.MBOX6.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX6.MDL.all = low_data;
	ECanaMboxes.MBOX6.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS6;

	ECanaRegs.CANTRS.bit.TRS6 = 1;
}

void SendDataToECanA_Mailbox7(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS7;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS7;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME7 = 0;
	ECanaMboxes.MBOX7.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX7.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME7 = 1;

	ECanaMboxes.MBOX7.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX7.MDL.all = low_data;
	ECanaMboxes.MBOX7.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS7;

	ECanaRegs.CANTRS.bit.TRS7 = 1;
}

void SendDataToECanA_Mailbox8(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS8;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS8;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME8 = 0;
	ECanaMboxes.MBOX8.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX8.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME8 = 1;

	ECanaMboxes.MBOX8.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX8.MDL.all = low_data;
	ECanaMboxes.MBOX8.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS8;

	ECanaRegs.CANTRS.bit.TRS8 = 1;
}

void SendDataToECanA_Mailbox9(long id, BYTE length, LONG low_data, LONG high_data)
{
	char temp_trs = 0;
	WORD CAN_ERR_COUNT = 0;

	temp_trs = ECanaRegs.CANTRS.bit.TRS9;

	while(temp_trs == 1)
	{
		temp_trs = ECanaRegs.CANTRS.bit.TRS9;
		if(CAN_ERR_COUNT > 5000) break;
		CAN_ERR_COUNT ++;
	}

	id = id & 0x000007ff;
	ECanaRegs.CANME.bit.ME9 = 0;
	ECanaMboxes.MBOX9.MSGID.all = 0x40000000;
	ECanaMboxes.MBOX9.MSGID.bit.STDMSGID = id;
	ECanaRegs.CANME.bit.ME9 = 1;

	ECanaMboxes.MBOX9.MSGCTRL.bit.DLC = length & 0xf;
	ECanaMboxes.MBOX9.MDL.all = low_data;
	ECanaMboxes.MBOX9.MDH.all = high_data;

	temp_trs = ECanaRegs.CANTRS.bit.TRS9;

	ECanaRegs.CANTRS.bit.TRS9 = 1;
}

void SendDataToECanB(long id, BYTE length, LONG low_data, LONG high_data)
{
	if(ECanbRegs.CANTRS.bit.TRS1 == 1)
	{
		while(ECanbRegs.CANTRS.bit.TRS1 == 1)
		{
			if(CAN_TX_READY_FLAG == 1) break;
		}
		CAN_TX_READY_FLAG = 0;
	}

	id = id & 0x000007ff;
	ECanbRegs.CANME.bit.ME1 = 0;
	ECanbMboxes.MBOX1.MSGID.all = 0x40000000;
	ECanbMboxes.MBOX1.MSGID.bit.STDMSGID = id;
	ECanbRegs.CANME.bit.ME1 = 1;

	ECanbMboxes.MBOX1.MSGCTRL.bit.DLC = length & 0xf;
	ECanbMboxes.MBOX1.MDL.all = low_data;
	ECanbMboxes.MBOX1.MDH.all = high_data;

	ECanbRegs.CANTRS.bit.TRS1 = 1;
}

void InitECanAGpio(void)
{
	EALLOW;

	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	    // Enable pull-up for GPIO18 (CANRXA)
	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	    // Enable pull-up for GPIO19 (CANTXA)

// Set qualification for selected CAN pins to asynch only
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3;   // Asynch qual for GPIO18 (CANRXA)

// Configure eCAN-A pins using GPIO regs
// This specifies which of the possible GPIO pins will be eCAN functional pins.
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 3;	// Configure GPIO18 for CANRXA operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;	// Configure GPIO19 for CANTXA operation
    EDIS;
}

void InitECanBGpio(void)
{
	EALLOW;

	GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;	    // Enable pull-up for GPIO20 (CANTXB)
	GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;	    // Enable pull-up for GPIO21 (CANRXB)

// Set qualification for selected CAN pins to asynch only
// Inputs are synchronized to SYSCLKOUT by default.
// This will select asynch (no qualification) for the selected pins.
	GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 3;   // Asynch qual for GPIO21 (CANRXB)

// Configure eCAN-A pins using GPIO regs
// This specifies which of the possible GPIO pins will be eCAN functional pins.
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;	// Configure GPIO18 for CANRXB operation
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;	// Configure GPIO19 for CANTXB operation
    EDIS;
}

void InitECanA(void)		// Initialize eCAN-A module
{
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

struct ECAN_REGS ECanaShadow;

	EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
    ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
    ECanaShadow.CANTIOC.bit.TXFUNC = 1;
    ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

    ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
    ECanaShadow.CANRIOC.bit.RXFUNC = 1;
    ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
									// HECC mode also enables time-stamping feature
/*
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
*/
/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

    ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;

    ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanaRegs.CANTA.all	= 0xFFFFFFFF;	/* Clear all TAn bits */

	ECanaRegs.CANRMP.all = 0xFFFFFFFF;	/* Clear all RMPn bits */

	ECanaRegs.CANGIF0.all = 0xFFFFFFFF;	/* Clear all interrupt flag bits */
	ECanaRegs.CANGIF1.all = 0xFFFFFFFF;


/* Configure bit timing parameters for eCANA*/
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

// add Auto-Bus ON 2010.12.17
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.ABO = 1 ;            // Set ABO = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
//-------------------------------------------------------------

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
	{
	    ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 1 );  		// Wait for CCE bit to be set..

    ECanaShadow.CANBTC.all = 0;

	//* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock)
	// - default. Bit rate = 1 Mbps See Note at End of File
//	ECanaShadow.CANBTC.bit.BRPREG = 4;
//	ECanaShadow.CANBTC.bit.TSEG2REG = 2;
//	ECanaShadow.CANBTC.bit.TSEG1REG = 10;

	// CAN bps 500.0Kbps Setting 2010.08.25.
	ECanaShadow.CANBTC.bit.BRPREG = 9;
	ECanaShadow.CANBTC.bit.TSEG2REG = 2;
	ECanaShadow.CANBTC.bit.TSEG1REG = 10;

	// CAN bps 250.0Kbps Setting 2010.08.20.
//	ECanaShadow.CANBTC.bit.BRPREG = 19;
//	ECanaShadow.CANBTC.bit.TSEG2REG = 2;
//	ECanaShadow.CANBTC.bit.TSEG1REG = 10;

    ECanaShadow.CANBTC.bit.SAM = 1;
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CCR = 0 ;            		// Set CCR = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    ECanaShadow.CANES.all = ECanaRegs.CANES.all;

    do
    {
       ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

	// Disable all Mailboxes
 	ECanaRegs.CANME.all = 0;						// Required before writing the MSGIDs

    EDIS;
}

void InitECanB(void)		// Initialize eCAN-A module
{
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. This is especially true while writing to/reading from a bit
 (or group of bits) among bits 16 - 31 */

struct ECAN_REGS ECanbShadow;

	EALLOW;		// EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
    ECanbShadow.CANTIOC.all = ECanbRegs.CANTIOC.all;
    ECanbShadow.CANTIOC.bit.TXFUNC = 1;
    ECanbRegs.CANTIOC.all = ECanbShadow.CANTIOC.all;

    ECanbShadow.CANRIOC.all = ECanbRegs.CANRIOC.all;
    ECanbShadow.CANRIOC.bit.RXFUNC = 1;
    ECanbRegs.CANRIOC.all = ECanbShadow.CANRIOC.all;

/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
									// HECC mode also enables time-stamping feature
/*
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.SCB = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
*/
/* Initialize all bits of 'Master Control Field' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

    ECanbMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX1.MSGCTRL.all = 0x00000000;

    ECanbMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX3.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX4.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX5.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX6.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX7.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX8.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX9.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX10.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX11.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX12.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX13.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX14.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX15.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX16.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX17.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX18.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX19.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX20.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX21.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX22.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX23.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX24.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX25.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX26.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX27.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX28.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX29.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX30.MSGCTRL.all = 0x00000000;
    ECanbMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//	as a matter of precaution.

	ECanbRegs.CANTA.all	= 0xFFFFFFFF;		/* Clear all TAn bits */
	ECanbRegs.CANRMP.all = 0xFFFFFFFF;		/* Clear all RMPn bits */
	ECanbRegs.CANGIF0.all = 0xFFFFFFFF;		/* Clear all interrupt flag bits */
	ECanbRegs.CANGIF1.all = 0xFFFFFFFF;

/* Configure bit timing parameters for eCANA*/
	ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 1 ;            			// Set CCR = 1
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

// add Auto-Bus ON 2010.12.17
    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.ABO = 1 ;            			// Set ABO = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
	{
	    ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 1 );  				// Wait for CCE bit to be set..

    ECanbShadow.CANBTC.all = 0;

	//* The following block for all 150 MHz SYSCLKOUT (75 MHz CAN clock)
	// - default. Bit rate = 1 Mbps See Note at End of File
//	ECanbShadow.CANBTC.bit.BRPREG = 4;
//	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
//	ECanbShadow.CANBTC.bit.TSEG1REG = 10;

	// CAN bps 500.0Kbps Setting 2010.08.25.
	ECanbShadow.CANBTC.bit.BRPREG = 9;
	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
	ECanbShadow.CANBTC.bit.TSEG1REG = 10;

	// CAN bps 250.0Kbps Setting 2010.08.20.
//	ECanbShadow.CANBTC.bit.BRPREG = 19;
//	ECanbShadow.CANBTC.bit.TSEG2REG = 2;
//	ECanbShadow.CANBTC.bit.TSEG1REG = 10;

    ECanbShadow.CANBTC.bit.SAM = 1;
    ECanbRegs.CANBTC.all = ECanbShadow.CANBTC.all;

    ECanbShadow.CANMC.all = ECanbRegs.CANMC.all;
	ECanbShadow.CANMC.bit.CCR = 0 ;            			// Set CCR = 0
    ECanbRegs.CANMC.all = ECanbShadow.CANMC.all;

    ECanbShadow.CANES.all = ECanbRegs.CANES.all;

    do
    {
       ECanbShadow.CANES.all = ECanbRegs.CANES.all;
    } while(ECanbShadow.CANES.bit.CCE != 0 ); 		// Wait for CCE bit to be  cleared..

	// Disable all Mailboxes
 	ECanbRegs.CANME.all = 0;		// Required before writing the MSGIDs

    EDIS;
}

void init_can(void)
{
	// Initialize CAN-A/B peripheral with 1M Bps
	// refer to DSP2833x_ECan.c file
	InitECanAGpio();
	InitECanA();
//	InitECanBGpio();
//	InitECanB();

	// Initialize CAN-A Transmiter/Receiver MailBox
	// CAN-A MBOX0 -> Receiver MailBox
	// CAN-A MBOX1 -> Transmiter MailBox
//	ECanaLAMRegs.LAM0.all = 0x1fffffff; 	  // MBOX1 acceptance mask(IDE match, IDs is don't care)
//	ECanaMboxes.MBOX0.MSGID.all = 0xc0000000; // IDE=1, AME=1  CAN2.0B
//	ECanaMboxes.MBOX0.MSGID.all = 0x40000000; // IDE=0, AME=1  CAN2.0A

/*
	ECanaLAMRegs.LAM0.bit.LAMI = 0;
	ECanaLAMRegs.LAM0.bit.LAM_H = 0;

	ECanaRegs.CANGAM.bit.AMI = 1;
	ECanaRegs.CANGAM.bit.GAM2816 = 0;

//	ECanaMboxes.MBOX0.MSGID.all = 0xc0000000; // IDE=1, AME=1  CAN2.0B
//	ECanaMboxes.MBOX0.MSGID.all = 0x40000000; // IDE=0, AME=1  CAN2.0A
	ECanaMboxes.MBOX0.MSGID.bit.IDE = 0;
	ECanaMboxes.MBOX0.MSGID.bit.AME = 1;
	ECanaMboxes.MBOX0.MSGID.bit.AAM = 1;
	ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = LDC_RX_MASK;
*/

	ECanaLAMRegs.LAM0.bit.LAMI = 1;
//	ECanaLAMRegs.LAM0.bit.LAM_H = (0x00B << 2);
	//ECanaLAMRegs.LAM0.bit.LAM_H = 0x002C;
	ECanaLAMRegs.LAM0.bit.LAM_H = 0x02ff;			// MASK

	ECanaRegs.CANGAM.bit.AMI = 1;
//	ECanaRegs.CANGAM.bit.GAM2816 = (0x00B << 2);
//	ECanaRegs.CANGAM.bit.GAM2816 = 0x002C;
	ECanaRegs.CANGAM.bit.GAM2816 = 0x000C;	//(0x0003 << 2)

//	ECanaMboxes.MBOX0.MSGID.all = 0xc0000000; // IDE=1, AME=1  CAN2.0B
//	ECanaMboxes.MBOX0.MSGID.all = 0x40000000; // IDE=0, AME=1  CAN2.0A
	ECanaMboxes.MBOX0.MSGID.bit.IDE = 0;
	ECanaMboxes.MBOX0.MSGID.bit.AME = 1;
	ECanaMboxes.MBOX0.MSGID.bit.AAM = 1;
	ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = RX_MASK;

	ECanaRegs.CANME.bit.ME0 = 1;
	ECanaRegs.CANMD.bit.MD0 = 1; // Receiver
	ECanaRegs.CANMD.bit.MD1 = 0; // Transmiter

	// Initialize CAN-A interrupt vector
	EALLOW;
	PieVectTable.ECAN0INTA = &ECAN0INTA_ISR;  // Rx
	PieVectTable.ECAN1INTA = &ECAN1INTA_ISR;  // Tx
	EDIS;

	EALLOW;
	// Initialize CAN-A/B Receiver and Transmiter MailBox interrupt level
	// CAN-A/B Receiver interrupt level   -> 0
	// CAN-A/B Transmiter interrupt level -> 1
	ECanaRegs.CANMIL.bit.MIL0 = 0;
	ECanaRegs.CANMIL.bit.MIL1 = 1;

	// Initialize CAN-A/B Receiver and Transmiter MailBox interrupt mask
	ECanaRegs.CANMIM.bit.MIM0 = 1;
	ECanaRegs.CANMIM.bit.MIM1 = 1;

	// Initialize CAN-A/B Global interrupt mask
	ECanaRegs.CANGIM.bit.I0EN = 1;
	ECanaRegs.CANGIM.bit.I1EN = 1;
	EDIS;

	// Enable CPU INT9 for CAN-A/B interrupt
	IER |= M_INT9;

    // Enable CAN-A/B INT0/1 in the PIE: Group 9 interrupt 5/6/7/8
	PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
	PieCtrlRegs.PIEIER9.bit.INTx6 = 1;

// Initialize CAN-B Transmiter/Receiver MailBox
// CAN-B MBOX0 -> Receiver MailBox
// CAN-B MBOX1 -> Transmiter MailBox
//	ECanbLAMRegs.LAM0.all = 0x1fffffff; 	  // MBOX1 acceptance mask(IDE match, IDs is don't care)
//	ECanbMboxes.MBOX0.MSGID.all = 0xc0000000; // IDE=1, AME=1  CAN2.0B
//	ECanbMboxes.MBOX0.MSGID.all = 0x40000000; // IDE=0, AME=1  CAN2.0A

/*
	ECanbLAMRegs.LAM0.bit.LAMI = 0;
	ECanbLAMRegs.LAM0.bit.LAM_H = 0;

	ECanbRegs.CANGAM.bit.AMI = 1;
	ECanbRegs.CANGAM.bit.GAM2816 = 0;

	ECanbMboxes.MBOX0.MSGID.bit.IDE = 0;
	ECanbMboxes.MBOX0.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX0.MSGID.bit.AAM = 1;
	ECanbMboxes.MBOX0.MSGID.bit.STDMSGID = LDC_RX_MASK;
*/

/*
	ECanbLAMRegs.LAM0.bit.LAMI = 1;
//	ECanbLAMRegs.LAM0.bit.LAM_H = (0x00B << 2);
	ECanbLAMRegs.LAM0.bit.LAM_H = 0x002C;

	ECanbRegs.CANGAM.bit.AMI = 1;
//	ECanbRegs.CANGAM.bit.GAM2816 = (0x00B << 2);
	ECanbRegs.CANGAM.bit.GAM2816 = 0x002C;

	ECanbMboxes.MBOX0.MSGID.bit.IDE = 0;
	ECanbMboxes.MBOX0.MSGID.bit.AME = 1;
	ECanbMboxes.MBOX0.MSGID.bit.AAM = 1;
	ECanbMboxes.MBOX0.MSGID.bit.STDMSGID = LDC_RX_MASK;

	ECanbRegs.CANME.bit.ME0 = 1;
	ECanbRegs.CANMD.bit.MD0 = 1; // Receiver
	ECanbRegs.CANMD.bit.MD1 = 0; // Transmiter

	// Initialize CAN-A interrupt vector
	EALLOW;
	PieVectTable.ECAN0INTB = &ECAN0INTB_ISR;  // rx
	PieVectTable.ECAN1INTB = &ECAN1INTB_ISR;  // tx
	EDIS;

	EALLOW;
	// Initialize CAN-A/B Receiver and Transmiter MailBox interrupt level
	// CAN-A/B Receiver interrupt level   -> 0
	// CAN-A/B Transmiter interrupt level -> 1
	ECanbRegs.CANMIL.bit.MIL0 = 0;
	ECanbRegs.CANMIL.bit.MIL1 = 1;

	// Initialize CAN-A/B Receiver and Transmiter MailBox interrupt mask
	ECanbRegs.CANMIM.bit.MIM0 = 1;
	ECanbRegs.CANMIM.bit.MIM1 = 1;

	// Initialize CAN-A/B Global interrupt mask
	ECanbRegs.CANGIM.bit.I0EN = 1;
	ECanbRegs.CANGIM.bit.I1EN = 1;
	EDIS;

	// Enable CPU INT9 for CAN-A/B interrupt
	IER |= M_INT9;

    // Enable CAN-A/B INT0/1 in the PIE: Group 9 interrupt 5/6/7/8
	PieCtrlRegs.PIEIER9.bit.INTx7 = 1;
	PieCtrlRegs.PIEIER9.bit.INTx8 = 1;
*/
}
