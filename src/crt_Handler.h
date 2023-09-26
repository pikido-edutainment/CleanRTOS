#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_ILogger.h"
#include "crt_Task.h"
#include "crt_IHandler.h"
#include "crt_IHandlerListener.h"

// by Marius Versteegen, 2023

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
	extern ILogger& logger;

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
		Handler(const char* taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint16_t periodMs) :
			Handler(taskName, taskPriority, taskCoreNumber, periodMs, infiniteBatchSizeUs /* by default, no batchsize limit */)
		{
		}

		// batchSizeUs : if a series of consequtive update() calls exceeds batchSizeUs,
		//               a task yield is inserted.
		Handler(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint16_t periodMs, uint64_t batchSizeUs) :
			Task(taskName, taskPriority, 5500 + MAXLISTENERCOUNT * sizeof(IHandlerListener*), taskCoreNumber), nofHandlerListeners(0), periodMs(periodMs), periodUs(periodMs*1000), batchSizeUs(batchSizeUs)// assert period.
		{
			start();
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
			
			vTaskDelay(500); // wait for other objects to initialise and add themselves as handlerlistener.
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
				if (diffAll >= periodUs)
				{
					ESP_LOGI("Handler Delay too long:", "%d us", diffAll);
				}

				if (diffAll < periodUs) {
					vTaskDelay((periodUs - diffAll) / 1000);
				}
				beforeAll = esp_timer_get_time();	 // This is the clean cut where the old interval ends

				dumpStackHighWaterMarkIfIncreased();

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
