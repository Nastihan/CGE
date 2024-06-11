
// RHI
#include "ShaderResourceGroupLayoutDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        ShaderInputBufferDescriptor::ShaderInputBufferDescriptor(
            const std::string& name,
            ShaderInputBufferAccess access,
            ShaderInputBufferType type,
            uint32_t bufferCount,
            uint32_t strideSize,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_count{ bufferCount }
            , m_strideSize{ strideSize }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputBufferDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_strideSize, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputImageDescriptor::ShaderInputImageDescriptor(
            const std::string& name,
            ShaderInputImageAccess access,
            ShaderInputImageType type,
            uint32_t imageCount,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_count{ imageCount }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputImageDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputBufferUnboundedArrayDescriptor::ShaderInputBufferUnboundedArrayDescriptor(
            const std::string& name,
            ShaderInputBufferAccess access,
            ShaderInputBufferType type,
            uint32_t strideSize,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_strideSize{ strideSize }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputBufferUnboundedArrayDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_strideSize, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputImageUnboundedArrayDescriptor::ShaderInputImageUnboundedArrayDescriptor(
            const std::string& name,
            ShaderInputImageAccess access,
            ShaderInputImageType type,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputImageUnboundedArrayDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputSamplerDescriptor::ShaderInputSamplerDescriptor(
            const std::string& name,
            uint32_t samplerCount,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_count{ samplerCount }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputSamplerDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputConstantDescriptor::ShaderInputConstantDescriptor(
            const std::string& name,
            uint32_t constantByteOffset,
            uint32_t constantByteCount,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_constantByteOffset{ constantByteOffset }
            , m_constantByteCount{ constantByteCount }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputConstantDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_constantByteOffset, seed);
            seed = TypeHash64(m_constantByteCount, seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }

        ShaderInputStaticSamplerDescriptor::ShaderInputStaticSamplerDescriptor(const std::string& name, const SamplerState& samplerState, uint32_t registerId, uint32_t spaceId)
            : m_name{ name }
            , m_samplerState{ samplerState }
            , m_registerId{ registerId }
            , m_spaceId{ spaceId } {}

        HashValue64 ShaderInputStaticSamplerDescriptor::GetHash(HashValue64 seed) const
        {
            seed = m_samplerState.GetHash(seed);
            seed = TypeHash64(m_registerId, seed);
            return seed;
        }
	}
}