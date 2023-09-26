// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file demonstrates the use of a single task.

namespace crt
{
	class HelloWorld : public Task
	{
	public:
		HelloWorld(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				ESP_LOGI("HelloWorld", "Hello world!");
				vTaskDelay(1000);							// Wait 1000ms.
			}
		}
	}; // end class TestTimers
};// end namespace crt
