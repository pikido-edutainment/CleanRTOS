#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_ILogger.h"
#include "crt_Task.h"
#include "crt_IHandler.h"
#include "crt_IHandlerListener.h"

// by Marius Versteegen, 2022

// A Handler object can be used to replace multiple periodic tasks by a single task,
// thereby saving task-switching overhead and resources.
 
// For that end, the handler task periodically calls update() member functions of the 
// objects that originally had their own task.

// (see the Handler example in the examples folder)

// NOTE: a Handler task must be started manually after its listeners have been added,
// by calling its start() member function.

// This Handler class assumes that 1 tick equals 1ms.
// Within the ArduinoIDE, that's the default already.
// The ESP_IDF however, uses 10ms per tick by default 
//    so if you use that, update in sdkconfig: CONFIG_FREERTOS_HZ=1000

// The define below can be uncommented temporarily to analyse the timings of the handler.
// #define TEST_CRT_HANDLER

namespace crt
{
	extern crt::ILogger& logger;

	template<unsigned int MAXLISTENERCOUNT> class Handler : public Task, public IHandler
	{
	private: 
		const uint64_t infiniteBatchSizeUs = 1000000000000; // 1e6 s means: infinite: no limitation in batchsize.

		// that converts LOGSIZE to stackSize in the initializer list of the constructor.
		IHandlerListener* arHandlerListener[MAXLISTENERCOUNT] = {};
		uint16_t nofHandlerListeners;
		uint16_t periodMs;
		uint64_t periodUs;
		uint64_t batchSizeUs;

	public:
		static void StaticMain(void *pParam)
		{
			Handler<MAXLISTENERCOUNT>* THIS = (Handler<MAXLISTENERCOUNT>*) pParam;
			THIS->main();
		}

	public:
		Handler(const char* taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint16_t periodMs) :
			Handler(taskName, taskPriority, taskCoreNumber, periodMs, infiniteBatchSizeUs /* by default, no batchsize limit */)
		{
		}

		// batchSizeUs : if a series of consequtive update() calls exceeds batchSizeUs,
		//               a task yield is inserted.
		Handler(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint16_t periodMs, uint64_t batchSizeUs) :
			Task(taskName, taskPriority, 3500 + MAXLISTENERCOUNT * sizeof(IHandlerListener*), taskCoreNumber), nofHandlerListeners(0), periodMs(periodMs), periodUs(periodMs*1000), batchSizeUs(batchSizeUs)// assert period.
		{
			for (int i = 0;i < MAXLISTENERCOUNT;i++)
			{
				arHandlerListener[i] = nullptr;
			}
			
			// start()   commented out: So the Handler Task does not start
			//           automatically. Instead, explicitly call the start function.
		}

		// The function start() starts this thread "manually".
		// It should be called after all handlerListeners have been added.
		// That way, use of (time consuming) mutexes can be avoided.
		/*override keyword not supported in current compiler*/
		void start()
		{
			Task::start();
		}

		/*override keyword not supported in current compiler*/
		void addHandlerListener(IHandlerListener* pHandlerListener)
		{
			if (!isAlreadyPresent(pHandlerListener))
			{	
				assert(nofHandlerListeners < MAXLISTENERCOUNT);
				arHandlerListener[nofHandlerListeners++] = pHandlerListener;
			}
		}

	private:
		bool isAlreadyPresent(IHandlerListener* pHandlerListener)
		{
			for (int i = 0; i < nofHandlerListeners; i++)
			{
				if (arHandlerListener[i] == pHandlerListener)
				{
					return true;
				}
			}
			return false;
		}

		void main()
		{
#ifdef TEST_CRT_HANDLER
			uint64_t before2		= esp_timer_get_time();	// For measuring of actual complete period.
			uint64_t after2			= 0;
			uint64_t diff2			= 0;
			int32_t  nofYieldsDone  = 0;
			uint64_t yieldTimeSpent = 0;
			uint64_t beforeBatches  = 0;
			uint64_t spentOnBatches = 0;
#endif
			uint64_t beforeAll		= esp_timer_get_time();
			uint64_t afterAll		= 0;	// gross Time spent by Handler, including yields to other threads.
			uint32_t diffAll		= 0;

			uint64_t beforeBatch    = 0;
			uint64_t batchTimeSpent	= 0;
			
			vTaskDelay(10); // wait for others to initialise
			while (true)
			{
				beforeBatch = esp_timer_get_time();

#ifdef TEST_CRT_HANDLER
				yieldTimeSpent = 0;
				nofYieldsDone  = 0;
				beforeBatches  = beforeBatch;
#endif				

				for (int i = 0; i < nofHandlerListeners; i++)
				{
					arHandlerListener[i]->update();

					if (batchSizeUs != infiniteBatchSizeUs)
					{
						uint64_t nowUs = esp_timer_get_time();
						batchTimeSpent = (nowUs - beforeBatch);

						if (batchTimeSpent > batchSizeUs)
						{
							taskYIELD();

#ifdef TEST_CRT_HANDLER
							yieldTimeSpent += (esp_timer_get_time() - nowUs);
							nofYieldsDone++;
#endif
							beforeBatch = esp_timer_get_time();
						}
					}
				}

				afterAll = esp_timer_get_time();     // time in micro seconds.
				diffAll = (afterAll - beforeAll);    // difference in milliseconds
				                                     // This is the time spent on batches,
													 // yields and optionally the test code
				                                     // within the #defines below.
				assert(diffAll < periodUs);
				if (diffAll < periodUs) {
					vTaskDelay((periodUs - diffAll) / 1000);
				}
				beforeAll = esp_timer_get_time();	 // This is the clean cut where the old interval ends

#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING
				auto temp = uxTaskGetStackHighWaterMark(nullptr);
				if (!prev_stack_hwm || temp < prev_stack_hwm)
				{
					prev_stack_hwm = temp;
					ESP_LOGI("hwm_check", "Task %s has left: %d stack bytes and %d heap bytes", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));
					//ESP_LOGI(taskName,"%d",unsigned(xPortGetFreeHeapSize()));vTaskDelay(1);
					//ESP_LOGI(taskName," heap bytes","");vTaskDelay(1);
				}
#endif

#ifdef TEST_CRT_HANDLER
				after2 = esp_timer_get_time();
				diff2 = (uint32_t)(after2 - before2);
				before2 = after2;

				spentOnBatches = afterAll - beforeBatches;

				logger.logText("-----------------------------------------------------");
				logger.logText(Task::taskName);
				logger.logText("Diagnostics:");
				logger.logText("nofYieldsDone:");
				logger.logUint32(nofYieldsDone);
				logger.logText("gross time spent on batches in microsecs, including yields:");
				logger.logUint32((int32_t)spentOnBatches);
				logger.logText("nett time spent on batches in microsecs:");
				logger.logUint32((int32_t)(spentOnBatches - yieldTimeSpent));
				logger.logText("handler interval microsecs");
				logger.logUint32((int32_t)diff2);
#endif
			}
		}
	}; // end class crt_Handler
}; // end namespace CleanRTOS
