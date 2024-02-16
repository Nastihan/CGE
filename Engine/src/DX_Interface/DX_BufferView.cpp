
// DX12
#include "DX_BufferView.h"
#include "DX_Device.h"
#include "DX_Buffer.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_BufferView> DX_BufferView::Create()
		{
			return new DX_BufferView();
		}

		const DX_Buffer& DX_BufferView::GetBuffer() const
		{
			return static_cast<const DX_Buffer&>(Base::GetBuffer());
		}

		DX_DescriptorHandle DX_BufferView::GetReadDescriptor() const
		{
			return m_readDescriptor;
		}

		DX_DescriptorHandle DX_BufferView::GetReadWriteDescriptor() const
		{
			return m_readWriteDescriptor;
		}

		DX_DescriptorHandle DX_BufferView::GetClearDescriptor() const
		{
			return m_clearDescriptor;
		}

		DX_DescriptorHandle DX_BufferView::GetConstantDescriptor() const
		{
			return m_constantDescriptor;
		}

		DX_GpuVirtualAddress DX_BufferView::GetGpuAddress() const
		{
			return m_gpuAddress;
		}

		ID3D12Resource* DX_BufferView::GetMemory() const
		{
			return m_memory;
		}

		RHI::ResultCode DX_BufferView::InitInternal(RHI::Device& device, const RHI::Resource& resourceBase)
		{
			DX_Device& dxDevice = static_cast<DX_Device&>(device);

			const DX_Buffer& buffer = static_cast<const DX_Buffer&>(resourceBase);
			const RHI::BufferViewDescriptor& viewDescriptor = GetDescriptor();
			DX_DescriptorContext& descriptorContext = dxDevice.GetDescriptorContext();

			// By default, if no bind flags are specified on the view descriptor, attempt to create all views that are compatible with the underlying buffer's bind flags
			// If bind flags are specified on the view descriptor, only create the views for the specified bind flags.
			bool hasOverrideFlags = viewDescriptor.m_overrideBindFlags != RHI::BufferBindFlags::None;
			const RHI::BufferBindFlags bindFlags = hasOverrideFlags ? viewDescriptor.m_overrideBindFlags : buffer.GetDescriptor().m_bindFlags;

			m_memory = buffer.GetMemoryView().GetMemory();
			m_gpuAddress = buffer.GetMemoryView().GetGpuAddress() + viewDescriptor.m_elementOffset * viewDescriptor.m_elementSize;

			if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::ShaderRead)))
			{
				descriptorContext.CreateShaderResourceView(buffer, viewDescriptor, m_readDescriptor, m_staticReadDescriptor);
			}

			if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::ShaderWrite)))
			{
				descriptorContext.CreateUnorderedAccessView(buffer, viewDescriptor, m_readWriteDescriptor, m_clearDescriptor, m_staticReadWriteDescriptor);
			}

			if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::Constant)))
			{
				descriptorContext.CreateConstantBufferView(buffer, viewDescriptor, m_constantDescriptor, m_staticConstantDescriptor);
			}

			return RHI::ResultCode::Success;
		}

		RHI::ResultCode DX_BufferView::InvalidateInternal()
		{
			return InitInternal(GetDevice(), GetResource());
		}

		void DX_BufferView::ShutdownInternal()
		{
			DX_Device& dxDevice = static_cast<DX_Device&>(GetDevice());

			DX_DescriptorContext& descriptorContext = dxDevice.GetDescriptorContext();

			descriptorContext.ReleaseDescriptor(m_readDescriptor);
			descriptorContext.ReleaseDescriptor(m_readWriteDescriptor);
			descriptorContext.ReleaseDescriptor(m_clearDescriptor);
			descriptorContext.ReleaseDescriptor(m_constantDescriptor);

			descriptorContext.ReleaseStaticDescriptor(m_staticReadDescriptor);
			descriptorContext.ReleaseStaticDescriptor(m_staticReadWriteDescriptor);
			descriptorContext.ReleaseStaticDescriptor(m_staticConstantDescriptor);

			m_memory = nullptr;
			m_gpuAddress = 0;
		}
	}
}