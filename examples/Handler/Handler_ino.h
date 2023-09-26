// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include <crt_Mutex.h>            // crt_Mutex.h must be included separately.

// All Tasks should be created in this main file.
#include <crt_Logger.h>
#include <crt_Handler.h>
#include "crt_TestHandler.h"
namespace crt
{
	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 34; // Pressing a button connected to this pin dumps the latest logs to serial monitor.

	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

    MainInits mainInits;            // Initialize CleanRTOS.
	Handler<10 /*MAXLISTENERCOUNT*/> CounterForTestHandlerHandler("CounterForTestHandlerHandler", 2 /*priority*/, ARDUINO_RUNNING_CORE, 1000 /*periodMs*/, 3000 /*batchTimeUs*/); // Don't forget to call its start() member during setup().
	CounterForTestHandler c0("c0", CounterForTestHandlerHandler);
	CounterForTestHandler c1("c1", CounterForTestHandlerHandler);
	CounterForTestHandler c2("c2", CounterForTestHandlerHandler);
	CounterForTestHandler c4("c4", CounterForTestHandlerHandler);
	CounterForTestHandler c5("c5", CounterForTestHandlerHandler);
	CounterForTestHandler c6("c6", CounterForTestHandlerHandler);
	CounterForTestHandler c7("c7", CounterForTestHandlerHandler);
	CounterForTestHandler c8("c8", CounterForTestHandlerHandler);
	CounterForTestHandler c9("c9", CounterForTestHandlerHandler);
}

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200);    // Only needed when using Serial.print();

	crt::CounterForTestHandlerHandler.start();  // IMPORTANT NOTE! a Handler task needs to be started manually,
								  // AFTER its listeners have been added or have added themselves to the handler.
	vTaskDelay(10);// allow tasks to initialize.
	ESP_LOGI("checkpoint", "start of main");vTaskDelay(1);
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
