
// RHI
#include "Graphics.h"
#include "RHI_Common.h"
#include "FrameGraphExecuter.h"

// DX12
#include "../DX_Interface/DX_Factory.h"

// std
#include <iostream>

namespace CGE
{
	namespace RHI
	{
		RHI::Ptr<Factory> Graphics::m_factory = nullptr;
		RHI::Ptr<BufferSystem> Graphics::m_bufferSystem = nullptr;

		Graphics::Graphics(std::string backendAPI, Window& window) : m_backendAPI(std::move(backendAPI)), m_window{ window }
		{
#ifdef USE_DX12
			m_factory = new DX12::DX_Factory();
#endif // !USE_DX12

#ifdef USE_VULKAN
			m_factory = new VK::VK_Factory();
#endif // !USE_VULKAN

			m_bufferSystem = new RHI::BufferSystem();
			
			Init();
			std::cout << m_physicalDevice->GetDescriptor().m_cardName << std::endl;
		}

		void Graphics::Init()
		{
			m_physicalDevice = m_factory->CreatePhysicalDevice();

			m_device = m_factory->CreateDevice();
			m_device->Init(*m_physicalDevice);

			m_swapChain = m_factory->CreateSwapChain();
			SwapChainDescriptor swapChainDesc{};
			swapChainDesc.hWnd = m_window.GetHwnd();
			swapChainDesc.m_verticalSyncInterval = 0;
			swapChainDesc.m_scalingMode = Scaling::None;
			swapChainDesc.m_dimensions.m_imageWidth = m_window.GetWidth();
			swapChainDesc.m_dimensions.m_imageHeight = m_window.GetHeight();
			swapChainDesc.m_dimensions.m_imageCount = 3;
			swapChainDesc.m_dimensions.m_imageFormat = RHI::Format::R8G8B8A8_UNORM;

			m_swapChain->Init(*m_device.get(), swapChainDesc);

			m_frameGraphExecuter = m_factory->CreateFrameGraphExecuter();
			m_frameGraphExecuter->Init(*m_device);

			m_bufferSystem->Init(*m_device);
		}

		Factory& Graphics::GetFactory()
		{
			return *m_factory;
		}

		// [todo] 30.2 : 14:20
		void Graphics::Render()
		{
			m_frameGraphExecuter->RenderFrame();
			m_device->EndFrame();
		}

		ResultCode Graphics::RecreateSwapChain()
		{
			m_window.ResetResizeFlag();
			SwapChainDimensions dimensions = m_swapChain->GetDescriptor().m_dimensions;
			dimensions.m_imageWidth = m_window.GetWidth();
			dimensions.m_imageHeight = m_window.GetHeight();
			return m_swapChain->Resize(dimensions);
		}

		BufferSystem& Graphics::GetBufferSystem()
		{
			return *m_bufferSystem;
		}
	}
}