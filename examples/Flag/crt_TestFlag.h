// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

namespace crt
{
	extern ILogger& logger;
  
	class FlagListener : public Task
	{
	private:
        Flag flagHi;

	public:
		FlagListener(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), flagHi(this)
		{
            logger.logText("Init FlagListener");
			start();
		}

	// Simply Notifying another task should be done via a member function like the 
	// one below, which sets a Flag.
    void sayHi()
    {
        flagHi.set();
    }

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				ESP_LOGI("FlagListener", "Waiting for flagHi");vTaskDelay(1);
				wait(flagHi);								// Wait for flag to be set.
				ESP_LOGI("FlagListener", "flagHi was set");vTaskDelay(1);
				ESP_LOGI("FlagListener", "So another thread said Hi to this thread");vTaskDelay(1);
			}
		}
	}; // end class FlagListener

	class FlagSetter : public Task
	{
	private:
		FlagListener& flagListener;

	public:
		static void StaticMain(void *pParam)
		{
			FlagSetter* THIS = (FlagSetter*)pParam;
			THIS->main();
		}

	public:
		FlagSetter(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, FlagListener& flagListener) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), flagListener(flagListener)
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
				ESP_LOGI("FlagSetter  ", "Saying Hi to FlagListener");vTaskDelay(1);
				flagListener.sayHi();
				vTaskDelay(1000);
			}
		}
	}; // end class FlagSetter
};// end namespace crt
