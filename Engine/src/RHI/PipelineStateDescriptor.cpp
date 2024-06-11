
// RHI
#include "PipelineStateDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		PipelineStateType PipelineStateDescriptor::GetType() const
		{
			return m_type;
		}

		bool PipelineStateDescriptor::operator==(const PipelineStateDescriptor& rhs) const
		{
			return m_type == rhs.m_type;
		}

		PipelineStateDescriptor::PipelineStateDescriptor(PipelineStateType pipelineStateType) : m_type{ pipelineStateType } {}

		PipelineStateDescriptorForDraw::PipelineStateDescriptorForDraw() : PipelineStateDescriptor(PipelineStateType::Draw) {}

		bool PipelineStateDescriptorForDraw::operator==(const PipelineStateDescriptorForDraw& rhs) const
		{
			return m_fragmentFunction == rhs.m_fragmentFunction &&
				m_renderStates == rhs.m_renderStates &&
				m_vertexFunction == rhs.m_vertexFunction &&
				m_inputStreamLayout == rhs.m_inputStreamLayout;
		}

		HashValue64 PipelineStateDescriptorForDraw::GetHash() const
		{
			HashValue64 seed = HashValue64{ 0 };

			if (m_vertexFunction)
			{
				seed = TypeHash64(m_vertexFunction->GetHash(), seed);
			}
			if (m_fragmentFunction)
			{
				seed = TypeHash64(m_fragmentFunction->GetHash(), seed);
			}
			// seed = TypeHash64(m_pipelineLayoutDescriptor->GetHash(), seed);
			seed = TypeHash64(m_inputStreamLayout.GetHash(), seed);
			seed = m_renderStates.GetHash(seed);

			return seed;
		}
	}
}