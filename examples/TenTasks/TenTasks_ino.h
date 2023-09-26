// by Marius Versteegen, 2023

// This file test 10 concurrently running tasks of the same type.

// Note: to avoid unnecessary task-switching overhead, it is normally
// better to combine multiple periodic tasks of the same type into a 
// single Handler Task. That is demonstrated in the Handler example elsewhere.

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.
#include "crt_TenTasks.h"
namespace crt
{
	MainInits mainInits;            // Initialize CleanRTOS.
	CounterTaskForTestTenTasks c0("c0", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c1("c1", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c2("c2", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c4("c4", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c5("c5", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c6("c6", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c7("c7", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c8("c8", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	CounterTaskForTestTenTasks c9("c9", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
}

// ********  end of constant part of this .ino file  **************

void setup()
{
	ESP_LOGI("checkpoint", "start of main");
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
