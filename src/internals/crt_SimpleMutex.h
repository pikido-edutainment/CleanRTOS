#pragma once
#include "internals/crt_FreeRTOS.h"

namespace crt
{
	// A SimpleMutex protects a resource from concurrent access by multiple threads.
	// It is a bit simpler to use and faster than the normal Mutex.
	// However, opposed to the normal Mutex, it does not detect deadlock situations.
	// Thus the SimpleMutex should only be used in situations that cannot potentially
	// yield deadlock situations, like inside a Pool.
	
	class SimpleMutex
	{
	public:
		SemaphoreHandle_t freeRtosMutex;
		BaseType_t rc;
		
	public:
		// MutexSections with lower mutexID can wrap MutexSections with higher mutexID.
		// but not the other way around (to prevent deadlocks).
		SimpleMutex() :
			freeRtosMutex(xSemaphoreCreateMutex())
			//mutexID(mutexID), freeRtosMutex(xSemaphoreCreateBinary())
		{
			//xSemaphoreGive(freeRtosMutex);	// In case of a binary semaphore: that one should be given before it can be taken.
		}
		
		void lock()
		{
			while (true)
			{
				// max tickDelay is the constant portMAX_DELAY
				rc = xSemaphoreTake(freeRtosMutex, 0.0001 /* tickDelay */);
				if (rc == pdPASS)
				{
					break;
				}
				taskYIELD(); // Feed the wdt.
			}
		}
		
		void unlock()
		{
			rc = xSemaphoreGive(freeRtosMutex);
			assert(rc == pdPASS);
		}
	};
};
