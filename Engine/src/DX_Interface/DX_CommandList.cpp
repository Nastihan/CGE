#include "DX_CommandList.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_CommandList> DX_CommandList::Create()
		{
			return new DX_CommandList();
		}

		bool DX_CommandList::IsInitilized() const
		{
			return DX_CommandListBase::IsInitialized();
		}

		void DX_CommandList::Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator, const std::shared_ptr<DX_DescriptorContext>& descriptorContext)
		{
			DX_CommandListBase::Init(device, hardwareQueueClass, commandAllocator);
			m_descriptorContext = descriptorContext;
		}

		void DX_CommandList::Shutdown() {}

		void DX_CommandList::Close()
		{
			DX_CommandListBase::Close();
		}

		void DX_CommandList::ClearRenderTarget(const ImageClearRequest& request)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_descriptorContext->GetCpuPlatformHandle(request.handle);
			GetCommandList()->ClearRenderTargetView(descriptorHandle, request.m_clearValue.m_vector4Float.data(), 0, nullptr);
		}
	}
}