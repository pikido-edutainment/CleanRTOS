// by Marius Versteegen, 2023

// Pools can be used to protect access to shared data without 
// explicitly worrying about mutexes. The class of the shared data should provide a copy constructor.
// Internally, a SimpleMutex is used to avoid concurrent access to the encapsulated data.
// (see the Pool example in the examples folder)

#pragma once
#include "internals/crt_FreeRTOS.h"
#include "internals/crt_SimpleMutex.h"
#include "crt_Mutex.h"
#include "crt_MutexSection.h"

namespace crt
{
	// This class allows automatic Mutex release using the RAI-pattern.
	template <class T> class Pool
	{
	private:
        T data;
		SimpleMutex simpleMutex;	// Unlike with Mutex, SimpleMutex does not offer deadlock protection,
	public:                         // but that is no problem because the Pool inheritely poses no deadlock thread. (no case of multiple mutexes that can have different lock orders).
		Pool()
		{}
		
		void write (T item) 
		{
			simpleMutex.lock();
			data = item;
			simpleMutex.unlock();
		}
      
		void read (T& item)
		{
			simpleMutex.lock();
			item = data;
			simpleMutex.unlock();
		}
	};
};
