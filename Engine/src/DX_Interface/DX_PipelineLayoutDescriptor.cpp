
// DX12
#include "DX_PipelineLayoutDescriptor.h"

namespace CGE
{
	namespace DX12
	{
        DX_RootConstantBinding::DX_RootConstantBinding(uint32_t constantCount, uint32_t constantRegister, uint32_t constantRegisterSpace)
            : m_constantCount(constantCount)
            , m_constantRegister(constantRegister)
            , m_constantRegisterSpace(constantRegisterSpace) {}

        RHI::HashValue64 DX_RootConstantBinding::GetHash(RHI::HashValue64 seed) const
        {
            RHI::HashValue64 hash = RHI::TypeHash64(m_constantCount, seed);
            hash = RHI::TypeHash64(m_constantRegister, hash);
            hash = RHI::TypeHash64(m_constantRegisterSpace, hash);
            return hash;
        }

        RHI::HashValue64 DX_ShaderResourceGroupVisibility::GetHash(RHI::HashValue64 seed) const
        {
            return RHI::TypeHash64(m_descriptorTableShaderStageMask, seed);
        }

        RHI::Ptr<DX_PipelineLayoutDescriptor> DX_PipelineLayoutDescriptor::Create()
        {
            return new DX_PipelineLayoutDescriptor();
        }

        void DX_PipelineLayoutDescriptor::SetRootConstantBinding(const DX_RootConstantBinding& rootConstantBinding)
        {
            m_rootConstantBinding = rootConstantBinding;
        }

        const DX_RootConstantBinding& DX_PipelineLayoutDescriptor::GetRootConstantBinding() const
        {
            return m_rootConstantBinding;
        }

        void DX_PipelineLayoutDescriptor::AddShaderResourceGroupVisibility(const DX_ShaderResourceGroupVisibility& shaderResourceGroupVisibility)
        {
            m_shaderResourceGroupVisibilities.push_back(shaderResourceGroupVisibility);
        }

        const DX_ShaderResourceGroupVisibility& DX_PipelineLayoutDescriptor::GetShaderResourceGroupVisibility(uint32_t index) const
        {
            return m_shaderResourceGroupVisibilities[index];
        }

        RHI::HashValue64 DX_PipelineLayoutDescriptor::GetHashInternal(RHI::HashValue64 seed) const
        {
            RHI::HashValue64 hash = RHI::TypeHash64(m_rootConstantBinding, seed);
            for (const auto& visibility : m_shaderResourceGroupVisibilities)
            {
                hash = RHI::TypeHash64(visibility.GetHash(), hash);
            }
            return hash;
        }
	}
}