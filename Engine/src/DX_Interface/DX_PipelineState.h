#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_PipelineLayout.h"

// RHI
#include "../RHI/PipelineState.h"
#include "../RHI/MultisampleState.h"

namespace CGE
{
	namespace DX12
	{
        class DX_Device;
        class DX_PipelineStateCache;

        struct PipelineStateDrawData
        {
            RHI::MultisampleState m_multisampleState;
            RHI::PrimitiveTopology m_primitiveTopology = RHI::PrimitiveTopology::Undefined;
        };

        struct PipelineStateData
        {
            PipelineStateData() : m_type(RHI::PipelineStateType::Draw) {}

            RHI::PipelineStateType m_type;
            union
            {
                // Only draw data for now.
                PipelineStateDrawData m_drawData;
            };
        };

        class DX_PipelineState final : public RHI::PipelineState
        {
            friend class DX_PipelineStateCache;
        public:
            static RHI::Ptr<DX_PipelineState> Create();
            const DX_PipelineLayout* GetPipelineLayout() const;
            ID3D12PipelineState* Get() const;
            const PipelineStateData& GetPipelineStateData() const;

        private:
            DX_PipelineState() = default;
            // RHI::PipelineState
            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::PipelineStateDescriptorForDraw& descriptor, RHI::PipelineLibrary* pipelineLibrary) override;
            void ShutdownInternal() override;

            RHI::ConstPtr<DX_PipelineLayout> m_pipelineLayout;
            RHI::Ptr<ID3D12PipelineState> m_pipelineState;

            // Only for Draw
            PipelineStateData m_pipelineStateData;
        };
	}
}