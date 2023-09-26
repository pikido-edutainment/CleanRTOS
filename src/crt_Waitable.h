// by Marius Versteegen, 2023

#pragma once
#include "crt_CleanRTOS.h"

// Waitable is the base class of anything that a task can wait for.
// It is the base class of Flag, Queueand Timer.
// You don't need to use it directly yourself.

namespace crt 
{
	enum class WaitableType { wt_None, wt_Queue, wt_Flag, wt_Timer };

	class Waitable
	{
	protected:
		uint32_t		bitNumber;
		uint32_t		bitMask;
		static const uint32_t	bitMaskUndefined = 0x0fffffff;
		WaitableType    waitableType = WaitableType::wt_None;

	public:
		Waitable(WaitableType waitableType) :bitNumber(0), bitMask(bitMaskUndefined), waitableType(waitableType)
		{
		}

		//virtual bool operator==(const Waitable& other) const = 0;
		// Make next function virtual, and.. crash!
		inline WaitableType getType() const {return WaitableType::wt_None;}

		void init(uint32_t nBitNumber) { this->bitNumber = nBitNumber; this->bitMask = 1 << nBitNumber; }
		inline uint32_t getBitNumber() const {return bitNumber;}
		inline uint32_t getBitMask() const { return bitMask; }

		operator uint32_t()
		{
			return bitMask;
		}

		uint32_t operator+(uint32_t other)
		{
			return bitMask | other;
		}

        uint32_t operator+(Waitable& other)
        {
            return bitMask | other.getBitMask();
        }
	};
};
