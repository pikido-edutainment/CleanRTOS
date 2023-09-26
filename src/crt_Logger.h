// by Marius Versteegen, 2023

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_CleanRTOS.h"
#include "crt_LoggerTask.h"

// crt::Logger

// This is fast Logger. It logs strings and numbers into buffers in a few clock-cycles.
// It can be that fast because it does not use mutexes or other synchronisation mechanisms.
// The price for that speed is thus logging reliability. It might happen once in a while that 
// the code of a log instruction is interrupted by a timer interrupt which triggers a new log.
// That would result in the loss / overwrite of a log. So far, I am not aware of ever having
// witnessed such a case, but as it is possible, this fast logger should only be used to 
// facilitate debugging.

// (see the Logger example in the examples folder)
 
// To use this logger, add in the main.cpp (or the .ino file):
// const unsigned int pinButtonDump = 34; // Pressing a button connected to this pin dumps the latest logs to serial monitor.
// Logger<100> theLogger("Logger", 2 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump); // A fast logger for 100 logs.
// ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

// Anywhere else where you'd like to use the logger, add next lines:
// #include "crt_Logger.h" (not needed when already using #include <crt_CleanRTOS.h>)
// extern crt::ILogger& logger;

// That allows you to quickly log text and numbers:
// logger.logText("RGC_MeldHitReceived");
// logger.logInt32(playerID);
// logger.logUint32(hitpoints);
// logger.logFloat(fraction);

// On a buttonpress, the logs are streamed to the serial monitor and the buffers are cleared.
// The same can be achieved by calling "dumpNow".

namespace crt
{
    enum class LogType:uint8_t { lt_None, lt_Text, lt_Int32, lt_Uint32, lt_Float };

