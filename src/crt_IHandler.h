// by Marius Versteegen, 2023

#pragma once
#include "crt_IHandlerListener.h"

namespace crt
{
	class IHandler
	{
	public:
		virtual void addHandlerListener(IHandlerListener* pHandlerListener) = 0;
	};
};
