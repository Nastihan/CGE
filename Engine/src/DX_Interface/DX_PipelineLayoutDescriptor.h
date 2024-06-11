#pragma once

// RHI
#include "../RHI/RHI_Common.h"
#include "../RHI/Handle.h"
#include "../RHI/TypeHash.h"
#include "../RHI/ShaderStages.h"
#include "../RHI/PipelineLayoutDescriptor.h"

namespace CGE
{
	namespace DX12
	{
        // Each srg will have one of this.
        // It will be used to get the index if the root parameter binding when we want to set the root arguments on the command list (check DX_CommandList::CommitShaderResources)
        class DX_RootParameterBinding
        {
        public:
            using RootParameterIndex = RHI::Handle<uint32_t>;

            // SetGraphicsRoot32BitConstants
            RootParameterIndex m_rootConstant;
            // SetComputeRootConstantBufferView
            RootParameterIndex m_constantBuffer;
            RootParameterIndex m_resourceTable;
            static const uint32_t MaxUnboundedArrays = 2;
            RootParameterIndex m_unboundedArrayResourceTables[MaxUnboundedArrays];
            RootParameterIndex m_bindlessTable;
            RootParameterIndex m_samplerTable;
        };

        struct DX_RootConstantBinding
        {
            DX_RootConstantBinding() = default;
            DX_RootConstantBinding(uint32_t constantCount, uint32_t constantRegister, uint32_t constantRegisterSpace);
            RHI::HashValue64 GetHash(RHI::HashValue64 seed = RHI::HashValue64{ 0 }) const;

            uint32_t m_constantCount = 0;
            uint32_t m_constantRegister = 0;
            uint32_t m_constantRegisterSpace = 0;
        };

        // Describes the shader stage mask for the descriptor table used by the SRG.
        struct DX_ShaderResourceGroupVisibility
        {
            DX_ShaderResourceGroupVisibility() = default;
            RHI::HashValue64 GetHash(RHI::HashValue64 seed = RHI::HashValue64{ 0 }) const;

            RHI::ShaderStageMask m_descriptorTableShaderStageMask = RHI::ShaderStageMask::None;
        };

        class DX_PipelineLayoutDescriptor final : public RHI::PipelineLayoutDescriptor
        {
            using Base = RHI::PipelineLayoutDescriptor;

        public:
            static RHI::Ptr<DX_PipelineLayoutDescriptor> Create();

            void SetRootConstantBinding(const DX_RootConstantBinding& rootConstantBinding);
            const DX_RootConstantBinding& GetRootConstantBinding() const;
            void AddShaderResourceGroupVisibility(const DX_ShaderResourceGroupVisibility& shaderResourceGroupVisibility);
            const DX_ShaderResourceGroupVisibility& GetShaderResourceGroupVisibility(uint32_t index) const;

        private:
            DX_PipelineLayoutDescriptor() = default;

            /// RHI::PipelineLayoutDescriptor
            RHI::HashValue64 GetHashInternal(RHI::HashValue64 seed) const override;

            DX_RootConstantBinding m_rootConstantBinding;
            std::vector<DX_ShaderResourceGroupVisibility> m_shaderResourceGroupVisibilities;
        };
	}
}