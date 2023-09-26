// by Marius Versteegen, 2023
//
// This file is included by CleanRTOS.h
// Normally, you don't need to change the settings below.

#pragma once
#define CRT_DEBUG_LOGGING
#define CRT_HIGH_WATERMARK_INCREASE_LOGGING
// #define ARDUINO_RUNNING_CORE 1  already defined in sdkconfig

namespace crt
{
	const uint32_t MAX_MUTEXNESTING = 20;

	// below, the mutexIDs directly involved in this test can be found.
	const uint32_t MutexID_Logger = (1 << 30);	// High ID, so can be nested very deeply.
};