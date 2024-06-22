
// RHI
#include "Graphics.h"
#include "RHI_Common.h"
#include "FrameGraphExecuter.h"
#include "PipelineStateDescriptor.h"

// DX12
#include "../DX_Interface/DX_Factory.h"
#include "../DX_Interface/DX_ImguiManager.h"

// std
#include <iostream>

namespace CGE
{
	namespace RHI
	{
		RHI::Ptr<Factory> Graphics::m_factory = nullptr;
		RHI::Ptr<BufferSystem> Graphics::m_bufferSystem = nullptr;
		RHI::Ptr<ImageSystem> Graphics::m_imageSystem = nullptr;
		RHI::Ptr<ImguiManager> Graphics::m_imguiManager = nullptr;
		std::unique_ptr<AssetProcessor> Graphics::m_assetProcessor = std::make_unique<AssetProcessor>();

		Graphics::Graphics(std::string backendAPI, Window& window) : m_backendAPI(std::move(backendAPI)), m_window{ window }
		{
#ifdef USE_DX12
			m_factory = new DX12::DX_Factory();
#endif // !USE_DX12

#ifdef USE_VULKAN
			m_factory = new VK::VK_Factory();
#endif // !USE_VULKAN

			m_bufferSystem = new RHI::BufferSystem();
			m_imageSystem = new RHI::ImageSystem();
			
			Init();
			std::cout << m_physicalDevice->GetDescriptor().m_cardName << std::endl;
		}

		RHI::Device& Graphics::GetDevice() const
		{
			return *m_device;
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

			m_bufferSystem->Init(*m_device);
			m_imageSystem->Init(*m_device);
			
			m_assetProcessor->InitShaderCompiler();
			m_assetProcessor->BuildShaderPermutations();

			m_frameGraphExecuter = m_factory->CreateFrameGraphExecuter();
			m_frameGraphExecuter->Init(*m_device);

			if (RHI::Graphics::GetFactory().GetBackendName() == "DX12")
			{
				m_imguiManager = new DX12::DX_ImguiManager();
				m_imguiManager->Init(GetDevice());
			}
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

		ImageSystem& Graphics::GetImageSystem()
		{
			return *m_imageSystem;
		}

		ImguiManager& Graphics::GetImguiManager()
		{
			return *m_imguiManager;
		}

		RHI::Ptr<RHI::FrameGraphExecuter> Graphics::GetFrameGraphExecuter() const
		{
			return m_frameGraphExecuter;
		}

		const AssetProcessor& Graphics::GetAssetProcessor()
		{
			return *m_assetProcessor;
		}
	}
}