// by Marius Versteegen, 2022

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_MutexSection.h>

namespace crt
{
	// Rule for the struct below: both numbers should stay equal.
	// If not, the data is compromised.
	
	// For instance imagine a hotelroom, the first number is the room number,
	// and the second numer is the key number, which should match the room number.
	
	struct TwoNumbers
	{
		TwoNumbers():number1(0),number2(0){}
		
		int32_t number1;
		int32_t number2;
	};
	
	// Shared resources.
	extern Pool<TwoNumbers> poolTwoNumbersA;
	extern TwoNumbers       twoNumbersB;

	class SharedNumberIncreaser : public Task
	{
	public:
		static void StaticMain(void *pParam)
		{
			SharedNumberIncreaser* THIS = (SharedNumberIncreaser*) pParam;
			THIS->main();
		}

	public:
		SharedNumberIncreaser(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
     		start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.
			
			uint64_t count = 0;

			TwoNumbers twoNumbers;							// temp variable to copy to and from the pool.

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				// Create a new clause to allow MutexSection to be used in a RAII fashion.
				for(int i=0; i<200; i++)
				{
					// increase all numbers by 2.
					// The numbers in the pool protected TwoNumber object are expected to remain in sync.
				    poolTwoNumbersA.read(twoNumbers);
					twoNumbers.number1 += 2;
					twoNumbers.number2 += 2;
					poolTwoNumbersA.write(twoNumbers);
					
					// The numbers in the unprotected TwoNumber object may get out of sync because of 
					// concurrent access.
					twoNumbersB.number1 += 2;
					twoNumbersB.number2 += 2;
				}
				count+=1;
				if((count%100)==0)
				{
					ESP_LOGI(Task::taskName, "Current numbers, the first two should remain synced:");
				    poolTwoNumbersA.read(twoNumbers);
					ESP_LOGI("TwoNumbersA","%d",twoNumbers.number1);
					ESP_LOGI("TwoNumbersA","%d",twoNumbers.number2);
					
					ESP_LOGI("TwoNumbersB","%d",twoNumbersB.number1);
					ESP_LOGI("TwoNumbersB","%d",twoNumbersB.number2);
				}
				vTaskDelay(10);  // wait long enough - giving each task a chance - to avoid the chance 
				                 // of the watchdog timer kicking in for one of them.
			}
		}
	}; // end class SharedNumberIncreaser
};// end namespace crt
