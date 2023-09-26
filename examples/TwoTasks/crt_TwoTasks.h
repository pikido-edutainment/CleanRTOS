// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

namespace crt
{
	class SayHelloTask : public Task
	{
	private:
		const char* text = NULL;

	public:
		SayHelloTask(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, const char* text) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),text(text)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				ESP_LOGI(Task::taskName,"%s",text);
				vTaskDelay(1000);							// Wait 1000ms.
			}
		}
	}; // end class TestTimers
};// end namespace crt
