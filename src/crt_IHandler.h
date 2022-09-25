// by Marius Versteegen, 2022

#pragma once
#include "crt_IHandlerListener.h"

namespace crt
{
	class IHandler
	{
	public:
		virtual void start() = 0;
		virtual void addHandlerListener(IHandlerListener* pHandlerListener) = 0;
	};
};
