#pragma once

// RHI
#include "RHI_Common.h"
#include "Format.h"
#include "Handle.h"

// std
#include <string>

#include "../Window.h"

namespace CGE
{
	namespace RHI
	{
		enum class Scaling : uint32_t
		{
			None = 0,               // No scaling
			Stretch,                // Scale the source to fit the target
			AspectRatioStretch,     // Scale the source to fit the target while preserving the aspect ratio of the source
		};
		struct SwapChainDimensions
		{
			uint32_t m_imageCount = 0;
			uint32_t m_imageWidth = 0;
			uint32_t m_imageHeight = 0;
			Format m_imageFormat = Format::Unknown;
		};
		class SwapChainDescriptor
		{
		public:
			SwapChainDimensions m_dimensions;
			// 0: disable VSync. >= 1: sync N vertical blanks.
			uint32_t m_verticalSyncInterval = 0;

			Scaling m_scalingMode = Scaling::None;

			// Platform
			HWND hWnd;
		};
	}
}