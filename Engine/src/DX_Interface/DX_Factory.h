#pragma once
#include "../RHI/Factory.h"

namespace CGE
{
	namespace DX12
	{
		class DX_Factory final : public RHI::Factory
		{
		public:
			DX_Factory() = default;
			~DX_Factory() = default;

		public:
			RHI::Ptr<RHI::PhysicalDevice> CreatePhysicalDevice() override;
			RHI::Ptr<RHI::Device> CreateDevice() override;
			RHI::Ptr<RHI::SwapChain> CreateSwapChain() override;
			RHI::Ptr<RHI::FrameGraphExecuter> CreateFrameGraphExecuter() override;

			RHI::Ptr<RHI::BufferView> CreateBufferView() override;
			RHI::Ptr<RHI::Buffer> CreateBuffer() override;
			RHI::Ptr<RHI::BufferPool> CreateBufferPool() override;
		};
	}
}