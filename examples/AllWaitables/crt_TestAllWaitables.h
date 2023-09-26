// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

// This file contains the code of multiple tasks that run concurrently and exchange information.
// Thus, all types of waitables are tested: flags, queues, timers and periodic timers.

// To see the test output in the serial monitor, just press the button that is assigned
// to the logger.

namespace crt
{
	extern ILogger& logger;
  
	class TestWaitables1 : public Task
	{
	private:
		Queue<int32_t, 5> queue1;
		Queue<int32_t, 5> queue2;

        Timer timer1;
        Timer sleepTimer;
        Flag flag1;

	public:
		/*static void StaticMain(void *pParam)
		{
			TestWaitables1* THIS = (TestWaitables1*) pParam;
			THIS->main();
		}*/

	public:
		TestWaitables1(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), queue1(this), queue2(this), timer1(this), sleepTimer(this), flag1(this)
		{
            logger.logText("Init TestWaitables1");
			start();
		}

    // Sending buffered data to another task should be done via a member function like the 
	// one below, which writes into a Queue.
	void ReportNumber1(int32_t number)
	{
        //logger.logText("writing in q1:");
        //logger.logInt32(number);
        bool bResult = queue1.write(number);
        if (!bResult)
        {
            logger.logText("queue1 was already full");
        }
	}

	void ReportNumber2(int32_t number)
	{
        //logger.logText("writing in q2:");
        //logger.logInt32(number);
		bool bResult = queue2.write(number);
        if (!bResult)
        {
            logger.logText("queue2 was already full");
        }
	}

	// Simply Notifying another task should be done via a member function like the 
	// one below, which sets a Flag.
    void Notify()
    {
        flag1.set();
    }

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			int32_t number=0;

            uint64_t timeMicros = 0;
            uint64_t timeMicrosNew = 0;

            uint64_t timeMicros2 = 0;
            uint64_t timeMicrosNew2 = 0;

            uint64_t timeMicros3 = 0;
            uint64_t timeMicrosNew3 = 0;

            timer1.start_periodic(750000);           		// This starts a periodic timer with a period of 750000 microseconds.

			ESP_LOGI("", "**************************************************************************");
			ESP_LOGI("", "**************************************************************************");
			ESP_LOGI("", "Please press the button attached to pinButtonDump (see AllWaitables_ino.h)");
			ESP_LOGI("", "**************************************************************************");
			ESP_LOGI("", "**************************************************************************");

			vTaskDelay(100);

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

                timeMicros3 = esp_timer_get_time();
                sleepTimer.sleep_us(1500);					// Test the one-shot timer with a sleep of 1500 microseconds.
                timeMicrosNew3 = esp_timer_get_time();
                logger.logText("sleep time:");
                logger.logUint32(timeMicrosNew3 - timeMicros3);

                waitAny(flag1 + queue1 + queue2 + timer1);  // Wait for any of the waitables to fire. (Note: WaitAll does exist as well).
                if (hasFired(flag1))                        // Note: using else if, only one event is handled at a time.
                {                                           // It is also possible to leave out the elses, potentially handling multiple events.
                    timeMicrosNew = esp_timer_get_time();   
                    logger.logText("Flag Interval:");       // Test reception of an activated flag.
                    logger.logUint32(timeMicrosNew - timeMicros);

                    timeMicros = timeMicrosNew;
                }
                else if (hasFired(queue1))
                {
                    queue1.read(number);
                    logger.logInt32(number);				// Test reception of a decreasing number via a queue from one task.
                }
                else if (hasFired(queue2))
                {
                    queue2.read(number);
                    logger.logInt32(number);				// Test reception of a increasing number via a queue from another task.
                }
                else if (hasFired(timer1))
                {
                    timeMicrosNew2 = esp_timer_get_time();
                    logger.logText("Timer Interval:");		// Test the interval of the periodic timer.
                    logger.logUint32(timeMicrosNew2 - timeMicros2);

                    timeMicros2 = timeMicrosNew2;
                }
			}
		}
	}; // end class TestWaitables1

	class TestWaitables2 : public Task
	{
	private:
		TestWaitables1& testWaitables1;

	public:
		static void StaticMain(void *pParam)
		{
			TestWaitables2* THIS = (TestWaitables2*)pParam;
			THIS->main();
		}

	public:
		TestWaitables2(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, TestWaitables1& testWaitables1) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), testWaitables1(testWaitables1)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			int32_t i = 0;

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased();
				vTaskDelay(300);
				testWaitables1.ReportNumber1(i--);
			}
		}
	}; // end class TestWaitables2

	class TestWaitables3 : public Task
	{
	private:
		TestWaitables1& testWaitables1;

	public:
		static void StaticMain(void *pParam)
		{
			TestWaitables3* THIS = (TestWaitables3*)pParam;
			THIS->main();
		}

	public:
		TestWaitables3(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, TestWaitables1& testWaitables1) :
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), testWaitables1(testWaitables1)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				dumpStackHighWaterMarkIfIncreased();
                vTaskDelay(2000);
                testWaitables1.Notify();
			}
		}
	}; // end class TestWaitables3

    class TestWaitables4 : public Task
    {
    private:
        TestWaitables1& testWaitables1;

    public:
        static void StaticMain(void *pParam)
        {
            TestWaitables4* THIS = (TestWaitables4*)pParam;
            THIS->main();
        }

    public:
        TestWaitables4(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, TestWaitables1& testWaitables1) :
            Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber), testWaitables1(testWaitables1)
        {
			start();
        }

    private:
        /*override keyword not supported*/
        void main()
        {
            vTaskDelay(1000); // wait for other threads to have started up as well.
            int32_t i = 10000;

            while (true)
            {
                dumpStackHighWaterMarkIfIncreased();
                vTaskDelay(600);
                testWaitables1.ReportNumber2(i++);
            }
        }
    }; // end class TestWaitables4
};// end namespace crt
