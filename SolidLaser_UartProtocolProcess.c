// 기능2 시작.
//변경.
#include "DSP2833x_28335_Define.h"
#include "SolidLaser_UartProtocolProcess.h"
#include "main.h"
#include "global.h"
#include "ctrl.h"

#define TRUE	1
#define FALSE 	0

enum
{
    eLaser_State_None,
	eLaser_State_Init_Warmup,
	eLaser_State_Warmup_Complete,
	eLaser_State_Charge,
	eLaser_State_Fire_Ready,
	eLaser_State_Fire,
	eLaser_State_Fire_Stop,
	eLaser_State_Max
} _eLASER_STATUS;

///////////////////////////////////
// 구현 완료 #1
// 구현 완료 #2
void LaserControlHandler_100ms()
{
    static volatile u16 state=0;
    static u32 timer100ms = 0;
    u16 FirerFrequency = 1;

	switch(state)
	{
		//--------------------------------------------------------------------------------
		// INIT
		//--------------------------------------------------------------------------------
		case 0:
			System_Ctrl_Init();
	 		total_detect_cnt = Read_Uint32(ADDR_EEPROM);
			state = 100;
			break;

		case 100:
			if(cmd_init_start == 1)
			{
                if(++timer100ms >= 10)	// 1초마다 연동초기화 전송.
                {
                    timer100ms = 0;
                    Laser_Init();
                }

				if((rsp_dt_fault.flt_beam_shutter == 0) && (rsp_dt_fault.flt_laser_init == 0))
				{
					if((LaserRcvInitRsp.Ack == 0xF2) && (LaserRcvPeriodicInfo.StatusInfo2 == 100))      // laser로부터 연동초기와 ack신호가 오고, warmingup이 100% 완료되었으면,
	                {
						Laser_Pwr_Complete = TRUE;
						SetDetectState(ST_INIT_COMPLETE);
						state = 200;
					}
				}
				else if((rsp_dt_fault.flt_beam_shutter == 0) && (rsp_dt_fault.flt_laser_init == 0) && (Laser_Pwr_Complete == FALSE))
				{
					SetDetectState(ST_INIT_RUN);
				}
				else
                {
					SetDetectState(ST_INIT_ERROR);
				}
				//detect_state = ST_INIT_COMPLETE;  	// Test_Mode
				//state = 200; 							// Test_Mode
			}
			else
				SetDetectState(ST_POWER_ON);
			break;

		//--------------------------------------------------------------------------------
		// READY
		//--------------------------------------------------------------------------------
		case 200:
			EX5_HighVoltage_Set(cmd_laser_set_hv);	// Set High-Voltage Parameter   , I2C로 출력전압 세팅.
			SetDetectState(ST_READY_COMPLETE);

			if((cmd_init_start == 1) && (cmd_operate == 1))
            {	// START CONTROL
                if(IsRcvdLaserSetCmd() == TRUE)    // 초기 laser 설정 정보를 can으로부터 수신했다면, 다음으로 진행
                {
                    // 각종 LASER 설정(파장, Fire회수, Frequency 설정 을 해준다.
                    if(cmd_laser_set_hv == 0)           Laser_Set_213nm();
                    else                                        Laser_Set_266nm();

                    if(cmd_laser_set_repeat == 0)    Laser_Set_MultiFire();
                    else                                        Laser_Set_SingleFire();

                    FirerFrequency = 1000/cmd_laser_set_period;
                    if(cmd_laser_set_period == 0) FirerFrequency = 10;
                    if((FirerFrequency <= 0) || (FirerFrequency>20))   FirerFrequency = 10;
                    Laser_Set_Frequency(FirerFrequency);

                    //Laser_ArangeFireOn();
                    //Laser_ArangeFireOff();
                    //Laser_Set_Frequency(10);

                    Laser_ReadyOn();        // 레이저 발사 준비
                    timer100ms = 0;
                    state = 250;
                }
			}
			break;

		case 250 :
			if(++timer100ms >= 11)
            {
                state = 300;      //  laser fire ready 명령 후 1초 대기.
            }
			break;

		//--------------------------------------------------------------------------------
		// START
		//--------------------------------------------------------------------------------
		case 300:
			if((cmd_init_start == 1) && (cmd_operate == 1))
            {
				trig_gen_cmd = 1;
				RST_EX5_DISENABLE();
				MOTOR1_ON();
				SetDetectState(ST_OPERATE_COMPLETE);
			}
			else if((cmd_init_start == 1) && (cmd_operate == 0))
			{
				state = 400;
				SetDetectState(ST_STOP_RUN);
			}
			break;

		//--------------------------------------------------------------------------------
		// STOP
		//--------------------------------------------------------------------------------
		case 400:
			EX5_HighVoltage_Init();
			RST_EX5_ENABLE();
			MOTOR1_OFF();
			Write_Uint32(ADDR_EEPROM, rsp_energy_index);
			I2C_Release();

			laser_trigger_flag = 1;		// Laser Trigger Repeat Flag
			trig_gen_cmd = 0;
			SetDetectState(ST_STOP_COMPLETE);
			state = 500;
			break;

		case 500:
			if((cmd_init_start == 1) && (cmd_operate == 1)) {
				laser_trigger_flag = 0;
				state = 200;
				SetDetectState(ST_OPERATE_RUN);
			}
			break;

		case 600:
			Helium_Vent_Seq();
			break;

		case 700:
			Premix_Vent_Seq();
			break;

		case 800:
			Refill_Seq();
			break;

		default:
			state = 0;
			break;
	}
}

