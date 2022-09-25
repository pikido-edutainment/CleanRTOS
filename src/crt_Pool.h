// by Marius Versteegen, 2022

// Pools can be used to protect access to shared (global) variables without 
// explicitly worrying about mutexes.
// Internally, a mutex is used to avoid concurrent access to the encapsulated data.
// (see the Pool example in the examples folder)

#pragma once
#include "crt_FreeRTOS.h"
#include "crt_Mutex.h"
#include "crt_MutexSection.h"

namespace crt
{
	// This class allows automatic Mutex release using the RAI-pattern.
	template <class T> class Pool
	{
	private:
        T data;	
		Mutex& mutex;
	public:
		Pool(Mutex& mutex) : mutex(mutex)
		{}
		
		void write (T item) 
		{
			MutexSection ms(mutex);
			data = item;
		}
      
		T read () 
		{
			MutexSection ms(mutex);
			return data;
		}
	};
};
