#include "common.h"
#include "config.h"

void Host_Memmgr_Init()
{
    GC_INIT()
    GC_enable_incremental();
    GC_disable();
}
