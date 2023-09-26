// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.
// That includes the Logger.

#include <crt_Logger.h>
#include <crt_Handler.h>

#include "crt_TestQueue.h"
namespace crt
{
 	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 34; // Pressing a button connected to this pin dumps the latest logs to serial monitor.
	
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

    MainInits mainInits;            // Initialize CleanRTOS

	NumberDisplayTask numberDisplayTask("NumberDisplayTask", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE); // Don't forget to call its start() memeber during setup().
	NumberSendTask    numberSendTask   ("NumberSendTask"   , 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, numberDisplayTask);
}

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200); // Only needed when using Serial.print();

	vTaskDelay(10);// allow logger to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);
	crt::logger.logText("starting");
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
