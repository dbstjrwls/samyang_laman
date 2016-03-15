#ifndef _GLOBAL_H
#define _GLOBAL_H

// git test
typedef enum
{
    ST_NONE_STATE = 0,
    ST_POWER_ON = 0x01,
    ST_START_ERROR = 0x0E,

    ST_INIT_RUN = 0x10,
    ST_INIT_COMPLETE = 0x11,
    ST_INIT_ERROR = 0x1E,

    ST_READY_RUN = 0x20,
    ST_READY_COMPLETE = 0x21,
    ST_READY_ERROR = 0x2E,

    ST_OPERATE_RUN = 0x30,
    ST_OPERATE_COMPLETE = 0x31,
    ST_OPERATE_ERROR = 0x3E,

    ST_CHECK_RUN = 0x40,
    ST_CHECK_COMPLETE = 0x41,
    ST_CHECK_ERROR = 0x4E,

    ST_STOP_RUN = 0xF0,
    ST_STOP_COMPLETE = 0xF1,
    ST_STOP_ERROR = 0xFE
} E_DETECT_STATE;

E_DETECT_STATE GetDetectState(void);
void SetDetectState(E_DETECT_STATE State);



#endif //_GOBAL_H
