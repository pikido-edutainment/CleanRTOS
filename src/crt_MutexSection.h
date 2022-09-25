// by Marius Versteegen, 2022

// Mutex Sections can be used to avoid concurrent resource usage
// within the scope they exist.

// Always use MutexSections in stead of locking mutexes directly.
// That way, it is ensured that every lock is always paired with a
// matching unlock().

// (see the MutexSection example in the examples folder)

#pragma once
#include "crt_Mutex.h"

namespace crt
{
	// This class allows automatic Mutex release using the RAI-pattern.
	class MutexSection
	{
	private:
		Mutex& mutex;
	public:
		MutexSection(Mutex& mutex) : mutex(mutex)
		{
			mutex.lock();
		}
		~MutexSection()
		{
			mutex.unlock();
		}
	};
};
