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
		class PipelineLayoutDescriptor;
		class ShaderStageFunction;
		class PipelineState;
		class ShaderResourceGroup;
		class ShaderCompiler;

		class Factory : public Object
		{
		public:
			Factory() = default;
			virtual ~Factory() = default;
			REMOVE_COPY_AND_MOVE(Factory);

			const std::string& GetBackendName();

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

			virtual Ptr<PipelineState> CreatePipelineState() = 0;
			virtual Ptr<PipelineLayoutDescriptor> CreatePipelineLayoutDescriptor() = 0;
			virtual Ptr<ShaderStageFunction> CreateShaderStageFunction() = 0;
			virtual Ptr<ShaderResourceGroup> CreateShaderResourceGroup() = 0;

			virtual Ptr<ShaderCompiler> CreateShaderCompiler() = 0;

		protected:
			std::string m_backendName;
		};
	}
}