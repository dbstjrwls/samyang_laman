
//------------------------------------------------------------------------
// Structure Data Frame
// SOF | LEN | MSG ID | DATA | CHECKSUM | EOT
// (1) | (1) | (1)    | (N)  | (1)      | (1)
//------------------------------------------------------------------------

#define START_FRAME             0x7E
#define END_TRANSMISSION        0xFE

#define MSG_TXID_INIT           0xA1
#define MSG_TXID_INSP           0xA2
#define MSG_TXID_CMD            0xA3

#define MSG_RXID_RSP_INIT       0xAA
#define MSG_RXID_RSP_STS        0xAB
#define MSG_RXID_RSP_INSP       0xAC
#define MSG_RXID_RSP_CMD        0xAD
#define MSG_RXID_RSP_LAUNCH     0xAE

#define MSG_SIZE_INIT           1
#define MSG_SIZE_INSP           1
#define MSG_SIZE_CMD            3

#define MSG_SIZE_RSP_INIT       10
#define MSG_SIZE_RSP_STS        5
#define MSG_SIZE_RSP_INSP       15
#define MSG_SIZE_RSP_CMD        3
#define MSG_SIZE_RSP_LAUNCH     4


typedef struct data_frame
{
    unsigned char SOF;
    unsigned char LEN;
    unsigned char MSG_ID;
    unsigned char DATA[15];
    unsigned char CHECKSUM;
    unsigned char EOT;
} data_frame_type;

//------------------------------------------------------------------------
// 1.연동 초기화
//------------------------------------------------------------------------
typedef struct rsp_frame_init
{
    unsigned char init_ack;     // 연동 초기화 응답
    unsigned char wavelen_sel;  // 파장선택 설정
    unsigned char launch_mode;  // 단발/연발 설정
    unsigned char period;       // 발사 주기
    unsigned long launch_cnt;   // 발사 횟수(5-MSB, 8-LSB)
    unsigned char sw_ver;       // SW 버전
    unsigned char module_num;   // 레이저모듈 장착 호기
} rsp_frame_init_type;

//------------------------------------------------------------------------
// 2.주기적 상태정보
//------------------------------------------------------------------------
typedef struct rsp_frame_sts
{
    unsigned char sts_ack;      // 상태정보 응답(0xF2)
    unsigned char cbit;         // CBIT 결과
    unsigned char temp;         // LDA 온도
    unsigned char sts_1;        // 상태 정보 1
    unsigned char sts_2;        // 상태 정보 2
} rsp_frame_sts_type;

//------------------------------------------------------------------------
// 3.레이저모듈 자체점검 결과
//------------------------------------------------------------------------
typedef struct rsp_frame_insp
{
    unsigned char insp_ack;      // 자체점검 요청(0xF2)
    unsigned char power_sts;     // 전원상태 응답
    unsigned char temp_board;    // 온도 및 보드 상태 응답
    unsigned int power_56v;      // 56V 전원상태 (4-MSB, 5-LSB)
    unsigned int power_12v;      // 12V 전원상태 (6-MSB, 7-LSB
    unsigned int power_5v;       // 5V 전원상태  (8-MSB, 9-LSB)
    unsigned char lda_temp;       // LDA 온도
    unsigned char board_temp;    // 보드 온도
    unsigned long launch_cnt;    // 발사 횟수 (12-MSB, 15-LSB)
} rsp_frame_insp_type;

//------------------------------------------------------------------------
// 4.레이저모듈 운용제어 응답
//------------------------------------------------------------------------
typedef struct rsp_frame_cmd
{
    unsigned char operate;        // 운용 제어 명령
    unsigned char launch;         // 발사 제어 명령
    unsigned char launch_period;  // 발사 주기 설정
} rsp_frame_cmd_type;

//------------------------------------------------------------------------
// 5.레이저모듈 발사응답
//------------------------------------------------------------------------
typedef struct rsp_frame_launch
{
    unsigned char launch_ack;     // 발사 제어 응답(0x01)
    unsigned int q_light;         // 광량(2-MSB, 3-LSB)
    unsigned char cbit;           // CBIT 결과
} rsp_frame_launch_type;

//------------------------------------------------------------------------

extern BOOL UART_INTERRUPT;
extern BOOL UART_INTERRUPT_C;
extern BOOL Command_Start;		// 점검에서 쓰임.
extern BOOL sof_flag;
extern char Debug_cmd_flag;
extern char Debug_cmd_flag_c;
extern char rxd;
extern char rxd_c;
