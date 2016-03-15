#include "global.h"

E_DETECT_STATE eDetectState = ST_NONE_STATE;

E_DETECT_STATE GetDetectState(void)
{
    return eDetectState;
}

void SetDetectState(E_DETECT_STATE eState)
{
    eDetectState = eState;
}
