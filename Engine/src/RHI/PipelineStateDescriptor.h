#pragma once

// RHI
#include "RHI_Common.h"
#include "ShaderStageFunction.h"
#include "TypeHash.h"
#include "InputStreamLayout.h"
#include "RenderStates.h"
#include "RenderAttachmentLayout.h"
#include "PipelineLayoutDescriptor.h"

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
            virtual HashValue64 GetHash() const = 0;

        protected:
            PipelineStateDescriptor(PipelineStateType pipelineStateType);

        private:
            PipelineStateType m_type = PipelineStateType::Count;

        public:
            ConstPtr<PipelineLayoutDescriptor> m_pipelineLayoutDescriptor = nullptr;
        };

        class PipelineStateDescriptorForDraw final : public PipelineStateDescriptor
        {
        public:
            PipelineStateDescriptorForDraw();
            bool operator==(const PipelineStateDescriptorForDraw& rhs) const;
            HashValue64 GetHash() const override;

            ConstPtr<ShaderStageFunction> m_vertexFunction;
            ConstPtr<ShaderStageFunction> m_fragmentFunction;
            InputStreamLayout m_inputStreamLayout;
            RenderStates m_renderStates;
            RenderAttachmentConfiguration m_renderAttachmentConfiguration;
        };
	}
}