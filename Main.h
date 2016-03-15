#ifndef _MAIN_H
#define _MAIN_H

extern float Alpha_H;
extern float Alpha_L;
extern double Alpha_D;
extern float Betha_H;
extern float Betha_L;
extern double Betha_D;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_period (0x101), Periodic_Sequence_number 명령

extern unsigned int cmd_period_seq_num;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_dt_cmd (0x111), 제어 명령

extern BOOL cmd_init_start;
extern BOOL cmd_beam_shutter;
extern BOOL cmd_operate;
extern BOOL cmd_diag;
extern BOOL cmd_stop;
extern BOOL cmd_req_state;
extern BOOL cmd_req_version;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_trigger (0x121), Excimer Laser Trigger

extern unsigned long cmd_trigger_index;
extern unsigned int cmd_trigger_state;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_energy (0x122), Excimer Laser Energy

extern unsigned long cmd_energy_index;
extern unsigned int cmd_energy_meter;
extern unsigned int cmd_energy_hv_peak;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_set (0x123), Excimer Laser Set

extern unsigned int cmd_laser_set_action;
extern unsigned int cmd_laser_set_hv;
extern unsigned int cmd_laser_set_period;
extern unsigned int cmd_laser_set_repeat;

//-----------------------------------------------------------------------------------
// Spectro -> Detect
// sp_laser_config (0x124), Excimer Laser Config

extern LASER_CTRL_LINE laser_ctrl_line;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_period (0x201), Periodic_Sequence_number 응답

extern unsigned int rsp_period_seq_num;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_version (0x202), 버전

extern unsigned int d_version_1;
extern unsigned int d_version_2;
extern unsigned int d_release_1;
extern unsigned int d_release_2;
extern unsigned int d_year;
extern unsigned int d_month;
extern unsigned int d_date;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_cmd (0x211), 제어 명령 응답

extern BOOL rsp_init_start;
extern BOOL rsp_beam_shutter;
extern BOOL rsp_operate;
extern BOOL rsp_diag;
extern BOOL rsp_stop;
extern BOOL rsp_req_state;
extern BOOL rsp_req_version;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_trigger (0x221)

extern unsigned long rsp_trigger_index;
extern unsigned int rsp_trigger_state;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_energy (0x222)

extern unsigned long rsp_energy_index;
extern Uint32 rsp_energy_meter;
extern unsigned int rsp_energy_hv_peak;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_laser_set (0x223)

extern unsigned int rsp_laser_set_action;
extern unsigned int rsp_laser_set_hv;
extern unsigned int rsp_laser_set_period;
extern unsigned int rsp_laser_set_repeat;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_laser_config (0x224)

extern LASER_CTRL_LINE rsp_laser_ctrl_line;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_status_1 (0x241)

extern unsigned int rsp_standby_time;
extern unsigned int rsp_gas_pressure;
extern float rsp_ls_temp;
extern float rsp_filament_m;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_status_2 (0x242)

extern unsigned int ctrl_bd_temp;

//-----------------------------------------------------------------------------------
// Detect -> Spectro
// dt_fault (0x281)

extern DETECT_ERROR_TYPE rsp_dt_fault;

//extern unsigned char detect_state;
extern unsigned char Detect_State_Device;
extern unsigned char Detect_Error_Device;
extern unsigned char Detect_Error_State;
extern unsigned char Detect_Error_Code;

extern unsigned long total_detect_cnt;
extern BOOL i2c_error;


#endif // _MAIN_H
