// by Marius Versteegen, 2022

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_MutexSection.h>

namespace crt
{
	// Shared resources.
	extern int32_t sharedIntA;
	extern int32_t sharedIntB;
	extern int32_t sharedIntC;

	// The mutexes that guard them.
	extern Mutex mutexSharedIntA;		// Mutex with id 1, which protects sharedIntA
	extern Mutex mutexSharedIntB;		// Mutex with id 2, which protects sharedIntB
	extern Mutex mutexSharedIntC;		// Mutex with id 3, ,, . 
	                                        // NOTE: it is forbidden to lock a new mutex with an id
					         				// that is lower than the highest id of currently locked mutexes.
							        		// By using MutexSections, that rule is safeguarded.

	class SharedNumberIncreaser : public Task
	{
	private:
		int32_t& sharedInt;
		Mutex&   mutexSharedInt;
		
	public:
		static void StaticMain(void *pParam)
		{
			SharedNumberIncreaser* THIS = (SharedNumberIncreaser*) pParam;
			THIS->main();
		}

	public:
		SharedNumberIncreaser(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, 
		                       int32_t& sharedInt, Mutex& mutexSharedInt) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),sharedInt(sharedInt),mutexSharedInt(mutexSharedInt)
		{
     		start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.
			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				// Create a new clause to allow MutexSection to be used in a RAII fashion.
				{
					MutexSection ms(this,mutexSharedInt);
					sharedInt++;
				}
				vTaskDelay(1);
			}
		}
	}; // end class SharedNumberIncreaser

	class SharedNumbersDisplayer : public Task
	{
	public:
		static void StaticMain(void *pParam)
		{
			SharedNumbersDisplayer* THIS = (SharedNumbersDisplayer*)pParam;
			THIS->main();
		}

	public:
		SharedNumbersDisplayer(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased();
				
				ESP_LOGI("SharedNumbersDisplayer", "Sleeping 1000ms, during which other tasks can update the numbers");
				vTaskDelay(1000);
				{
					ESP_LOGI("SharedNumbersDisplayer", "Waiting to obtain the mutexes");
				    MutexSection msA(this,mutexSharedIntA);
					MutexSection msB(this,mutexSharedIntB);
					MutexSection msC(this,mutexSharedIntC);
					
					ESP_LOGI("SharedNumbersDisplayer", "Mutexes obtained. no one else can alter the ints now");
					ESP_LOGI("sharedIntA","%d",sharedIntA);
					ESP_LOGI("sharedIntB","%d",sharedIntB);
					ESP_LOGI("sharedIntC","%d",sharedIntC);
					ESP_LOGI("SharedNumbersDisplayer", "Sleeping 1000ms, but in spite of that no other task can change the ints");
					vTaskDelay(1000);
					ESP_LOGI("SharedNumbersDisplayer", "Next three values should thus be the same as the previous three:");
					ESP_LOGI("sharedIntA","%d",sharedIntA);
					ESP_LOGI("sharedIntB","%d",sharedIntB);
					ESP_LOGI("sharedIntC","%d",sharedIntC);
					
					ESP_LOGI("SharedNumbersDisplayer", "Releasing the mutexes"); // The destructors of MutexSection release the mutexes.
				}
			}
		}
	}; // end class SharedNumbersDisplayer
};// end namespace crt
