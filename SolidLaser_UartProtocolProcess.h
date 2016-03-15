// 구현 완료 #2
#ifndef __PROCESS_H
#define __PROCESS_H

#include "DSP2833x_Device.h"

#define RX_BUFFER_SIZE 1280
#define MAX_RCV_DATA_LEN	15	// 고체레이저 프로토콜의 최대 수신 순수 데이타는 15바이트임.

#define GARBAGE_PACKET			0xCC

//------------------------------------------------------------------------
// Structure Data Frame
// SOF | LEN | MSG ID | DATA | CHECKSUM | EOT
// (1) | (1) | (1)    | (N)  | (1)      | (1)
//------------------------------------------------------------------------

// falcon serial protocol
#define SOF						0x7E
#define EOT             		0xFE

// Raman -> Laser Module
#define R2L_INIT				0xA1	// 1Hz
#define R2L_SELF_CHECK_REQ		0xA3
#define R2L_CONTROL_COMMAND		0xA4

// Laser Module -> Raman
#define L2R_INIT_RSP			0xAA
#define L2R_PERIODIC_INFO		0xAB
#define L2R_SELF_CHECK_RSP		0xAC
#define L2R_CONTROL_COMMAND_RSP	0xAD
#define L2R_FIRE_COMMAND_RSP	0xAE

#define R2L_INIT_DATA_LENGTH			1
#define R2L_SELF_CHECK_REQ_DATA_LENGTH	1
#define R2L_CONTROL_COMMAND_DATA_LENGTH	3

// Laser Frequency Min, Max
#define MIN_LASER_FREQUENCY		1
#define MAX_LASER_FREQUENCY		20

// Recieve state
#define RECV_STATE_0_READY	  	0
#define RECV_STATE_1_SOF	  	1
#define RECV_STATE_2_LEN		2
#define RECV_STATE_3_MSGID		3
#define RECV_STATE_4_DATA		4
#define RECV_STATE_5_CHECKSUM	5
#define RECV_STATE_6_EOT		6
#define RECV_STATE_99_ERROR		99

/*
// serial protocol define
enum
{
    RECV_STATE_0_READY,
    RECV_STATE_1_SOF,
    RECV_STATE_2_LEN,
    RECV_STATE_3_MSGID,
    RECV_STATE_4_DATA,
    RECV_STATE_5_CHECKSUM,
    RECV_STATE_6_EOT,
    RECV_STATE_99_ERROR
} _CMD_STATE;
*/

// packet position in protocol
#define PACKET_POSITION_SOF		0
#define PACKET_POSITION_LEN  	1
#define PACKET_POSITION_MSGID 	2
#define PACKET_POSITION_DATA 	3
/*
// USART Packet field
enum
{
    PACKET_SOF = 0,
    PACKET_LEN = 1,
    PACKET_MSGID = 2,
    PACKET_DATA = 3
} _PACKET_ODER;
*/

//------------------------------------------------------------------------
// 1.연동 초기화
//------------------------------------------------------------------------
typedef struct {
	u8 		Ack;
	u8 		Wavelength;
	u8 		SingleOrMulti;
	u8 		FireFrequency;
	u32		NumberOfFire;
	u8		SwVersion;
	u8		MachineInfo;
} LASER_RCV_INIT_RSP;

//------------------------------------------------------------------------
// 2.주기적 상태정보
//------------------------------------------------------------------------
typedef struct {
	u8 		Ack;
	u8 		CbitResult;
	u8 		LdaTemp;
	u8		StatusInfo1;
	u8		StatusInfo2;
} LASER_RCV_PERIODIC_INFO;

//------------------------------------------------------------------------
// 3.레이저모듈 자체점검 결과
//------------------------------------------------------------------------
typedef struct {
	u8 		Ack;
	u8 		StatusPower;
	u8 		StatusTempBoard;
	u16		Volt56V;
	u16		Volt12V;
	u16		Volt5V;
	u8 		TempLda;
	u8 		TempBoard;
	u32 	NumberOfFire;
} LASER_RCV_SELF_CHECK_RSP;

//------------------------------------------------------------------------
// 4.레이저모듈 운용제어 응답
//------------------------------------------------------------------------
typedef struct {
	u8 		ControlCommand;
	u8 		FireControlCommand;
	u8 		FireFrequency;
} LASER_RCV_CONTROL_COMMAND_RSP;

//------------------------------------------------------------------------
// 5.레이저모듈 발사응답
//------------------------------------------------------------------------
typedef struct {
	u8 		FireControlResponse;
	u16 	AmountOfLaser;
	u8 		CbitResult;
} LASER_RCV_FIRE_COMMAND_RSP;

LASER_RCV_INIT_RSP 				LaserRcvInitRsp;
LASER_RCV_PERIODIC_INFO 		LaserRcvPeriodicInfo;
LASER_RCV_SELF_CHECK_RSP 		LaserRcvSelfCheckRsp;
LASER_RCV_CONTROL_COMMAND_RSP	LaserRcvControlCommandRsp;
LASER_RCV_FIRE_COMMAND_RSP		LaserRcvFireCommandRsp;

// sequence control
void LaserControlHandler_100ms(void);

// receive function
void SCICProtocolFunction(u8 bData);
void DoProcess(u8 *bPacket, int nLength);

void DoProcessInitRsp(u8 *bPacket, int nLength);
void DoProcessPeriodicInfo(u8 *bPacket, int nLength);
void DoProcessSelfCheckRsp(u8 *bPacket, int nLength);
void DoProcessControlCommandRsp(u8 *bPacket, int nLength);
void DoProcessFireCommandRsp(u8 *bPacket, int nLength);

// send function
void Sci_C_Send_Byte(u8 data);
void Sci_C_Send_2Byte(u16 data);
void Sci_C_Send_4Byte(u32 data);
void Sci_C_Send_String(u8 *pt, u16 nLength);

void Laser_Init(void);
void Laser_SelfCheck(void);
void Laser_FireOn(void);
void Laser_FireOff(void);
void Laser_Set_213nm(void);
void Laser_Set_266nm(void);
void Laser_ReadyOn(void);
void Laser_ReadyOff(void);
void Laser_Set_SingleFire(void);
void Laser_Set_MultiFire(void);
void Laser_ArangeFireOn(void);
void Laser_ArangeFireOff(void);
void Laser_Set_Frequency(u8 frequency);

void Laser_TX_Test(void);
#endif // PROCESS_H_
