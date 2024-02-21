#pragma once

// RHI
#include "RHI_Common.h"
#include "ShaderStageFunction.h"

namespace CGE
{
	namespace RHI
	{
        enum class PipelineStateType : uint32_t
        {
            Draw = 0,
            Dispatch,
            RayTracing,
            Count
        };

        class PipelineStateDescriptor
        {
        public:
            virtual ~PipelineStateDescriptor() = default;
            PipelineStateType GetType() const;
            bool operator==(const PipelineStateDescriptor& rhs) const;

            ConstPtr<PipelineLayout> m_pipelineLayoutDescriptor = nullptr;

        protected:
            PipelineStateDescriptor(PipelineStateType pipelineStateType);

        private:
            PipelineStateType m_type = PipelineStateType::Count;
        };

        class PipelineStateDescriptorForDraw final : public PipelineStateDescriptor
        {
        public:
            PipelineStateDescriptorForDraw();
            bool operator==(const PipelineStateDescriptorForDraw& rhs) const;

            ConstPtr<ShaderStageFunction> m_vertexFunction;
            ConstPtr<ShaderStageFunction> m_fragmentFunction;
            // InputStreamLayout m_inputStreamLayout;
            // RenderAttachmentConfiguration m_renderAttachmentConfiguration;
            // RenderStates m_renderStates;
        };
	}
}