/*
///////////////////////////////////
// 100ms handler
void LaserControlHandler_100ms(void)
{
	static u8 step = eLaser_State_None;
	static u32 timer100ms = 0;

	switch(step)
	{
        case eLaser_State_None:
            TRIG_EX5_HIGH();  		        // Fire trigger I/O 를 HIGH로 세팅 -> 이후 쓰이지 않음.
            total_detect_cnt = Read_Uint32(ADDR_EEPROM);
            step = eLaser_State_Init_Warmup;
            break;

		case eLaser_State_Init_Warmup :
            if(cmd_init_start == TRUE)
            {
                if(++timer100ms >= 10)	// 1초마다 연동초기화 전송.
                {
                    timer100ms = 0;
                    Laser_Init();
                }

                if((LaserRcvInitRsp.Ack == 0xF2) && (LaserRcvPeriodicInfo.StatusInfo2 == 100))      // 연동초기화 ack가 수신되고, warmup 이 100%가 되었으면, 다음으로 진행.
                {
                    SetDetectState(ST_INIT_COMPLETE);
                    step = eLaser_State_Warmup_Complete;
                }
                else
                {
                    SetDetectState(ST_INIT_RUN);
                }
            }
            else
            {
                SetDetectState(ST_POWER_ON);
            }

			break;

		case eLaser_State_Warmup_Complete :
		    SetDetectState(ST_READY_COMPLETE);

            if((cmd_init_start == TRUE) && (cmd_operate == TRUE))
            {
                if(IsRcvdLaserSetCmd() == TRUE)    // 초기 laser 설정 정보를 can으로부터 수신했다면, 다음으로 진행
                {
                    // 각종 LASER 설정(파장, Fire회수, Frequency 설정 을 해준다.

                    //Laser_Set_213nm();
                    //Laser_Set_266nm();
                    //Laser_Set_SingleFire();
                    //Laser_Set_MultiFire();
                    //Laser_ArangeFireOn();
                    //Laser_ArangeFireOff();
                    //Laser_Set_Frequency(10);


                    Laser_ReadyOn();        // 레이저 발사 준비
                    timer100ms = 0;

                    step = eLaser_State_Charge;
                }
            }
			break;

		case eLaser_State_Charge :
			if(++timer100ms >= 11) step = eLaser_State_Fire_Ready;      //  laser fire 명령 후 1초 대기.
			break;

		case eLaser_State_Fire_Ready :
			Laser_ReadyOn();
			step = eLaser_State_Fire;
			break;

		case eLaser_State_Fire :
			// 이 부분에서 단발 연발에 따라 정지하는 제어를 해주면 된다.

			break;

		case eLaser_State_Fire_Stop :
		    // 이 부분에서 레이저 발사 정지시 수행할 것들을 수행하고, eLaser_State_Warmup_Complete step으로 이동한다.
		    step = eLaser_State_Warmup_Complete;

			break;

		default:
			break;
	}

}
*/

