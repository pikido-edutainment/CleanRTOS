// by Marius Versteegen, 2023

#pragma once
namespace crt
{
	class IHandlerListener
	{
	public:
		virtual void update()=0;
	};
};