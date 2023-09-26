// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file demonstrates the use of a periodic timer and a fast one-shot timer.

namespace crt
{
	class TestTimers : public Task
	{
	private:
        Timer periodicTimer;
        Timer sleepTimer;

	public:
		TestTimers(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), periodicTimer(this), sleepTimer(this)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

            uint64_t timeMicros = 0;
            uint64_t timeMicrosNew = 0;
			uint64_t timeMicros2 = 0;
            uint64_t timeMicrosNew2 = 0;

            periodicTimer.start_periodic(1500000);          // This starts a periodic timer with a period of 1500000 microseconds, which is 1.5 second.
			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				ESP_LOGI("TestTimer", "Start waiting for periodicTimer");
				wait(periodicTimer);
				timeMicrosNew = esp_timer_get_time();
				ESP_LOGI("TestTimer", "Timer fired after microseconds:");
				ESP_LOGI("TestTimer","%d",int32_t(timeMicrosNew - timeMicros));
				timeMicros = timeMicrosNew; 				// prepare for next measurement
				
				ESP_LOGI("TestTimer", "Starting short sleep");
				timeMicros2 = esp_timer_get_time();         // sleepTimer.sleep_us(100) is equal to sleepTimer.start(100);wait(sleepTimer);
                sleepTimer.sleep_us(100);					// Test the one-shot timer with a sleep of 100 microseconds.
                timeMicrosNew2 = esp_timer_get_time();      // Note: it is recommended to use vTaskDelay for all sleeps greater than 1ms (that saves on hardware timer resources).
                ESP_LOGI("TestTimer", "Sleep time was microseconds:");
                ESP_LOGI("TestTimer","%d",int32_t(timeMicrosNew2 - timeMicros2));
			}
		}
	}; // end class TestTimers
};// end namespace crt
