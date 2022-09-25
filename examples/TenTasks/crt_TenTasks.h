// by Marius Versteegen, 2022

#pragma once
#include <crt_CleanRTOS.h>

namespace crt
{
	class CounterTaskForTestTenTasks : public Task
	{
	private:
		int32_t count;

	public:
		static void StaticMain(void* pParam)
		{
			CounterTaskForTestTenTasks* THIS = (CounterTaskForTestTenTasks*)pParam;
			THIS->main();
		}

	public:
		CounterTaskForTestTenTasks(const char* taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), count(0)
		{
			start(); // For simplicity, the task is started right away in it's constructor.
		}

	private:
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				ESP_LOGI(Task::taskName, "%d", count);
				count++;
				vTaskDelay(1000);  // Wait 1000ms.
			}
		}
	}; // end class CounterForTestHandler
};// end namespace crt
