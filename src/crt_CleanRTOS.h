// by Marius Versteegen, 2023

// crt_CleanRTOS.h is a convenience include: it includes the most common parts 
// of CleanRTOS

#pragma once

// Next define can be used to make other libraries conditionally
// dependent on CleanRTOS.
#define CleanRTOS_INCLUDED

#include "internals/crt_FreeRTOS.h"
#include "crt_Config.h"
#include "crt_MainInits.h"
#include "crt_ILogger.h"
#include "crt_Task.h"
#include "crt_Flag.h"
#include "crt_Queue.h"
#include "crt_Timer.h"
#include "crt_Pool.h"
#include "crt_IHandler.h"
#include "crt_IHandlerListener.h"

// crt_Logger, crt_Handler, crt_MutexSection, crt_Mutex and crt_Handler are to
// be included separately, if needed.
//
// Reasons:
//    * Logger should never be needed outside main.cpp (or .ino)). 
//      ILogger should be used everywhere else instead.
//    * Handler should never be needed outside main.cpp (or .ino))
//      IHandler should be used everywhere else instead.
//    * Mutex should never be needed outside main.cpp (or .ino)),
//      to keep a good overview of the assignment and order of mutex ids.
//		Also, Mutex should not be used stand-alone, as that is error prone.
//      Use MutexSections in conjunction.
//    * Even MutexSections should not be needed often, with a clear design.
//      For example, try to use a Pool instead of a single MutexSection.