#pragma once

#include "RHI_Common.h"
#include "DeviceObject.h"
#include "Format.h"
#include "BufferPool.h"

namespace CGE
{
	namespace RHI
	{
        // Types of common buffer pools that buffer system provides.
        // Still need to build your own buffer pool for custom usage and budget.
        enum class CommonBufferPoolType : uint8_t
        {
            Constant = 0,           // For structured constants. They are often used as ConstantBuffer in shaders
            StaticInputAssembly,    // For input assembly buffers that are not updated often (vertex buffer)
            DynamicInputAssembly,   // For input assembly buffers that are updated per frame (vertex buffer)
            ReadBack,               // For gpu write cpu read buffers which is mainly used to read back gpu data
            ReadWrite,              // For gpu read/write buffers. They are often used as both StructuredBuffer and RWStructuredBuffer in different shaders
            ReadOnly,               // For buffers which are read only. They are usually only used as StructuredBuffer in shaders
            Indirect,               // For buffers which are used as indirect call arguments

            Count,
            Invalid = Count
        };

        struct CommonBufferDescriptor
        {
            std::string m_bufferName;
            CommonBufferPoolType m_poolType = CommonBufferPoolType::Invalid;
            uint32_t m_elementSize = 1;
            RHI::Format m_elementFormat = RHI::Format::Unknown; // will be used instead of m_elementSize
            u64 m_byteCount = 0;
            const void* m_bufferData = nullptr;
            bool m_isUniqueName = false;
        };

        class Buffer;

        class BufferSystem final : public DeviceObject
        {
        public:
            BufferSystem() = default;

            void Init(RHI::Device& device);
            void Shutdown();
            RHI::Ptr<RHI::BufferPool> GetCommonBufferPool(CommonBufferPoolType poolType);
            RHI::Ptr<Buffer> CreateBufferFromCommonPool(const CommonBufferDescriptor& descriptor);

        protected:
            bool CreateCommonBufferPool(CommonBufferPoolType poolType);

        private:
            RHI::Ptr<RHI::BufferPool> m_commonPools[static_cast<uint8_t>(CommonBufferPoolType::Count)];
            bool m_initialized = false;
        };
	}
}