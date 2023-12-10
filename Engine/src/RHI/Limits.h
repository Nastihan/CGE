#pragma once
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
		namespace Limits
		{
			namespace Device
			{
				constexpr uint32_t FrameCountMax = 3;

				constexpr uint32_t ClientWidth = 800;
				constexpr uint32_t ClientHeight = 600;

				// DX12 only supports flip model
				constexpr uint32_t MinSwapChainImages = 2;
			}
		}
	}
}