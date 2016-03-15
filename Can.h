#ifndef _CAN_H
#define _CAN_H
extern void Can_Process_RX(void);
extern void Can_Process_TX(void);
extern void Can_PROCESSING(void);
extern void init_can(void);
extern void SetRcvdLaserSetCmd(void);

extern BOOL CAN_TX_INT_FLAG;
extern BOOL CAN_TX_READY_FLAG;
extern BOOL CAN_CH_FLAG;
extern BOOL CAN_CH;

extern BOOL CAN_rx_flag;
extern LONG CAN_rx_id;
extern BYTE CAN_rx_length;
extern LONG CAN_rx_low_data;
extern LONG CAN_rx_high_data;
//extern WORD CAN_PERIOD_CHECK;

extern char Period_Count;
extern unsigned int can_energy_cnt;
extern unsigned char before_detect_state;
extern Uint32 CAN_tx_low[10];
extern Uint32 CAN_tx_high[10];
#endif // _CAN_H