void SCICProtocolFunction(u8 bData)     // SCI-C
{
 	volatile u8 Rcv_Checksum = 0;
	static volatile u8 Calc_Checksum = 0;

	static u8 nRcvStep = RECV_STATE_1_SOF;    // 시리얼 프로토콜을 처리하기 위한 Sequence 변수
	static u8 nRcvBytes = 0;                  // SOT, LEN, MSGID, DATA, CHECKSUM, EOT 모두를 더한 수신 데이타의 Byte수
	static u8 nRcvDataCount = 0;              // 수신 데이타 중 순수 Data를 카운터 하기 위한 변수

	static u8 rx_buffer[RX_BUFFER_SIZE];        // 수신된 Data(STX ~ ETX)를 저장하기 위한 배열

    u8 bRecvComp = FALSE;

    switch(nRcvStep)
    {
        case RECV_STATE_99_ERROR:

            if(bData == EOT)
                nRcvStep = RECV_STATE_1_SOF;
            break;

        case RECV_STATE_1_SOF:
            if(bData != SOF)
                nRcvStep = RECV_STATE_99_ERROR;
            else
            {
				Calc_Checksum = 0;
                nRcvBytes = 0;
                nRcvDataCount = 0;

                nRcvStep = RECV_STATE_2_LEN;
            }
            break;

        case RECV_STATE_2_LEN:
			nRcvDataCount = bData;
			if(nRcvDataCount > MAX_RCV_DATA_LEN)
                nRcvStep = RECV_STATE_99_ERROR;
            else
            {
				Calc_Checksum ^= bData;			// LEN, MSGID, DATA 필드의 checksum 계산.(XOR)
                nRcvStep = RECV_STATE_3_MSGID;
            }
			break;

        case RECV_STATE_3_MSGID:
			if((bData != L2R_INIT_RSP) && (bData != L2R_PERIODIC_INFO) && (bData != L2R_SELF_CHECK_RSP) && (bData != L2R_CONTROL_COMMAND_RSP) && (bData != L2R_FIRE_COMMAND_RSP))
				nRcvStep = RECV_STATE_99_ERROR;
			else
			{
				Calc_Checksum ^= bData;			// LEN, MSGID, DATA 필드의 checksum 계산.(XOR)
                nRcvStep = RECV_STATE_4_DATA;
            }
			break;

		case RECV_STATE_4_DATA:
			Calc_Checksum ^= bData;			// LEN, MSGID, DATA 필드의 checksum 계산.(XOR)

            if(--nRcvDataCount == 0) 	nRcvStep = RECV_STATE_5_CHECKSUM;
		    else    					nRcvStep = RECV_STATE_4_DATA;
			break;

        case RECV_STATE_5_CHECKSUM:
			Rcv_Checksum = bData;
			if(Calc_Checksum != Rcv_Checksum)  	nRcvStep = RECV_STATE_99_ERROR;
			else 								nRcvStep = RECV_STATE_6_EOT;
            break;

        case RECV_STATE_6_EOT:
            if(bData != EOT)            nRcvStep = RECV_STATE_99_ERROR;
            else
            {
                nRcvStep = RECV_STATE_1_SOF;
                bRecvComp = TRUE;
            }
            break;
    }

    if(nRcvStep != RECV_STATE_99_ERROR)
    {
        rx_buffer[nRcvBytes] = bData;
        nRcvBytes++;
    }

    if(bRecvComp == TRUE)
	{
		 DoProcess(rx_buffer, nRcvBytes);       // rx_buffer :수신 버퍼, nRcvBytes : 수신 버퍼의 byte수
	}
}

void DoProcess(u8 * bPacket, int nLength)
{
    volatile u8 *pPacket;

	volatile u8 arrPacket[30] = {0,};

	memcpy((void*)arrPacket, (void*)bPacket, nLength);
	pPacket = bPacket;

	Sci_C_Send_Byte('q');
    //Uart1_Send_String("ACK");

    switch (pPacket[PACKET_POSITION_MSGID])
    {
        case L2R_INIT_RSP :
        {
			DoProcessInitRsp((u8*)pPacket, nLength);
            break;
        }
        case L2R_PERIODIC_INFO :
        {
			DoProcessPeriodicInfo((u8*)pPacket, nLength);
		//	Sci_C_Send_String((u8*)arrPacket, nLength);
            break;
        }
        case L2R_SELF_CHECK_RSP :
        {
			DoProcessSelfCheckRsp((u8*)pPacket, nLength);
		//	Sci_C_Send_String((u8*)arrPacket, nLength);
            break;
        }
        case L2R_CONTROL_COMMAND_RSP :
        {
			DoProcessControlCommandRsp((u8*)pPacket, nLength);
		//	Sci_C_Send_String((u8*)arrPacket, nLength);
			break;
        }
        case L2R_FIRE_COMMAND_RSP :
        {
			DoProcessFireCommandRsp((u8*)pPacket, nLength);
		//	Sci_C_Send_String((u8*)arrPacket, nLength);
            break;
        }
        default:
        {
            break;
        }
    }
    //free(pPacket);
}

