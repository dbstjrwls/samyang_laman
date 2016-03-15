
typedef struct {
	unsigned long wTick;	// inner ticks, overflow limit = timer_swt
	unsigned long wCount;	// outer ticks, timer_sw unit
}
Timer;

extern void timerIsr(void);
extern void timerClr(Timer *pTimer);
extern BOOL timerCheck_mSec(Timer *pTimer, unsigned long wLimit);
extern BOOL timerCheck_Sec(Timer *pTimer, unsigned long wLimit);