	// Don't select the LOGSIZE too high, or you may suffer from stack overflow troubles.
	template<unsigned int LOGSIZE> class Logger : public LoggerTask, public ILogger
	{
	private:
		// Important: if you add additional log types, don't forget to update the formula 
		// that converts LOGSIZE to stackSize in the initializer list of the constructor.
		const char *arTextLogs[LOGSIZE] = {}; // Circular buffer.
		int32_t arInt32Logs[LOGSIZE] = {};
        uint32_t arUint32Logs[LOGSIZE] = {};
		float arFloatLogs[LOGSIZE] = {};
        LogType arLogTypes[LOGSIZE];	// 0 means: end of logs. 1 means text type log. 2 means: int32 type log. 

		const char** pNextTextLog;
		int32_t* pNextInt32Log;
        uint32_t* pNextUint32Log;
		float* pNextFloatLog;
        LogType* pNextLogType;

		uint8_t pinButtonDump; // A press on this button (active low) activates the dump.

		volatile int32_t latestBatchCount;   // Count of logs since last dump. = index of arLogTypes to write to, if not cyclic.

		bool bPrinting;					     // Not a mutex, but perhaps safe enough for logging.

		bool prevPress = true;               // As the button is active low, this is what we need to start with to avoid a premature dump.
		unsigned int j = 0;

		unsigned prev_stack_hwm = 0, temp;

	public:
		static void StaticMain(void *pParam)
		{
			Logger<LOGSIZE>* THIS = (Logger<LOGSIZE>*) pParam;
			THIS->Main();
		}

	public:
		Logger(const char *taskName, unsigned int taskPriority, unsigned int taskCoreNumber, uint8_t pinButtonDump) :
			LoggerTask(taskName, taskPriority, 2000 + LOGSIZE * (sizeof(char*)+sizeof(int32_t)+sizeof(uint32_t) + sizeof(float) + sizeof(LogType)) /*taskSizeBytes*/, taskCoreNumber),
			pNextTextLog(arTextLogs), pNextInt32Log(arInt32Logs), pNextUint32Log(arUint32Logs), pNextFloatLog(arFloatLogs), pNextLogType(arLogTypes), pinButtonDump(pinButtonDump), latestBatchCount(0), bPrinting(false)
		{
			//SuspendSchedulerSection();
			clearLogs();
            start();    // The (one and only-) logger is allowed to start itself
                        // right after construction.
                        // That allows to debug the construction of other subsequent objects.
		}

		inline void clearLogs() 
		{
			// memset(arLogTypes, 0, LOGSIZE * sizeof(LogType)); initialisation is not needed: we can just follow the directions of the first lastestBatchCount LogType classifiers.
			pNextTextLog = arTextLogs;
			pNextInt32Log = arInt32Logs;
            pNextUint32Log = arUint32Logs;
			pNextFloatLog = arFloatLogs;
			pNextLogType = arLogTypes;
			latestBatchCount=0;

			// The last unit is meant for out of bounds safety.
			arTextLogs[LOGSIZE - 1] = "";
			arInt32Logs[LOGSIZE - 1] = 0;
            arUint32Logs[LOGSIZE - 1] = 0;
			arFloatLogs[LOGSIZE - 1] = 0;
			arLogTypes[LOGSIZE - 1] = LogType::lt_None;
		}

		inline void logText(const char *text)
		{
#ifdef CRT_DEBUG_LOGGING
			if (bPrinting) { return; } // prevent logging during printing: printing slows down everything, so the logs won't be representative anymore.
			//MutexSection(mutex, portMAX_DELAY);
			//SuspendSchedulerSection();
			if (latestBatchCount < (LOGSIZE - 1))
			{
				(*pNextTextLog++) = text;
				(*pNextLogType++) = LogType::lt_Text;
				latestBatchCount++;
			}
#endif
		}

		/*override keyword not supported in current compiler*/
		inline void logInt32(int32_t intNumber)
		{
#ifdef CRT_DEBUG_LOGGING
			if (bPrinting) { return; } // prevent logging during printing: printing slows down everything, so the logs won't be representative anymore.
			//MutexSection(mutex, portMAX_DELAY);
			//SuspendSchedulerSection();
			if (latestBatchCount++ >= (LOGSIZE - 1)) { --latestBatchCount; return; }
			(*pNextInt32Log++) = intNumber;
			(*pNextLogType++) = LogType::lt_Int32;
#endif
		}

        /*override keyword not supported in current compiler*/
        inline void logUint32(uint32_t intNumber)
        {
#ifdef CRT_DEBUG_LOGGING
            if (bPrinting) { return; } // prevent logging during printing: printing slows down everything, so the logs won't be representative anymore.
            //MutexSection(mutex, portMAX_DELAY);
            //SuspendSchedulerSection();
            if (latestBatchCount++ >= (LOGSIZE - 1)) { --latestBatchCount; return; }
            (*pNextUint32Log++) = intNumber;
            (*pNextLogType++) = LogType::lt_Uint32;
#endif
        }

		/*override keyword not supported in current compiler*/
		inline void logFloat(float floatNumber)
		{
#ifdef CRT_DEBUG_LOGGING
			if (bPrinting) { return; } // prevent logging during printing: printing slows down everything, so the logs won't be representative anymore.
			//MutexSection(mutex, portMAX_DELAY);
			//SuspendSchedulerSection();
			if (latestBatchCount++ >= (LOGSIZE - 1)) { --latestBatchCount; return; }
			(*pNextFloatLog++) = floatNumber;
			(*pNextLogType++) = LogType::lt_Float;
#endif
		}

    private:
		// Better keep the function below private: While printing logs, subsequent logs are muted (because of bPrintint = true).
        // So better activate dumpNow by buttonpress.
        // Another strange thing may happen if you call the function below directly in an early stage.
        // Somehow, that results in the instantiation of a second logger object.
		/*override*/ void dumpNow()
		{
#ifndef CRT_DEBUG_LOGGING
            return;
#endif
			bPrinting = true;
			
			const char** pNextTextLog = arTextLogs;
			int32_t* pNextInt32Log = arInt32Logs;
            uint32_t* pNextUint32Log = arUint32Logs;
			float* pNextFloatLog = arFloatLogs;
			LogType* pNextLogType = arLogTypes;

			pNextTextLog = arTextLogs;
			pNextInt32Log = arInt32Logs;
            pNextUint32Log = arUint32Logs;
			pNextFloatLog = arFloatLogs;
			pNextLogType = arLogTypes;

			int32_t latestLatestBatchCount = latestBatchCount;

			if (latestBatchCount == 0)
			{
				ESP_LOGI(LoggerTask::taskName, "%s", "no new logs");
			}

			for (int32_t i = 0; i < latestLatestBatchCount; i++)
			{
				LogType logType = *pNextLogType;
				pNextLogType++;

				if (logType == LogType::lt_Text)
				{
					ESP_LOGI(LoggerTask::taskName,"%s",*pNextTextLog++);
				}
				else if (logType == LogType::lt_Int32)
				{
					ESP_LOGI(LoggerTask::taskName,"%d",*pNextInt32Log++);
				}
                else if (logType == LogType::lt_Uint32)
                {
                    ESP_LOGI(LoggerTask::taskName, "%d", *pNextUint32Log++);
                }
				else if (logType == LogType::lt_Float)
				{
					ESP_LOGI(LoggerTask::taskName, "%f", *pNextFloatLog++);
				}
				else
				{
					ESP_LOGI(LoggerTask::taskName,"%d",latestLatestBatchCount);
					ESP_LOGI(LoggerTask::taskName,"%d",latestBatchCount);
					ESP_LOGI(LoggerTask::taskName,"%d",i);
					
					assert(false);
					break; // something's wrong.
				}

				vTaskDelay(1); // prevent slowdown due to buffer-overflow in arduino-serial monitor
			}

#ifdef CRT_HIGH_WATERMARK_INCREASE_LOGGING
			auto temp = uxTaskGetStackHighWaterMark(nullptr);
			if (!prev_stack_hwm || temp < prev_stack_hwm)
			{
                prev_stack_hwm = temp;
                ESP_LOGI("hwm_check", "Task %s has left: %d stack bytes and %d heap bytes", taskName, prev_stack_hwm, unsigned(xPortGetFreeHeapSize()));
				//ESP_LOGI(taskName,"%d",unsigned(xPortGetFreeHeapSize()));vTaskDelay(1);
				//ESP_LOGI(taskName," heap bytes","");vTaskDelay(1);
			}
#endif
			vTaskDelay(100);
			clearLogs();
			bPrinting = false;
			vTaskDelay(100);
		}

	private:
        // The logger is an exception. It is normally the first task that is instantiated. It starts itself.
        void start()
        {
#ifdef CRT_DEBUG_LOGGING
            xTaskCreatePinnedToCore(
                StaticMain
                , taskName            // A name just for humans
                , taskStackSizeBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
                , this
                , taskPriority
                , NULL
                , taskCoreNumber);
#endif
        }

		void Main()
		{
		    gpio_pad_select_gpio(pinButtonDump);
			gpio_set_direction((gpio_num_t)pinButtonDump, GPIO_MODE_INPUT);

			logText("StackBytes Reserved for Logger:");
			logInt32(taskStackSizeBytes);

            while (true)
			{
				bool buttonReadDump = (gpio_get_level((gpio_num_t)pinButtonDump) != 0);

				if (buttonReadDump && !prevPress /*&& Serial*/)
				{
					dumpNow();
				}
				prevPress = buttonReadDump;
				vTaskDelay(100);
			}
		}
	}; // end class Logger
}; // end namespace CleanRTOS
