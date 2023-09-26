// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.
#include "crt_TwoTasks.h"
namespace crt
{
	MainInits mainInits;            // Initialize CleanRTOS.
	SayHelloTask sayHelloTaskBert("Bert", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, "Hello Ernie!");
	SayHelloTask sayHelloTaskErnie("Ernie", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, "Hello Bert!");
}

void setup()
{
	ESP_LOGI("checkpoint", "start of main");
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
