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

			namespace DefaultValues
			{
				namespace Memory
				{
					// 16MB page size
					constexpr uint64_t BufferPoolPageSizeInBytes = 16ul * 1024 * 1024;
					// 2MB page size
					constexpr uint64_t MediumStagingBufferPageSizeInBytes = 2ul * 1024 * 1024;
					// 134MB page size
					constexpr uint64_t LargestStagingBufferPageSizeInBytes = 128ul * 1024 * 1024;
				}
			} // namespace DefaultValues

			namespace Pipeline
			{
				// Max vertex buffers
				constexpr uint32_t StreamCountMax = 12;
				// Max channels in each vertex buffer
				constexpr uint32_t StreamChannelCountMax = 16;
				constexpr uint32_t AttachmentColorCountMax = 8;
			}
		}
	}
}