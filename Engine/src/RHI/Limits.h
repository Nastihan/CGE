#pragma once

// RHI
#include "RHI_Common.h"

// std
#include <limits>

namespace CGE
{
	namespace RHI
	{
		namespace Limits
		{
			namespace Device
			{
				constexpr uint32_t FrameCountMax = 3;

				constexpr uint32_t ClientWidth = 2560;
				constexpr uint32_t ClientHeight = 1440;

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
					// 4MB
					constexpr uint64_t UploadQueueStagingBufferSizeInBytes = 4ul * 1024 * 1024;
				}
			} // namespace DefaultValues

			namespace Pipeline
			{
				// Max vertex buffers
				constexpr uint32_t StreamCountMax = 12;

				// Max channels in each vertex buffer
				constexpr uint32_t StreamChannelCountMax = 16;
				// Max render targets we can bind to the pipeline
				constexpr uint32_t AttachmentColorCountMax = 8;
				// RenderAttachments + ResolveAttachments + DepthStencilAttachment +  ShadingRateAttachment
				constexpr uint32_t RenderAttachmentCountMax = 2 * AttachmentColorCountMax + 2;
				constexpr uint32_t SubpassCountMax = 10;
				constexpr uint32_t MultiSampleCustomLocationsCountMax = 16;
				constexpr uint32_t MultiSampleCustomLocationGridSize = 16;

				constexpr uint32_t ShaderResourceGroupCountMax = 7;
			}

			namespace Image
			{
				constexpr uint32_t MipCountMax = 15;
			}

			constexpr uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();
		}
	}
}