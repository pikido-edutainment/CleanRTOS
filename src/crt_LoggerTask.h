#pragma once
#include "internals/crt_FreeRTOS.h"
namespace crt
{
    // The logger has a dedicated task type of its own.
    // That way, the logger can be used in the Task class that is used by all other tasks,
    // without creating a chicken-egg problem.
	class LoggerTask
	{
	public:
		LoggerTask(const char *taskName, unsigned int taskPriority, unsigned int taskStackSizeBytes, unsigned int taskCoreNumber) :
			taskName(taskName), taskPriority(taskPriority), taskStackSizeBytes(taskStackSizeBytes), taskCoreNumber(taskCoreNumber)
		{}

	public:
		const char *taskName;
		unsigned int taskPriority;
		unsigned int taskStackSizeBytes;
		unsigned int taskCoreNumber;
		TaskHandle_t taskHandle;
	};
};
