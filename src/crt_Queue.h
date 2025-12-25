// by Marius Versteegen, 2023

// A Queue is a waitable. It is meant for inter task communications.
// The task that owns the queue should wait for another task to put something into it.
// (see the Queue example in the examples folder)

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_Waitable.h"
#include "crt_Task.h"
#include "crt_ILogger.h"
namespace crt
{
    //extern ILogger& logger;

	template<typename TYPE, uint32_t COUNT> class Queue : public Waitable
	{
	private:
		QueueHandle_t qh;
        Task* pTask;
        TickType_t writeDelay;
		TYPE dummy;

	public:
		Queue(Task* pTask,bool bWriteWaitIfQueueFull=false):Waitable(WaitableType::wt_Queue),pTask(pTask),
            writeDelay(bWriteWaitIfQueueFull ? portMAX_DELAY : 0)
		{
            Waitable::init(pTask->queryBitNumber(this));
			qh = xQueueCreate(COUNT, sizeof(TYPE));
		}
		
		///*override*/ bool operator==(const Waitable& other) const { return this == &other; };

		void read(TYPE& returnVariable) 
		{
			BaseType_t rc = xQueueReceive(qh, &returnVariable, portMAX_DELAY);
            if (uxQueueMessagesWaiting(qh) > 0)
            {
                // The queue is not empty yet,
                // Make sure that the corresponding eventbit gets set again,
                // Such that a wait for the queue will fire.
                pTask->setEventBits(Waitable::getBitMask());
            }
            else
            {
                pTask->clearEventBits(Waitable::getBitMask());
            }
			assert(rc == pdPASS);
		}

		bool write(TYPE& variableToCopy)
		{
			BaseType_t rc = xQueueSend(qh, &variableToCopy, writeDelay);
            if (rc != pdPASS)
            {
                // The queue got full. Note: that cannot happen if bWriteWaitIfQueueFull==true.
                return false;
            }
            pTask->setEventBits(Waitable::getBitMask());
            return true;
		}

		int getNofMessagesWaiting()
		{
			return uxQueueMessagesWaiting(qh);
		}

		void clear()
		{
			while (uxQueueMessagesWaiting(qh) > 0)
			{
				xQueueReceive(qh, &dummy, portMAX_DELAY);
			}
			
			pTask->clearEventBits(Waitable::getBitMask());
		}
	};
};

