#include "Graphics.h"
#include "../DX_Interface/DX_Factory.h"

#include <iostream>

namespace CGE
{
	namespace RHI
	{
		Graphics::Graphics(std::string backendAPI) : m_backendAPI(std::move(backendAPI)) 
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
		}
		// [todo] 30.2 : 14:20
		void Graphics::Render()
		{

		}
	}
}