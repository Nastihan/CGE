// DX12
#include "DX_DescriptorPool.h"

// RHI
#include "../RHI/FreeListAllocator.h"
#include "../RHI/PoolAllocator.h"
#include "../RHI/Limits.h"

namespace CGE
{
    namespace DX12
    {
        void DX_DescriptorPool::Init(ID3D12DeviceX* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32_t descriptorCountForHeap, uint32_t descriptorCountForAllocator)
        {
            m_desc.Type = type;
            m_desc.Flags = flags;
            m_desc.NumDescriptors = descriptorCountForHeap;
            m_desc.NodeMask = 0;

            ID3D12DescriptorHeap* heap;
            DXAssertSuccess(device->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&heap)));
            heap->SetName(L"DescriptorHeap");

            m_descriptorHeap.Attach(heap);
            m_stride = device->GetDescriptorHandleIncrementSize(m_desc.Type);
            m_cpuStart = heap->GetCPUDescriptorHandleForHeapStart();

            const bool isGpuVisible = (flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (isGpuVisible)
            {
                m_gpuStart = heap->GetGPUDescriptorHandleForHeapStart();
            }

            if (isGpuVisible)
            {
                RHI::FreeListAllocator::Descriptor descriptor;
                descriptor.m_alignmentInBytes = 1;

                //It is possible for descriptorCountForAllocator to not match descriptorCountForHeap for DescriptorPoolShaderVisibleCbvSrvUav
                //heaps in which case descriptorCountForAllocator defines the number of static handles
                descriptor.m_capacityInBytes = static_cast<uint32_t>(descriptorCountForAllocator);
                descriptor.m_garbageCollectLatency = RHI::Limits::Device::FrameCountMax;

                RHI::FreeListAllocator* allocator = new RHI::FreeListAllocator();
                allocator->Init(descriptor);
                m_allocator.reset(allocator);
            }
            else
            {
                // Non-shader-visible heaps don't require contiguous descriptors. Therefore, we can allocate
                // them using a block allocator.

                RHI::PoolAllocator::Descriptor descriptor;
                descriptor.m_alignmentInBytes = 1;
                descriptor.m_elementSize = 1;
                descriptor.m_capacityInBytes = static_cast<uint32_t>(descriptorCountForAllocator);
                descriptor.m_garbageCollectLatency = 0;

                RHI::PoolAllocator* allocator = new RHI::PoolAllocator();
                allocator->Init(descriptor);
                m_allocator.reset(allocator);
            }
        }

        void DX_DescriptorPool::InitPooledRange(DX_DescriptorPool& parent, uint32_t offset, uint32_t count)
        {
            m_desc = parent.m_desc;
            m_descriptorHeap.Attach(parent.GetPlatformHeap());
            m_stride = parent.m_stride;
            m_cpuStart = parent.m_cpuStart;
            m_cpuStart.ptr += m_stride * offset;

            bool shaderVisible = RHI::CheckBitsAll(m_desc.Flags, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
            if (shaderVisible)
            {
                m_gpuStart = parent.m_gpuStart;
                m_gpuStart.ptr += m_stride * offset;
            }

            // NOTE: The range is currently only used for the static descriptor region of the shader visible heap, so
            // we leverage the PoolAllocator since these descriptors are allocated one-at-a-time (fragmentation free).
            RHI::PoolAllocator::Descriptor desc;
            desc.m_alignmentInBytes = 1;
            desc.m_elementSize = 1;
            desc.m_capacityInBytes = count;
            desc.m_garbageCollectLatency = RHI::Limits::Device::FrameCountMax;
            m_allocator = std::make_unique<RHI::PoolAllocator>();
            static_cast<RHI::PoolAllocator*>(m_allocator.get())->Init(desc);
        }

        ID3D12DescriptorHeap* DX_DescriptorPool::GetPlatformHeap() const
        {
            return m_descriptorHeap.Get();
        }

        DX_DescriptorHandle DX_DescriptorPool::AllocateHandle(uint32_t count)
        {
            RHI::VirtualAddress address;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                address = m_allocator->Allocate(count, 1);
            }

            if (address.IsValid())
            {
                DX_DescriptorHandle handle(m_desc.Type, m_desc.Flags, static_cast<uint32_t>(address.m_ptr));
                return handle;
            }
            else
            {
                return DX_DescriptorHandle{};
            }
        }

        void DX_DescriptorPool::ReleaseHandle(DX_DescriptorHandle handle)
        {
            if (handle.IsNull())
            {
                return;
            }

            std::lock_guard<std::mutex> lock(m_mutex);
            m_allocator->DeAllocate(RHI::VirtualAddress::CreateFromOffset(handle.m_index));
        }

        DX_DescriptorTable DX_DescriptorPool::AllocateTable(uint32_t count)
        {
            return DX_DescriptorTable(AllocateHandle(count), static_cast<uint16_t>(count));
        }

        void DX_DescriptorPool::ReleaseTable(DX_DescriptorTable table)
        {
            ReleaseHandle(table.GetOffset());
        }

        void DX_DescriptorPool::GarbageCollect()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_allocator->GarbageCollect();
        }

        D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorPool::GetCpuPlatformHandle(DX_DescriptorHandle handle) const
        {
            assert(handle.m_index != DX_DescriptorHandle::NullIndex, "Index is invalid");
            return D3D12_CPU_DESCRIPTOR_HANDLE{ m_cpuStart.ptr + handle.m_index * m_stride };
        }

        D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorPool::GetGpuPlatformHandle(DX_DescriptorHandle handle) const
        {
            assert(handle.IsShaderVisible(), "Handle is not shader visible");
            assert(handle.m_index != DX_DescriptorHandle::NullIndex, "Index is invalid");
            return D3D12_GPU_DESCRIPTOR_HANDLE{ m_gpuStart.ptr + (handle.m_index * m_stride) };
        }

        D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorPool::GetCpuPlatformHandleForTable(DX_DescriptorTable descTable) const
        {
            DX_DescriptorHandle handle = descTable.GetOffset();
            assert(handle.m_index != DX_DescriptorHandle::NullIndex, "Index is invalid");
            return D3D12_CPU_DESCRIPTOR_HANDLE{ m_cpuStart.ptr + handle.m_index * m_stride };
        }

        D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorPool::GetGpuPlatformHandleForTable(DX_DescriptorTable descTable) const
        {
            DX_DescriptorHandle handle = descTable.GetOffset();
            assert(handle.IsShaderVisible(), "Handle is not shader visible");
            assert(handle.m_index != DX_DescriptorHandle::NullIndex, "Index is invalid");
            return D3D12_GPU_DESCRIPTOR_HANDLE{ m_gpuStart.ptr + (handle.m_index * m_stride) };
        }
    }
}