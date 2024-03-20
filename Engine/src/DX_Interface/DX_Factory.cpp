#include "DX_Factory.h"

#include "DX_PhysicalDevice.h"
#include "DX_Device.h"
#include "DX_SwapChain.h"
#include "DX_FrameGraphExecuter.h"
#include "DX_BufferView.h"
#include "DX_Buffer.h"
#include "DX_BufferPool.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<RHI::PhysicalDevice> DX_Factory::CreatePhysicalDevice()
		{
			return DX_PhysicalDevice::Create();
		}

		RHI::Ptr<RHI::Device> DX_Factory::CreateDevice()
		{
			return DX_Device::Create();
		}

		RHI::Ptr<RHI::SwapChain> DX_Factory::CreateSwapChain()
		{
			return DX_SwapChain::Create();
		}

		RHI::Ptr<RHI::FrameGraphExecuter> DX_Factory::CreateFrameGraphExecuter()
		{
			return DX_FrameGraphExecuter::Create();
		}

		RHI::Ptr<RHI::BufferView> DX_Factory::CreateBufferView()
		{
			return DX_BufferView::Create();
		}

		RHI::Ptr<RHI::Buffer> DX_Factory::CreateBuffer()
		{
			return DX_Buffer::Create();
		}

		RHI::Ptr<RHI::BufferPool> DX_Factory::CreateBufferPool()
		{
			return DX_BufferPool::Create();
		}
	}
}