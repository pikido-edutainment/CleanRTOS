// by Marius Versteegen, 2023

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_Waitable.h"
#include "crt_Task.h"
#include "crt_ILogger.h"
namespace crt
{
    // A Flag is a waitable.It is meant for inter task communications.
    // The task that owns the flag should wait till another task sets the flag.
    // (see the Flag example in the examples folder)

    class Flag : public Waitable
    {
    private:
        Task* pTask;

    public:
        Flag(Task* pTask) : Waitable(WaitableType::wt_Flag), pTask(pTask)
        {
            Waitable::init(pTask->queryBitNumber(this));
        }
        
        void set()
        {
            pTask->setEventBits(Waitable::getBitMask());
        }

		void clear()
		{
			pTask->clearEventBits(Waitable::getBitMask());
		}
    };
};

