
// DX12
#include "DX_BufferPoolResolver.h"

// RHI
#include "../RHI/BufferViewDescriptor.h"

namespace CGE
{
	namespace DX12
	{
		DX_BufferPoolResolver::DX_BufferPoolResolver(DX_Device& device, const RHI::BufferPoolDescriptor& descriptor)
		{
            m_device = &device;

            if (RHI::CheckBitsAny(static_cast<uint32_t>(descriptor.m_bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::InputAssembly) | static_cast<uint32_t>(RHI::BufferBindFlags::DynamicInputAssembly)))
            {
                m_readOnlyState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER;
            }
            if (RHI::CheckBitsAll(static_cast<uint32_t>(descriptor.m_bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::Constant)))
            {
                m_readOnlyState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            }
            if (RHI::CheckBitsAll(static_cast<uint32_t>(descriptor.m_bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::ShaderRead)))
            {
                m_readOnlyState |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            }
            if (RHI::CheckBitsAll(static_cast<uint32_t>(descriptor.m_bindFlags), static_cast<uint32_t>(RHI::BufferBindFlags::Indirect)))
            {
                m_readOnlyState |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
            }
		}

		DX_CpuVirtualAddress DX_BufferPoolResolver::MapBuffer(const RHI::BufferMapRequest& request)
		{
            DX_MemoryView stagingMemory = m_device->AcquireStagingMemory(request.m_byteCount, DX_Alignment::Buffer);
            if (!stagingMemory.IsValid())
            {
                return nullptr;
            }

            DX_BufferUploadPacket uploadRequest;
            DX_Buffer* buffer = static_cast<DX_Buffer*>(request.m_buffer);

            uploadRequest.m_buffer = buffer;
            uploadRequest.m_memory = buffer->GetMemoryView().GetMemory();
            uploadRequest.m_memoryByteOffset = buffer->GetMemoryView().GetOffset() + request.m_byteOffset;
            uploadRequest.m_sourceMemory = stagingMemory;

            auto address = uploadRequest.m_sourceMemory.Map(RHI::HostMemoryAccess::Write);
            m_uploadPackets.emplace_back(std::move(uploadRequest));

            return address;
		}

		void DX_BufferPoolResolver::OnResourceShutdown(const RHI::Resource& resource)
		{

		}
	}
}