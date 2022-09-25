// by Marius Versteegen, 2022

#pragma once
#include <crt_CleanRTOS.h>

// This file contains the code of a task that sends numbers to another task, which displays them.
// The numbers are transferred via the Queue synchronization mechanism.

// To see the test output in the serial monitor, just press the button that is assigned
// to the logger.

namespace crt
{
	extern crt::ILogger& logger;
  
	class NumberDisplayTask : public Task
	{
	private:
		Queue<int32_t, 10> queueNumbers;

	public:
		static void StaticMain(void *pParam)
		{
			NumberDisplayTask* THIS = (NumberDisplayTask*) pParam;
			THIS->main();
		}

	public:
		NumberDisplayTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), queueNumbers(this)
		{
			start();
		}

    // The function below is called by an other thread, thus passing on the numbers that is wants to get displayed.
	void displayNumber(int32_t number)
	{
        bool bResult = queueNumbers.write(number);
        if (!bResult)
        {
			ESP_LOGI("NumberDisplayTask", "OOPS! queueNumber was already full!");vTaskDelay(1);
        }
	}

	private:
		/*override keyword not supported*/
		void main()
		{
			int32_t number=0;
			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				wait(queueNumbers);
                queueNumbers.read(number);
				ESP_LOGI("NumberDisplayTask","%d",number);
			}
		}
	}; // end class NumberDisplayTask

	class NumberSendTask : public Task
	{
	private:
		NumberDisplayTask& numberDisplayTask;

	public:
		static void StaticMain(void *pParam)
		{
			NumberSendTask* THIS = (NumberSendTask*)pParam;
			THIS->main();
		}

	public:
		NumberSendTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, NumberDisplayTask& numberDisplayTask) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), numberDisplayTask(numberDisplayTask)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			int32_t i = 1;

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased();
				
				// Send a burst of 5 numbers to the NumerDisplayTask.
				for(int n=0;n<5;n++)
				{
					numberDisplayTask.displayNumber(i++);
				}
				
				vTaskDelay(1000);
			}
		}
	}; // end class NumberSendTask
};// end namespace crt
