// by Marius Versteegen

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "crt_Waitable.h"
#include "crt_Task.h"

// A Timer is a microsecond timer. It can be fire once (20 us or more) or periodic(50 us or more).
// The timer is also a waitable.It can be waited for by the task that owns it.

namespace crt
{
	class Timer;
	struct TimerCallBackInfo
	{
		Timer* pTimer;
		uint32_t bitMask;

		// bitmask is set by calling timer.setBitMask from Waitables during Task initialisation.
		TimerCallBackInfo() : pTimer(nullptr), bitMask(0)
		{}
		void init(Timer* pTimer, uint32_t bitMask)
		{
			this->pTimer = pTimer;
			this->bitMask = bitMask;
		}
	};

	class Timer : public Waitable
	{
	private:
		esp_timer_create_args_t timer_args;
		esp_timer_handle_t hTimer;

		TimerCallBackInfo timerCallBackInfo;
        Task* pTask;

	public:
        Timer(Task* pTask):Waitable(WaitableType::wt_Timer), pTask(pTask)
		{
            Waitable::init(pTask->queryBitNumber(this));	// This will cause the bitmask of Waitable to be set properly.
            timerCallBackInfo.init(this, Waitable::getBitMask());

            timer_args.callback = static_timer_callback;
            timer_args.name = "timer"; // name for debug purposes.
            timer_args.arg = &timerCallBackInfo;

            esp_err_t err = esp_timer_create(&timer_args, &hTimer);
            switch (err)
            {
            case ESP_ERR_INVALID_ARG:
                ESP_LOGE("Error:", "Invalid args timer create");
                assert(false);
                break;
            case ESP_ERR_INVALID_STATE:
                ESP_LOGE("Error:","Esp timer library not initialised yet. Make sure to call esp_timer_init() at the start of your main function");
                assert(false);
                break;
            case ESP_ERR_NO_MEM:
                ESP_LOGE("Error:", "Memory allocation failed at timer create");
                assert(false);
                break;
            default:
               break;
            }
        }

        inline void sleep_us(uint64_t duration_us)
        {
            start(duration_us);
            pTask->wait(*this);
        }

        inline void stop()
        {
            esp_timer_stop(hTimer);
        }

        inline void start(uint64_t duration_us)
        {
            assert(duration_us >= 50);                      // assert against bad design
            if (esp_timer_is_active(hTimer))
            {
                stop();
            }
            esp_timer_start_once(hTimer, duration_us-30);   // the starting once thing appears to have a 30us penalty.
        }

        inline void start_periodic(uint64_t period_us)
        {
            assert(period_us >= 50);                        // assert against bad design
            if (esp_timer_is_active(hTimer))
            {
                stop();
            }
            esp_timer_start_periodic(hTimer, period_us);
        }

		static void static_timer_callback(void* arg)
		{
			TimerCallBackInfo* pWCI = (TimerCallBackInfo*)arg;
			pWCI->pTimer->timer_callback(pWCI->bitMask);
		}

		inline void timer_callback(uint32_t bitMask)
		{
            pTask->setEventBits(bitMask);

            // Resumingly, this is how it works ( I think :-) ):
            // The timer generates a hardware interrupt.
            // That means: running code is interrupted (program counter and stackpointer are pushed on a special stack).
            // When the timer fires, this callback function is called as interrupt handler.
            // It simply sets an event bit that can be waited on in FreeRTOS by the Task that set the timer.
            // 
            // Upon exiting this function, the interrupt ends (program counter and stackpointer of where the code was 
            // running prior to the interrupt are popped from the stack and used).
		
			// Note: if CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD would be enabled (but it is disabled by default),
			// xEventGroupSetBitsFromISR should be used instead.
		}
	};
};