void DoProcessInitRsp(u8 *bPacket, int nLength)
{
	u16 pos = PACKET_POSITION_DATA;

	LaserRcvInitRsp.Ack			 	= bPacket[pos++];
	LaserRcvInitRsp.Wavelength		= bPacket[pos++];
	LaserRcvInitRsp.SingleOrMulti	= bPacket[pos++];
	LaserRcvInitRsp.FireFrequency	= bPacket[pos++];

	LaserRcvInitRsp.NumberOfFire	= (0xff<<24)&((u32)bPacket[pos++]<<24);
	LaserRcvInitRsp.NumberOfFire	|= (0xff<<16)&((u32)bPacket[pos++]<<16);
	LaserRcvInitRsp.NumberOfFire	|= (0xff<<8)&((u32)bPacket[pos++]<<8);
	LaserRcvInitRsp.NumberOfFire	|= (0xff<<0)&((u32)bPacket[pos++]<<0);

	LaserRcvInitRsp.SwVersion		= bPacket[pos++];
	LaserRcvInitRsp.MachineInfo		= bPacket[pos++];
}

void DoProcessPeriodicInfo(u8 *bPacket, int nLength)
{
	u16 pos = PACKET_POSITION_DATA;

	LaserRcvPeriodicInfo.Ack			= bPacket[pos++];
	LaserRcvPeriodicInfo.CbitResult		= bPacket[pos++];
	LaserRcvPeriodicInfo.LdaTemp		= bPacket[pos++];
	LaserRcvPeriodicInfo.StatusInfo1	= bPacket[pos++];
	LaserRcvPeriodicInfo.StatusInfo2	= bPacket[pos++];
}

void DoProcessSelfCheckRsp(u8 *bPacket, int nLength)
{
	u16 pos = PACKET_POSITION_DATA;
	u32 temp;

	LaserRcvSelfCheckRsp.Ack				= bPacket[pos++];
	LaserRcvSelfCheckRsp.StatusPower		= bPacket[pos++];
	LaserRcvSelfCheckRsp.StatusTempBoard	= bPacket[pos++];

	LaserRcvSelfCheckRsp.Volt56V			= (0xff<<8)&((u16)bPacket[pos++]<<8);
	LaserRcvSelfCheckRsp.Volt56V			|= (0xff<<0)&((u16)bPacket[pos++]<<0);

	LaserRcvSelfCheckRsp.Volt12V			= (0xff<<8)&((u16)bPacket[pos++]<<8);
	LaserRcvSelfCheckRsp.Volt12V			|= (0xff<<0)&((u16)bPacket[pos++]<<0);

	LaserRcvSelfCheckRsp.Volt5V				= (0xff<<8)&((u16)bPacket[pos++]<<8);
	LaserRcvSelfCheckRsp.Volt5V				|= (0xff<<0)&((u16)bPacket[pos++]<<0);

	LaserRcvSelfCheckRsp.TempLda			= bPacket[pos++];
	LaserRcvSelfCheckRsp.TempBoard			= bPacket[pos++];

	LaserRcvSelfCheckRsp.NumberOfFire		= (0xff<<24)&((u32)bPacket[pos++]<<24);
	LaserRcvSelfCheckRsp.NumberOfFire		|= (0xff<<16)&((u32)bPacket[pos++]<<16);
	LaserRcvSelfCheckRsp.NumberOfFire		|= (0xff<<8)&((u32)bPacket[pos++]<<8);
	LaserRcvSelfCheckRsp.NumberOfFire		|= (0xff<<0)&((u32)bPacket[pos++]<<0);
}

void DoProcessControlCommandRsp(u8 *bPacket, int nLength)
{
	u16 pos = PACKET_POSITION_DATA;

	LaserRcvControlCommandRsp.ControlCommand		= bPacket[pos++];
	LaserRcvControlCommandRsp.FireControlCommand	= bPacket[pos++];
	LaserRcvControlCommandRsp.FireFrequency			= bPacket[pos++];
}

void DoProcessFireCommandRsp(u8 *bPacket, int nLength)
{
	u16 pos = PACKET_POSITION_DATA;

	LaserRcvFireCommandRsp.FireControlResponse	= bPacket[pos++];

	LaserRcvFireCommandRsp.AmountOfLaser		= (0xff<<8)&((u16)bPacket[pos++]<<8);
	LaserRcvFireCommandRsp.AmountOfLaser		|= (0xff<<0)&((u16)bPacket[pos++]<<0);

	LaserRcvFireCommandRsp.CbitResult			= bPacket[pos++];
}

