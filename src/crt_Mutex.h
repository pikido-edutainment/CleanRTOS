#pragma once
#include "internals/crt_FreeRTOS.h"

namespace crt
{
	// A mutex could be created for each resource that is shared by multiple threads.
	// The mutex can be used to avoid concurrent usage. Instead of locking the Mutex
	// directly, it is better to do it via MutexSection instead
	//	(which uses Mutex internally). That helps to avoid deadlocks due to misaligned
	//	order of resource lockings.


	//	Note: Most of the times, it is neater instead to make sure that each resource
	//	is only directly accessed by a single thread (let's call that thread the 
	//	"resource keeper"). Multiple threads could then queue resource - interaction
	//	commands to the "resource keeper".Use of Mutex(-Sections) can be omitted, then.

	// (see the MutexSection example in the examples folder)

	// Each Task has its own mutex stack. That makes sure that within the Task,
	// mutexes are always locked in the order of their mutex priority.
	
	class Mutex
	{
	public:
		uint32_t mutexID;
		SemaphoreHandle_t freeRtosMutex;
		BaseType_t rc;
		
	public:
		// MutexSections with lower mutexID can wrap MutexSections with higher mutexID.
		// but not the other way around (to prevent deadlocks).
		Mutex(uint32_t mutexID) :
			mutexID(mutexID), freeRtosMutex(xSemaphoreCreateMutex())
			//mutexID(mutexID), freeRtosMutex(xSemaphoreCreateBinary())
		{
			assert(mutexID != 0);	// MutexID should not be 0. Zero is reserved (to indicate absence of mutexID)
			//xSemaphoreGive(freeRtosMutex);	// In case of a binary semaphore: that one should be given before it can be taken.
		}
		
		void lock(Task* pTask)
		{
			while (true)
			{
				assert(mutexID > pTask->mutexIdStack.top()); // Error : Potential Deadlock : Within each thread, never try to lock a mutex with lower mutex priority than a mutex that is locked(before it).

				// max tickDelay is the constant portMAX_DELAY
				rc = xSemaphoreTake(freeRtosMutex, 0.0001 /* tickDelay */);
				if (rc == pdPASS)
				{
					assert(pTask->mutexIdStack.push(mutexID));// Assert would mean that either the amount of nested concurrently locked mutexes for this task exceeds the constant MAX_MUTEXNESTING, or the lock and unlock of the mutex are not performed in the same task.
					break;
				}
				taskYIELD(); // Feed the wdt.
			}
		}
		
		void unlock(Task* pTask)
		{
			pTask->mutexIdStack.pop();
			rc = xSemaphoreGive(freeRtosMutex);
			assert(rc == pdPASS);
		}
	};
};
