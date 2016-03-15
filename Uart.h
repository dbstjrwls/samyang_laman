
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
// 1.���� �ʱ�ȭ
//------------------------------------------------------------------------
typedef struct rsp_frame_init
{
    unsigned char init_ack;     // ���� �ʱ�ȭ ����
    unsigned char wavelen_sel;  // ���弱�� ����
    unsigned char launch_mode;  // �ܹ�/���� ����
    unsigned char period;       // �߻� �ֱ�
    unsigned long launch_cnt;   // �߻� Ƚ��(5-MSB, 8-LSB)
    unsigned char sw_ver;       // SW ����
    unsigned char module_num;   // ��������� ���� ȣ��
} rsp_frame_init_type;

//------------------------------------------------------------------------
// 2.�ֱ��� ��������
//------------------------------------------------------------------------
typedef struct rsp_frame_sts
{
    unsigned char sts_ack;      // �������� ����(0xF2)
    unsigned char cbit;         // CBIT ���
    unsigned char temp;         // LDA �µ�
    unsigned char sts_1;        // ���� ���� 1
    unsigned char sts_2;        // ���� ���� 2
} rsp_frame_sts_type;

//------------------------------------------------------------------------
// 3.��������� ��ü���� ���
//------------------------------------------------------------------------
typedef struct rsp_frame_insp
{
    unsigned char insp_ack;      // ��ü���� ��û(0xF2)
    unsigned char power_sts;     // �������� ����
    unsigned char temp_board;    // �µ� �� ���� ���� ����
    unsigned int power_56v;      // 56V �������� (4-MSB, 5-LSB)
    unsigned int power_12v;      // 12V �������� (6-MSB, 7-LSB
    unsigned int power_5v;       // 5V ��������  (8-MSB, 9-LSB)
    unsigned char lda_temp;       // LDA �µ�
    unsigned char board_temp;    // ���� �µ�
    unsigned long launch_cnt;    // �߻� Ƚ�� (12-MSB, 15-LSB)
} rsp_frame_insp_type;

//------------------------------------------------------------------------
// 4.��������� ������� ����
//------------------------------------------------------------------------
typedef struct rsp_frame_cmd
{
    unsigned char operate;        // ��� ���� ���
    unsigned char launch;         // �߻� ���� ���
    unsigned char launch_period;  // �߻� �ֱ� ����
} rsp_frame_cmd_type;

//------------------------------------------------------------------------
// 5.��������� �߻�����
//------------------------------------------------------------------------
typedef struct rsp_frame_launch
{
    unsigned char launch_ack;     // �߻� ���� ����(0x01)
    unsigned int q_light;         // ����(2-MSB, 3-LSB)
    unsigned char cbit;           // CBIT ���
} rsp_frame_launch_type;

//------------------------------------------------------------------------

extern BOOL UART_INTERRUPT;
extern BOOL UART_INTERRUPT_C;
extern BOOL Command_Start;		// ���˿��� ����.
extern BOOL sof_flag;
extern char Debug_cmd_flag;
extern char Debug_cmd_flag_c;
extern char rxd;
extern char rxd_c;
