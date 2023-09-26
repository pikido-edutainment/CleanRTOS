// by Marius Versteegen, 2023

#pragma once

namespace crt 
{
	namespace std
	{
		// This class works for Valuetypes.
		// It probably works for classes with copy-constructor and operator== implemented, but that's not tested yet.
		template<typename TYPE, uint32_t SIZE> class Stack
		{
		private:
			TYPE arItemIds[SIZE] = {};
			TYPE emptyResult;

			uint32_t indexNext;			// This is the index where the next item would be pushed.

		public:
			// emptyResult is the result that should be returned if the Stack is empty, and pop() or top() are used nonetheless.
			Stack(TYPE emptyResult) :emptyResult(emptyResult), indexNext(0)
			{
			}
			// implementation is in .cpp

		public:
			bool push(TYPE item)
			{
				if (indexNext < SIZE)
				{
					arItemIds[indexNext++] = item;
					return true;
				}

				return false;
			}

			TYPE pop()
			{
				if (indexNext > 0)
				{
					return arItemIds[--indexNext];
				}

				return emptyResult;
			}

			TYPE top()
			{
				if (indexNext > 0)
				{
					return arItemIds[indexNext - 1];
				}

				return emptyResult;
			}

			bool isEmpty()
			{
				return (indexNext == 0);
			}

			void clear()
			{
				indexNext = 0;
			}
		};
	};
};
