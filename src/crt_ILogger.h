// by Marius Versteegen, 2023

#pragma once
namespace crt
{
	class ILogger
	{
	public:
		virtual void start() = 0;
		virtual void logText(const char *text) = 0;
		virtual void logInt32(int32_t intNumber) = 0;
        virtual void logUint32(uint32_t intNumber) = 0;
		virtual void logFloat(float floatNumber) = 0;
		virtual void dumpNow() = 0;
	};
};
