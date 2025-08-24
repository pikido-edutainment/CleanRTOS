// by Marius Versteegen, 2023

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "internals/crt__std_stack.h"
#include "crt_Config.h"
#include "crt_ILogger.h"
#include "crt_Waitable.h"

// After a class has inherited from Task, every instantiated object of that
// class will run in its own thread. Within the main function of a such object, 
// it is possible to wait for Waitables.
// (see the examples HelloWorld, TwoTasks and TenTasks in the examples folder)

namespace crt
{
    // extern ILogger& logger;

	class Task
	{
	protected:
		UBaseType_t prev_stack_hwm = 0;

	private:
		EventGroupHandle_t hEventGroup;
		uint32_t latestResult = 0;

	public:
		const char *taskName;
		unsigned int taskPriority;
		unsigned int taskStackSizeBytes;
		unsigned int taskCoreNumber;
		TaskHandle_t taskHandle;

		uint32_t nofWaitables;
        uint32_t queuesMask;        // Every bit in this mask belongs to a queue.
        uint32_t flagsMask;         // Every bit in this mask belongs to a flag.
        uint32_t timersMask;        // Every bit in this mask belongs to a timer.

        std::Stack<uint32_t, MAX_MUTEXNESTING> mutexIdStack;

	public:
        Task(const char *taskName, unsigned int taskPriority, unsigned int taskStackSizeBytes, unsigned int taskCoreNumber)
            : taskName(taskName), taskPriority(taskPriority), taskStackSizeBytes(taskStackSizeBytes), taskCoreNumber(taskCoreNumber),
            nofWaitables(0), queuesMask(0), flagsMask(0), timersMask(0), mutexIdStack(0)  // The value 0 is reserved for "empty stack".
		{
			hEventGroup = xEventGroupCreate();
			assert(hEventGroup != NULL); // If failed, not enough heap memory.
		}

        uint32_t queryBitNumber(Waitable* pWaitable)
        {
            assert(nofWaitables < 24);
            switch (pWaitable->getType())
            {
            case WaitableType::wt_Queue:
                queuesMask |= (1 << nofWaitables);
                break;
            case WaitableType::wt_Timer:
                timersMask |= (1 << nofWaitables);
                break;
            case WaitableType::wt_Flag:
                flagsMask |= (1 << nofWaitables);
                break;
            default:
                break;
            }
            return nofWaitables++;
        }

        EventGroupHandle_t* getEventGroup()
        {
            return &hEventGroup;
        }

        inline void setEventBits(const EventBits_t uxBitsToSet)
        {
            xEventGroupSetBits(hEventGroup,uxBitsToSet);
        }

        inline void clearEventBits(const EventBits_t uxBitsToClear)
        {
            xEventGroupClearBits(hEventGroup, uxBitsToClear);
        }

		// Next function starts the thread.
		// It should be called from setup() or the constructor of the class that inherits from Task.
		void start()
		{
			xTaskCreatePinnedToCore(
				staticMain				// The static Main function that will call the local main function.
				, taskName				// A name just for humans
				, taskStackSizeBytes	// This stack size can be checked & adjusted by reading the Stack Highwater
				, this					// Pointer to this to be able to call the local version of main.
				, taskPriority
				, &taskHandle
				, taskCoreNumber);
		}

		virtual void main() = 0;

	public:
		// Next construct allows the main thread of the task to be run in a non-static function.
		// That way, we can easily create multiple task objects from the same class.
		static void staticMain(void *pParam)
		{
			Task* pTask = (Task*)pParam;
			pTask->main();
		}

		// Next function gives an indication of whether the task (still) has enough stack and heap
		// memory available.
        inline void dumpStackHighWaterMarkIfIncreased()
        {
#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING	
            UBaseType_t temp = uxTaskGetStackHighWaterMark(NULL);
            if (!prev_stack_hwm || temp < prev_stack_hwm)
            {
                prev_stack_hwm = temp;

                ESP_LOGI("hwm_check", "Task %s has left: %d stack bytes and %d heap bytes", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));

                // Serial.print("So far, this Task:");
                // if (taskName != NULL)
                // {
                    // Serial.print(taskName);
                // }

                // Serial.print(", has NOT used yet:");
                // Serial.print(prev_stack_hwm);
                // Serial.print(" stack bytes and ");
                // Serial.print(unsigned(xPortGetFreeHeapSize()));
                // Serial.println(" heap bytes");

                // vTaskDelay(10);	// Allow Serial prints to complete before allowing more of them.
            }
#endif
        }

        // Wait for a single waitable.
        inline void wait(Waitable& waitable)
        {
            waitAll(waitable.getBitMask());
        }

        // Waitll waits till ALL the specified waitables have fired.
        // It automatically clears the event-bits, apart from the queue bits.
        // So there's no need to check with hasFired.
		inline void waitAll(uint32_t bitsToWaitFor)
		{
			latestResult = xEventGroupWaitBits(
				hEventGroup,
				bitsToWaitFor,
				pdTRUE, // xClearOnExit, We'd like to set it to false, but that would create a race condition, right after this function returns, another thread could set another flag. .  Waiting for all, all bits can be cleared!.. except for the queue bits - they can only become cleared after reading from the queue has emptied it..
				pdTRUE, // xWaitForAllBits
				portMAX_DELAY); // xTicksToWait)

            // Actually, we didn't want to clear the queue bits, so let's repair that:
            setEventBits(queuesMask & latestResult);
		}

		// return value: the bits that were set at the time of firing.
        // Use hasFired() to determine which one.
        // It is possible that multiple event bits were set at the same time.
        // For a clean design, stop after finding the first one using hasFired.
        //
        // Thus, it is advised always to process only the actions on a single event after a waitAny.
		inline void waitAny(uint32_t bitsToWaitFor)
		{
			latestResult = xEventGroupWaitBits(
				hEventGroup,
				bitsToWaitFor,
				pdFALSE, // xClearOnExit,  Waiting for any: individual "hasFired" checks are needed. They will clear/consume the corresponding event.
				pdFALSE, // xWaitForAllBits,
				portMAX_DELAY); // xTicksToWait)
		}

		inline bool hasFired(Waitable& waitable)
		{
            uint32_t bitmask = waitable.getBitMask();
			bool result = ((latestResult & bitmask) != 0);
            if (result)
            {
                // "Manually" Clear, except if a queue is involved for a queue, only a read() may consume the event that signals that there's someting in the queue.
                clearEventBits((~queuesMask) & bitmask);
            }
            return result;
		}
	};
};
