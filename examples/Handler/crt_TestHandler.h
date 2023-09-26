// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>
#include <crt_MutexSection.h>

namespace crt
{
	class CounterForTestHandler : public IHandlerListener
	{
	private:
		const char* CounterForTestHandlerName;
		int32_t count;
	public:
		CounterForTestHandler(const char *CounterForTestHandlerName, IHandler& CounterForTestHandlerHandler) : CounterForTestHandlerName(CounterForTestHandlerName), count(0)
		{
			CounterForTestHandlerHandler.addHandlerListener(this);
		}

		/*override keyword not supported in current compiler*/
		void update()
		{
			ESP_LOGI(CounterForTestHandlerName,"%d",count);
			count++;
		}
	}; // end class CounterForTestHandler
};// end namespace crt
