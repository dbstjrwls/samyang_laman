
typedef	enum {
/*  0 */  HELIUM_STATE_IDLE,
/*  1 */  HELIUM_STATE_STS1,
/*  2 */  HELIUM_STATE_STS2,
/*  3 */  HELIUM_STATE_STS3,
/*  4 */  HELIUM_STATE_STS4,
/*  5 */  HELIUM_STATE_END
}
HeliumSeqType;

typedef	enum {
/*  0 */  PREMIX_STATE_IDLE,
/*  1 */  PREMIX_STATE_STS1,
/*  2 */  PREMIX_STATE_STS2,
/*  3 */  PREMIX_STATE_STS3,
/*  4 */  PREMIX_STATE_STS4,
/*  5 */  PREMIX_STATE_END
}
PremixSeqType;

typedef	enum {
/*  0 */  REFILL_STATE_IDLE,
/*  1 */  REFILL_STATE_STS1,
/*  2 */  REFILL_STATE_STS2,
/*  3 */  REFILL_STATE_STS3,
/*  4 */  REFILL_STATE_STS4,
/*  5 */  REFILL_STATE_STS5,
/*  6 */  REFILL_STATE_STS6,
/*  7 */  REFILL_STATE_STS7,
/*  8 */  REFILL_STATE_STS8,
/*  9 */  REFILL_STATE_STS9,
/* 10 */  REFILL_STATE_STS10,
/* 11 */  REFILL_STATE_STS11,
/* 12 */  REFILL_STATE_STS12,
/* 13 */  REFILL_STATE_STS13,
/* 14 */  REFILL_STATE_STS14,
/* 15 */  REFILL_STATE_STS15,
/* 16 */  REFILL_STATE_STS16,
/* 17 */  REFILL_STATE_END
}
RefillSeqType;

extern void System_Ctrl_Init();
extern void Fault_Flag_Init();
extern void Sync_Check(void);
extern void Trigger_Gen(void);
extern void Trigger_Gen_Repeat(BOOL gen);
extern float Moving_Average(float input);
extern void Peak_Data_Conv(void);
extern void EX5_Laser_Init(void);
extern void EX5_HighVoltage_Init(void);
extern void EX5_HighVoltage_Set(unsigned int num);
extern void Beam_Shutter_Init(void);
extern void Beam_Shutter_On(void);
extern void Beam_Shutter_Off(void);
extern void Laser_CCD_Trig_Init(void);
extern void Shutter_Fault_Check();
extern void EX5_LASER_STATUS(void);
extern void Monitoring(void);
extern BOOL Helium_Vent_Seq();
extern BOOL Premix_Vent_Seq();
extern BOOL Refill_Seq();

extern BOOL laser_trigger_flag;
extern BOOL IsPeakRcvComplete;
extern BOOL beam_shutter_end;
extern BOOL trig_gen_cmd;
extern BOOL shutter_status;
extern BOOL Laser_Pwr_Complete;
extern BOOL sync_trig;
extern BOOL sync_iccd;
extern BOOL sync_emccd;

extern char c_flag;
extern char c_flag_old;
extern char inter_lock;
extern char EX5_Laser_Fault;

//extern unsigned int dt_freq;
//extern unsigned int laser_trigger_count;
extern unsigned int rev_emeter_cnt;
extern unsigned int rev_sync_cnt;
extern unsigned int peak_trigger_cnt;
extern unsigned int sync_trigger_cnt;
//extern unsigned int Laser_Fault_Cnt;
extern unsigned int sec_cnt;

extern unsigned int DETECT_PERIOD;

extern LONG trigger_index;
extern unsigned char trigger_state;

extern LONG energy_index;
extern float energy_meter;
extern float hv_peak;

