#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_PipelineLayout.h"

// RHI
#include "../RHI/PipelineState.h"

namespace CGE
{
	namespace DX12
	{
        class DX_PipelineState final : public RHI::PipelineState
        {
        public:
            static RHI::Ptr<DX_PipelineState> Create();
            const DX_PipelineLayout* GetPipelineLayout() const;
            ID3D12PipelineState* Get() const;

        private:
            DX_PipelineState() = default;
            // RHI::PipelineState
            RHI::ResultCode InitInternal(RHI::Device& device, const RHI::PipelineStateDescriptorForDraw& descriptor, RHI::PipelineLibrary* pipelineLibrary) override;
            void ShutdownInternal() override;

            RHI::ConstPtr<DX_PipelineLayout> m_pipelineLayout;
            RHI::Ptr<ID3D12PipelineState> m_pipelineState;
        };
	}
}