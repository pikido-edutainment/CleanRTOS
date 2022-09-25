// by Marius Versteegen, 2022

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
	// As this blocks everything else (at least on the same core - it's not clear
	// whether the other core continues), try to make it last as short as possible.
	extern portMUX_TYPE criticalSectionMutex; // Implement this singleton in main.cpp

	class TaskCriticalSection
	{
	public:
		TaskCriticalSection() { taskENTER_CRITICAL(&criticalSectionMutex); };
		~TaskCriticalSection() { taskEXIT_CRITICAL(&criticalSectionMutex); };
	};
};
