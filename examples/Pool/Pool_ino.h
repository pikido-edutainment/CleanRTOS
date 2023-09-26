// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include <crt_Mutex.h>            // crt_Mutex.h must be included separately.

// All Tasks should be created in this main file.

#include "crt_TestPool.h"
namespace crt
{
	MainInits mainInits;            // Initialize CleanRTOS.

	// Note: this file is actually part of the main.cpp (or .ino) file, as this file is included from that.
	
	// Shared resources
	Pool<TwoNumbers> poolTwoNumbersA;								// Protected..
	TwoNumbers twoNumbersB;				   				            // Unprotected..

	SharedNumberIncreaser sharedNumberIncreaserA("SharedNumberIncreaser A", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	SharedNumberIncreaser sharedNumberIncreaserB("SharedNumberIncreaser B", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
	SharedNumberIncreaser sharedNumberIncreaserC("SharedNumberIncreaser C", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
}

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200); // Only needed when using Serial.print();

	vTaskDelay(10);// allow tasks to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
