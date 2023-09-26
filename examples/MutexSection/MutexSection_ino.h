// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.

// All Tasks should be created in this main file.

#include "crt_TestMutexSection.h"
namespace crt
{
	MainInits mainInits;            // Initialize CleanRTOS.
	
	// Shared resources
	int32_t sharedIntA;
	int32_t sharedIntB;
	int32_t sharedIntC;

	// The mutexes that guard them.
	
	// Note: this file is actually part of the main.cpp (or .ino) file, as this file is included from that.
	// It is a good idea to define all mutexes that your application needs in this file.
	// That way, you can make sure that every mutex gets a unique id, and that the
	// order of the ids is correct (when multiple mutexes are locked concurrently,
	// Within each thread (separately), MUTEXES WITH SMALLER ID MUST BE LOCKED BEFORE MUTEXES WITH LARGER ID.
	
	Mutex mutexSharedIntA(1);		// Mutex with id 1, which protects sharedIntA
	Mutex mutexSharedIntB(2);		// Mutex with id 2, which protects sharedIntB
	Mutex mutexSharedIntC(3);		// Mutex with id 3, ,, . 
	                                // NOTE: it is forbidden to lock a new mutex with an id
									// that is lower than the highest id of currently locked mutexes.
									// By using MutexSections, that rule is safeguarded.

	SharedNumberIncreaser sharedNumberIncreaserA("SharedNumberIncreaser A", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	                                            sharedIntA, mutexSharedIntA); // Don't forget to call its start() member during setup().
	SharedNumberIncreaser sharedNumberIncreaserB("SharedNumberIncreaser B", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	                                            sharedIntB, mutexSharedIntB); // Don't forget to call its start() member during setup().
	SharedNumberIncreaser sharedNumberIncreaserC("SharedNumberIncreaser C", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE,
	                                            sharedIntC, mutexSharedIntC); // Don't forget to call its start() member during setup().
    SharedNumbersDisplayer sharedNumbersDisplayer("SharedNumbersDisplayer", 2 /*priority*/, 4000 /*stackBytes*/, ARDUINO_RUNNING_CORE);
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
