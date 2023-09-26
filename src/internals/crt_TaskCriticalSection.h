// by Marius Versteegen, 2023

#pragma once

namespace crt
{
	// This class allows automatic interrupt resumption using RAII pattern.
	// Note: it can be used recursively, resumption is only achieved when 
	// the number of resumptions meets the number of suspensions.
	// Note 2: don't use interrupts within the critical section,
	// like waitables and non-busy-timers. 
	// You can still use busy-timers though, like waitMicroseconds(1) or NOP operations.
	//
	// As this blocks all interrupts on both cores, try to make it last as short as possible.
	// I believe typically, a critical section takes about a 1us to enter and exit.
	extern portMUX_TYPE criticalSectionMutex; // Implement this singleton in main.cpp

	class TaskCriticalSection
	{
	public:
		TaskCriticalSection() { taskENTER_CRITICAL(&criticalSectionMutex); };
		~TaskCriticalSection() { taskEXIT_CRITICAL(&criticalSectionMutex); };
	};
};
