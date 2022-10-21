// by Marius Versteegen, 2022

// This application uses the CleanRTOS library.
// CleanRTOS is a collection of wrappers around FreeRTOS.
// It lives in the namespace crt (abbreviation of CleanRTOS).
// It's purpose is to make use of FreeRTOS easier and safer.
//
// To help prevent potential headerfile name clashes, it's constituent files are prefixed with crt_
// The classnames themselves don't need the prefix because of membership of the namespace.
// A benefit of that choice is that you can use autocompletion after typing crt:: to find all CleanRTOS 
// parts.
//

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.
// Handlers en Loggers are task objects, so let's include them here.
// In the rest of the code, we can reference (and link) to them via their interfaces ILogger and IHandler.
#include <crt_Logger.h>
#include <crt_Handler.h>

#include "crt_TestAllWaitables.h"
namespace crt
{
 	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 34; // Pressing a button connected to this pin dumps the latest logs to serial monitor.
	
	Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

	MainInits mainInits;            // Initialize CleanRTOS

//	Handler<10 /*MAXLISTENERCOUNT*/> buttonHandler3("ButtonHandler2", 2 /*priority*/, ARDUINO_RUNNING_CORE, 70 /*periodMs*/); // Don't forget to call its start() memeber during setup().
//	TestButtons testButtons("TestButtons", 2 /*priority*/, ARDUINO_RUNNING_CORE, buttonHandler3); // Don't forget to call its start() member during setup().

	TestWaitables1 testWaitables1("TestWaitables1", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE); // Don't forget to call its start() memeber during setup().
	TestWaitables2 testWaitables2("TestWaitables2", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, testWaitables1);
	TestWaitables3 testWaitables3("TestWaitables3", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, testWaitables1);
    TestWaitables4 testWaitables4("TestWaitables4", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE, testWaitables1);
}
// ********  end of constant part of this .ino file  **************

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200); // Only needed when using Serial.print();

	vTaskDelay(10);// allow logger to initialize.
	ESP_LOGI("checkpoint", "start of main");
	ESP_LOGI("NOTE", "You can inspect the Logs by pressing the LOG button.");
	crt::logger.logText("starting");
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
