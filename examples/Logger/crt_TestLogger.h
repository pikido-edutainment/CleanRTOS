// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// The code below compares the speed of direct logging with ESP_LOGI to 
// indirect logging via Logger.

namespace crt
{
	extern ILogger& logger;
  
	class TestLogger : public Task
	{
	public:
		TestLogger(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			int32_t  anInt  = 0;
			uint32_t aUint  = 0;
			float    aFloat = 0.0f;

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				ESP_LOGI("", "Immediate logging:");

				uint64_t beforeImmediateLogging = esp_timer_get_time();
				ESP_LOGI("", "This is some plain text");
				ESP_LOGI("This is an int32", "%d", anInt);
				ESP_LOGI("This is an uint32", "%d", aUint);
				ESP_LOGI("This is a float", "%f", aFloat);
				uint64_t afterImmediateLogging = esp_timer_get_time();

				ESP_LOGI("", "Postponed logging:");

				uint64_t beforePostPonedLogging = esp_timer_get_time();
				logger.logText("This is some plain text.");
				logger.logText("This is an int32:");
				logger.logInt32(anInt);
				logger.logText("This is a uint32:");
				logger.logUint32(aUint);
				logger.logText("This is a float:");
				logger.logFloat(aFloat);
				uint64_t afterPostponedLogging = esp_timer_get_time();

				logger.dumpNow();

				ESP_LOGI("Immediate logging spent microseconds", "%d", (int32_t)(afterImmediateLogging - beforeImmediateLogging));
				ESP_LOGI("Postponed logging spent microseconds", "%d", (int32_t)(afterPostponedLogging - beforePostPonedLogging));
				ESP_LOGI("", "So for time critical debugging, it's better to use the postponed logger.");

				ESP_LOGI("", "Apart from calling logger.dumpNow(), you can also initiate");
				ESP_LOGI("", "a log dump using a button that is attached to the pin that");
				ESP_LOGI("", "is passed to the constructor of the logger.");

				ESP_LOGI("____________", "_______________________________________________");

				anInt--;
				aUint++;
				aFloat += 0.3f;

				vTaskDelay(2000);  // wait 2s before next round of logs.
			}
		}
	}; // end class FlagListener
};// end namespace crt