void Laser_TX_Test(void)
{
	static u8 i = 0;

	switch(i)
	{
		case 0 :
			Laser_Init();
			break;
		case 1 :
			Laser_SelfCheck();
			break;
		case 2 :
			Laser_FireOn();
			break;
		case 3 :
			Laser_FireOff();
			break;
		case 4 :
			Laser_Set_213nm();
			break;
		case 5 :
			Laser_Set_266nm();
			break;
		case 6 :
			Laser_ReadyOn();
			break;
		case 7 :
			Laser_ReadyOff();
			break;
		case 8 :
			Laser_Set_SingleFire();
			break;
		case 9 :
			Laser_Set_MultiFire();
			break;
		case 10 :
			Laser_ArangeFireOn();
			break;
		case 11 :
			Laser_ArangeFireOff();
			break;
		case 12 :
			Laser_Set_Frequency(5);
			break;
		default :
			break;
	}
	if(++i > 12) i= 0;
}

void Laser_Init(void)
{
	u8 i;
	u8 datalength = R2L_INIT_DATA_LENGTH;
	u8 msgid = R2L_INIT;
	u8 data[R2L_INIT_DATA_LENGTH] = {0xF1};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_SelfCheck(void)
{
	u8 i;
	u8 datalength = R2L_SELF_CHECK_REQ_DATA_LENGTH;
	u8 msgid = R2L_SELF_CHECK_REQ;
	u8 data[R2L_SELF_CHECK_REQ_DATA_LENGTH] = {0xF1};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

//#define LASER_FIRE_CONTROL_BY_IO		// IO가 제대로 안나감. -> 추후 확인.

void Laser_FireOn(void)
{
#ifdef LASER_FIRE_CONTROL_BY_IO
	GpioDataRegs.GPCCLEAR.bit.GPIO76 = TRUE;
#else
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x01, 0x11, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
#endif
}

void Laser_FireOff(void)
{
#ifdef LASER_FIRE_CONTROL_BY_IO
	GpioDataRegs.GPCSET.bit.GPIO76 = TRUE;
#else
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x01, 0x12, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
#endif
}

void Laser_Set_213nm(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x02, 0x21, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_Set_266nm(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x02, 0x22, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_ReadyOn(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x03, 0x31, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_ReadyOff(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x03, 0x32, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_Set_SingleFire(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x04, 0x41, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_Set_MultiFire(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x04, 0x42, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_ArangeFireOn(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x05, 0x51, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_ArangeFireOff(void)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x05, 0x52, GARBAGE_PACKET};
	u8 checksum = 0;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Laser_Set_Frequency(u8 frequency)
{
	u8 i;
	u8 datalength = R2L_CONTROL_COMMAND_DATA_LENGTH;
	u8 msgid = R2L_CONTROL_COMMAND;
	u8 data[R2L_CONTROL_COMMAND_DATA_LENGTH] = {0x06, GARBAGE_PACKET, 0xff};
	u8 checksum = 0;
	data[R2L_CONTROL_COMMAND_DATA_LENGTH-1] = frequency;

	if((frequency < MIN_LASER_FREQUENCY) || (frequency > MAX_LASER_FREQUENCY)) return;

	Sci_C_Send_Byte(SOF);
	Sci_C_Send_Byte(datalength);
	Sci_C_Send_Byte(msgid);

	checksum = datalength^msgid;

	for(i = 0; i < datalength; i++)
	{
		Sci_C_Send_Byte(data[i]);
		checksum ^= data[i];
	}

	Sci_C_Send_Byte(checksum);

	Sci_C_Send_Byte(EOT);
}

void Sci_C_Send_Byte(u8 data)
{
	while(!ScicRegs.SCICTL2.bit.TXRDY);
	ScicRegs.SCITXBUF = data;
}

void Sci_C_Send_2Byte(u16 data)
{
	Sci_C_Send_Byte((u8)((data&0x0000ff00)>>8));
	Sci_C_Send_Byte((u8)((data&0x000000ff)));
}

void Sci_C_Send_4Byte(u32 data)
{
    Sci_C_Send_Byte((u8)((data&0xff000000)>>24));
    Sci_C_Send_Byte((u8)((data&0x00ff0000)>>16));
    Sci_C_Send_Byte((u8)((data&0x0000ff00)>>8));
    Sci_C_Send_Byte((u8)((data&0x000000ff)));
}

void Sci_C_Send_String(u8 *pt, u16 nLength)
{
	int i = 0;
	for(i = 0; i < nLength; i++)	Sci_C_Send_Byte(*pt++);
}
