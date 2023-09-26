// by Marius Versteegen, 2023

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
		Task* pTask;
		Mutex& mutex;
	public:
		MutexSection(Task* pTask, Mutex& mutex) : pTask(pTask), mutex(mutex)
		{
			mutex.lock(pTask);
		}
		~MutexSection()
		{
			mutex.unlock(pTask);
		}
	};
};
