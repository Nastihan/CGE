#pragma once

// RHI
#include "DrawItem.h"
#include "Viewport.h"
#include "Scissor.h"

namespace CGE
{
	namespace RHI
	{
		class CommandList
		{
		public:
			virtual void SetViewports(const Viewport* viewports, uint32_t count) = 0;
			virtual void SetScissors(const Scissor* scissors, uint32_t count) = 0;
			virtual void Submit(const DrawItem& drawItem) = 0;
		};
	}
}