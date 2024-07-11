
// DX12
#include "DX_BufferPoolResolver.h"
#include "DX_CommandList.h"

// RHI
#include "../RHI/BufferViewDescriptor.h"

#include <d3dx12.h>

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
            buffer->m_pendingResolves++;

            uploadRequest.m_buffer = buffer;
            uploadRequest.m_memory = buffer->GetMemoryView().GetMemory();
            // buffer->GetMemoryView().GetOffset() is for our buffer page system.
            // request.m_byteOffset is the user requested offset
            // check how I'm updating the light structured buffer list in Scene.cpp
            uploadRequest.m_memoryByteOffset = buffer->GetMemoryView().GetOffset() + request.m_byteOffset;
            uploadRequest.m_sourceMemory = stagingMemory;

            auto address = uploadRequest.m_sourceMemory.Map(RHI::HostMemoryAccess::Write);
            m_uploadPackets.emplace_back(std::move(uploadRequest));

            return address;
		}

        // [todo - FrameGraph]
        void DX_BufferPoolResolver::Compile()
        {
            for (DX_BufferUploadPacket& packet : m_uploadPackets)
            {
                // We have to unmap before the resolve (copy) operation.
                packet.m_sourceMemory.Unmap(RHI::HostMemoryAccess::Write);
            }
        }

        void DX_BufferPoolResolver::OnResourceShutdown(const RHI::Resource& resource) {}

        void DX_BufferPoolResolver::Resolve(DX_CommandList& commandList) const
        {
            for (const DX_BufferUploadPacket& packet : m_uploadPackets)
            {
                const auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(packet.m_memory,
                    m_readOnlyState, D3D12_RESOURCE_STATE_COPY_DEST);
                commandList.GetCommandList()->ResourceBarrier(1, &barrier1);

                commandList.GetCommandList()->CopyBufferRegion(
                    packet.m_memory,
                    packet.m_memoryByteOffset,
                    packet.m_sourceMemory.GetMemory(),
                    packet.m_sourceMemory.GetOffset(),
                    packet.m_sourceMemory.GetSize());

                const auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(packet.m_memory,
                    D3D12_RESOURCE_STATE_COPY_DEST, m_readOnlyState);
                commandList.GetCommandList()->ResourceBarrier(1, &barrier2);
            }
        }

        void DX_BufferPoolResolver::QueueEpilogueTransitionBarriers(DX_CommandList& commandList) const {}

        void DX_BufferPoolResolver::Deactivate()
        {
            for (const auto& uploadPacket : m_uploadPackets)
            {
                assert(uploadPacket.m_buffer->m_pendingResolves, "There should be pending resolves if buffer is in m_uploadPackets list");
                uploadPacket.m_buffer->m_pendingResolves--;
            }
            m_uploadPackets.clear();
        }
	}
}