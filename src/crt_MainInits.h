// by Marius Versteegen, 2022

// MainInits initializes CleanRTOS.
// A MainInits object must be defined before the Tasks are started.

#pragma once
#include "internals/crt_FreeRTOS.h"
namespace crt
{
    class MainInits
    {
    public:
        MainInits()
        {
            esp_timer_init();   // Allow creation and use of microsecond timers.
        }
    };
};
