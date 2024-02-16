
// RHI
#include "Graphics.h"
#include "Buffer.h"
#include "BufferPoolDescriptor.h"
#include "BufferSystem.h"
#include "BufferPool.h"
#include "BufferViewDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		void BufferSystem::Init(RHI::Device& device)
		{
			DeviceObject::Init(device);
            m_initialized = true;
			for (size_t i = 0; i < static_cast<uint8_t>(CommonBufferPoolType::Count); i++)
			{
				CreateCommonBufferPool(static_cast<CommonBufferPoolType>(i));
			}
		}

		void BufferSystem::Shutdown()
		{
			if (!m_initialized)
			{
				return;
			}
			for (uint8_t index = 0; index < static_cast<uint8_t>(CommonBufferPoolType::Count); index++)
			{
				m_commonPools[index] = nullptr;
			}
			m_initialized = false;
		}

		RHI::Ptr<RHI::BufferPool> BufferSystem::GetCommonBufferPool(CommonBufferPoolType poolType)
		{
			const uint8_t index = static_cast<uint8_t>(poolType);
			return m_commonPools[index];
		}

		bool BufferSystem::CreateCommonBufferPool(CommonBufferPoolType poolType)
		{
            if (!m_initialized)
            {
                return false;
            }
            auto* device = &GetDevice();

            RHI::Ptr<RHI::BufferPool> bufferPool = Graphics::GetFactory().CreateBufferPool();
            RHI::BufferPoolDescriptor bufferPoolDesc;
            switch (poolType)
            {
            case CommonBufferPoolType::Constant:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::Constant;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Device;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;

            case CommonBufferPoolType::StaticInputAssembly:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::InputAssembly | RHI::BufferBindFlags::ShaderRead;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Device;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;

            case CommonBufferPoolType::DynamicInputAssembly:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::DynamicInputAssembly | RHI::BufferBindFlags::ShaderRead;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Host;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;

            case CommonBufferPoolType::ReadBack:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::CopyWrite;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Host;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Read;
                break;

            case CommonBufferPoolType::ReadWrite:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::ShaderWrite | RHI::BufferBindFlags::ShaderRead | RHI::BufferBindFlags::CopyRead;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Device;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;

            case CommonBufferPoolType::ReadOnly:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::ShaderRead;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Device;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;

            case CommonBufferPoolType::Indirect:
                bufferPoolDesc.m_bindFlags = RHI::BufferBindFlags::ShaderReadWrite | RHI::BufferBindFlags::Indirect | RHI::BufferBindFlags::CopyRead | RHI::BufferBindFlags::CopyWrite;
                bufferPoolDesc.m_heapMemoryLevel = RHI::HeapMemoryLevel::Device;
                bufferPoolDesc.m_hostMemoryAccess = RHI::HostMemoryAccess::Write;
                break;
            default:
                assert(false, "Unknown common buffer pool type");
                return false;
            }

            bufferPool->SetName("CommonBufferPool_%i" + std::to_string(static_cast<uint32_t>(poolType)));
            RHI::ResultCode resultCode = bufferPool->Init(*device, bufferPoolDesc);
            if (resultCode != RHI::ResultCode::Success)
            {
                assert(false, "Failed to create buffer pool");
                return false;
            }

            m_commonPools[static_cast<uint8_t>(poolType)] = bufferPool;
            return true;
		}

		RHI::Ptr<Buffer> BufferSystem::CreateBufferFromCommonPool(const CommonBufferDescriptor& descriptor)
		{
            RHI::Ptr<RHI::BufferPool> bufferPool = GetCommonBufferPool(descriptor.m_poolType);

            if (!bufferPool)
            {
                assert(false, "Common buffer pool doesn't exist");
                return nullptr;
            }

            RHI::BufferDescriptor bufferDesc;
            bufferDesc.m_alignment = descriptor.m_elementSize;
            bufferDesc.m_bindFlags = bufferPool->GetDescriptor().m_bindFlags;
            bufferDesc.m_byteCount = descriptor.m_byteCount;

            RHI::BufferViewDescriptor viewDescriptor;
            if (descriptor.m_elementFormat != RHI::Format::Unknown)
            {
                viewDescriptor = RHI::BufferViewDescriptor::CreateTyped(0, static_cast<uint32_t>(bufferDesc.m_byteCount / descriptor.m_elementSize), descriptor.m_elementFormat);
            }
            else
            {
                viewDescriptor = RHI::BufferViewDescriptor::CreateStructured(0, static_cast<uint32_t>(bufferDesc.m_byteCount / descriptor.m_elementSize), descriptor.m_elementSize);
            }

            return nullptr;
		}
	}
}