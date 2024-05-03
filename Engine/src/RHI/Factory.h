#pragma once
#include "RHI_Common.h"

#include "PhysicalDevice.h"
#include "Device.h"
#include "SwapChain.h"
#include "FrameGraphExecuter.h"

namespace CGE
{
	namespace RHI
	{
		class Device;
		class Buffer;
		class BufferPool;
		class BufferView;
		class Image;
		class ImagePool;
		class ImageView;

		class Factory : public Object
		{
		public:
			Factory() = default;
			virtual ~Factory() = default;
			REMOVE_COPY_AND_MOVE(Factory);

			static Factory& Get();

		public:
			virtual Ptr<PhysicalDevice> CreatePhysicalDevice() = 0;
			virtual Ptr<Device> CreateDevice() = 0;
			virtual Ptr<SwapChain> CreateSwapChain() = 0;
			virtual Ptr<FrameGraphExecuter> CreateFrameGraphExecuter() = 0;

			virtual Ptr<BufferView> CreateBufferView() = 0;
			virtual Ptr<Buffer> CreateBuffer() = 0;
			virtual Ptr<BufferPool> CreateBufferPool() = 0;

			virtual Ptr<ImageView> CreateImageView() = 0;
			virtual Ptr<Image> CreateImage() = 0;
			virtual Ptr<ImagePool> CreateImagePool() = 0;
		};
	}
}