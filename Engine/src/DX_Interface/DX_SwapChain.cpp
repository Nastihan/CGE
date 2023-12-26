// DX12
#include "DX_SwapChain.h"
#include "DX_Device.h"
#include "DX_Conversions.h"
#include "DX_CommandQueueContext.h"

// RHI
#include "../RHI/Limits.h"

// std
#include <algorithm>

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_SwapChain> DX_SwapChain::Create()
		{
			return new DX_SwapChain();
		}

		DX_Device& DX_SwapChain::GetDevice() const
		{
			return static_cast<DX_Device&>(RHI::SwapChain::GetDevice());
		}

		// In this function the swapchain descriptor has a hwnd so its platform dependant
		RHI::ResultCode DX_SwapChain::InitInternal(RHI::Device& device, const RHI::SwapChainDescriptor& descriptor, RHI::SwapChainDimensions* nativeDimensions)
		{
			DX_Device& dxDevice = static_cast<DX_Device&>(device);
			BOOL allowTearing = FALSE;
			wrl::ComPtr<IDXGIFactoryX> dxgiFactory;
			DXAssertSuccess(CreateDXGIFactory2(0, IID_PPV_ARGS(dxgiFactory.GetAddressOf())));
			m_isTearingSupported = SUCCEEDED(dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))) && allowTearing;

			if (nativeDimensions)
			{
				*nativeDimensions = descriptor.m_dimensions;
			}
			const uint32_t SwapBufferCount = std::max(RHI::Limits::Device::MinSwapChainImages, RHI::Limits::Device::FrameCountMax);

			DXGI_SWAP_CHAIN_DESCX swapChainDesc = {};
			swapChainDesc.Width = descriptor.m_dimensions.m_imageWidth;
			swapChainDesc.Height = descriptor.m_dimensions.m_imageHeight;
			swapChainDesc.Format = ConvertFormat(descriptor.m_dimensions.m_imageFormat);
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = SwapBufferCount;
			swapChainDesc.Scaling = ConvertScaling(descriptor.m_scalingMode);
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			if (m_isTearingSupported)
			{
				swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}

			const auto& cmdQueueCtx = dxDevice.GetCommandQueueContext();
			const auto& gfxCmdQueue = cmdQueueCtx.GetCommandQueue(RHI::HardwareQueueClass::Graphics);
			wrl::ComPtr<IDXGISwapChain1> swapChainPtr;
			DXAssertSuccess(dxgiFactory->CreateSwapChainForHwnd(gfxCmdQueue.GetPlatformQueue(), descriptor.hWnd, &swapChainDesc, nullptr, nullptr, &swapChainPtr));

			swapChainPtr->QueryInterface(IID_PPV_ARGS(m_swapChain.GetAddressOf()));

			if (m_isTearingSupported)
			{
				IDXGIFactoryX* parentFactory = nullptr;
				m_swapChain->GetParent(__uuidof(IDXGIFactoryX), (void**)&parentFactory);
				DXAssertSuccess(parentFactory->MakeWindowAssociation(descriptor.hWnd, DXGI_MWA_NO_ALT_ENTER));
			}

			return RHI::ResultCode::Success;
		}

		RHI::ResultCode DX_SwapChain::ResizeInternal(const RHI::SwapChainDimensions& dimensions, RHI::SwapChainDimensions* nativeDimensions)
		{
			return RHI::ResultCode::Success;
		}

		uint32_t DX_SwapChain::PresentInternal()
		{
			if (m_swapChain)
			{
				UINT presentFlags = (m_isTearingSupported) ? DXGI_PRESENT_ALLOW_TEARING : 0;
				HRESULT hr = m_swapChain->Present(GetDescriptor().m_verticalSyncInterval, presentFlags);
				GetDevice().DXAssertSuccess(hr);
				return m_swapChain->GetCurrentBackBufferIndex();
			}
			return -1;
		}

		void DX_SwapChain::SetVerticalSyncIntervalInternal(uint32_t previousVerticalSyncInterval)
		{
		}

		RHI::ResultCode DX_SwapChain::InitImagesInternal()
		{
			DX_Device& dxDevice = static_cast<DX_Device&>(GetDevice());

			for (size_t i = 0; i < RHI::Limits::Device::FrameCountMax; i++)
			{
				wrl::ComPtr<ID3D12Resource> backBuffer;
				DXAssertSuccess(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

				dxDevice.GetDescriptorContext().CreateRenderTargetView(backBuffer.Get(), m_swapChainDescriptorHandles[i]);

				m_backBuffers[i] = backBuffer;
			}

			return RHI::ResultCode::Success;
		}

		ID3D12Resource* DX_SwapChain::GetBackBuffer()
		{
			return m_backBuffers[GetCurrentImageIndex()].Get();
		}

		DX_DescriptorHandle& DX_SwapChain::GetBackBufferDescriptorHandle()
		{
			return m_swapChainDescriptorHandles[GetCurrentImageIndex()];
		}
	}
}