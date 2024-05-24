
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
            const RHI::ShaderStageMask& mask,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_count{ bufferCount }
            , m_strideSize{ strideSize }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputBufferDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_strideSize, seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputImageDescriptor::ShaderInputImageDescriptor(
            const std::string& name,
            ShaderInputImageAccess access,
            ShaderInputImageType type,
            uint32_t imageCount,
            const RHI::ShaderStageMask& mask,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_count{ imageCount }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputImageDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputBufferUnboundedArrayDescriptor::ShaderInputBufferUnboundedArrayDescriptor(
            const std::string& name,
            ShaderInputBufferAccess access,
            ShaderInputBufferType type,
            uint32_t strideSize,
            const RHI::ShaderStageMask& mask,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_strideSize{ strideSize }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputBufferUnboundedArrayDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_strideSize, seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputImageUnboundedArrayDescriptor::ShaderInputImageUnboundedArrayDescriptor(
            const std::string& name,
            ShaderInputImageAccess access,
            ShaderInputImageType type,
            const RHI::ShaderStageMask& mask,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_access{ access }
            , m_type{ type }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputImageUnboundedArrayDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_access, seed);
            seed = TypeHash64(m_type, seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputSamplerDescriptor::ShaderInputSamplerDescriptor(
            const std::string& name,
            uint32_t samplerCount,
            const RHI::ShaderStageMask& mask,
            uint32_t registerId,
            uint32_t spaceId)
            : m_name{ name }
            , m_count{ samplerCount }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputSamplerDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_count, seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputConstantDescriptor::ShaderInputConstantDescriptor(const std::string& name, const RHI::ShaderStageMask& mask, uint32_t registerId, uint32_t spaceId)
            : m_name{ name }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputConstantDescriptor::GetHash(HashValue64 seed) const
        {
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        ShaderInputStaticSamplerDescriptor::ShaderInputStaticSamplerDescriptor(const std::string& name, const SamplerState& samplerState, const RHI::ShaderStageMask& mask, uint32_t registerId, uint32_t spaceId)
            : m_name{ name }
            , m_samplerState{ samplerState }
            , m_resourceBindingInfo{ mask, registerId, spaceId }
        {}

        HashValue64 ShaderInputStaticSamplerDescriptor::GetHash(HashValue64 seed) const
        {
            seed = m_samplerState.GetHash(seed);
            seed = TypeHash64(m_resourceBindingInfo, seed);
            return seed;
        }

        RootConstantBinding::RootConstantBinding(uint32_t constantCount, uint32_t constantRegister, uint32_t constantRegisterSpace)
            : m_constantCount(constantCount)
            , m_constantRegister(constantRegister)
            , m_constantRegisterSpace(constantRegisterSpace) {}
        
        HashValue64 RootConstantBinding::GetHash(HashValue64 seed) const
        {
            HashValue64 hash = TypeHash64(m_constantCount, seed);
            hash = TypeHash64(m_constantRegister, hash);
            hash = TypeHash64(m_constantRegisterSpace, hash);
            return hash;
        }
	}
}