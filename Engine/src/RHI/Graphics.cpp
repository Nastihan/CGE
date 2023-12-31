#include "Graphics.h"
#include "../DX_Interface/DX_Factory.h"

#include <iostream>

namespace CGE
{
	namespace RHI
	{
		Graphics::Graphics(std::string backendAPI, const Window& window) : m_backendAPI(std::move(backendAPI)), m_window{ window }
		{
			if (m_backendAPI == "DX12")
			{
				m_factory = new DX12::DX_Factory();
			}
			else if (m_backendAPI == "VK")
			{
				// m_factory = new VK::VK_Factory();
			}
			else
			{
				std::cout << "Error, Please set correct name for backend API." << std::endl;
			}
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
			swapChainDesc.m_verticalSyncInterval = 1;
			swapChainDesc.m_scalingMode = Scaling::None;
			swapChainDesc.m_dimensions.m_imageWidth = m_window.GetWidth();
			swapChainDesc.m_dimensions.m_imageHeight = m_window.GetHeight();
			swapChainDesc.m_dimensions.m_imageCount = 3;
			swapChainDesc.m_dimensions.m_imageFormat = RHI::Format::R8G8B8A8_UNORM;

			m_swapChain->Init(*m_device.get(), swapChainDesc);
		}
		// [todo] 30.2 : 14:20
		void Graphics::Render()
		{

		}
	}
}