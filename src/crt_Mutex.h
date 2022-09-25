#pragma once
#include "crt_FreeRTOS.h"
#include "internals/crt__std_stack.h"

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

	// Next global variable is accessed from the function bodies of all Mutex objects.
	std::Stack<uint32_t, crt::MAX_MUTEXNESTING> mutexIdStack(0);
	
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
				taskYIELD ();
			}

			assert(mutexID >= mutexIdStack.top());	// If this assert fails, there's a potential deadlock due to wrong nesting order.
			mutexIdStack.push(mutexID);
		}
		
		void unlock()
		{
			assert(mutexIdStack.pop() == mutexID); // If this assert fails, there is no hierarchical nesting.
                                                  
			rc = xSemaphoreGive(freeRtosMutex);
			assert(rc == pdPASS);
		}
	};
};
