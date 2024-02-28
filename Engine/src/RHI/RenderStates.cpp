
// RHI
#include "RenderStates.h"

namespace CGE
{
	namespace RHI
	{
		bool RasterState::operator==(const RasterState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(RasterState)) == 0);
		}

		bool TargetBlendState::operator==(const TargetBlendState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(TargetBlendState)) == 0);
		}

		bool BlendState::operator==(const BlendState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(BlendState)) == 0);
		}

		bool DepthState::operator==(const DepthState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(DepthState)) == 0);
		}

		bool StencilOpState::operator==(const StencilOpState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(StencilOpState)) == 0);
		}

		bool StencilState::operator==(const StencilState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(StencilState)) == 0);
		}

		bool DepthStencilState::operator==(const DepthStencilState& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(DepthStencilState)) == 0);
		}

		DepthStencilState DepthStencilState::CreateDepth()
		{
			DepthStencilState descriptor;
			descriptor.m_depth.m_func = ComparisonFunc::LessEqual;
			return descriptor;
		}

		DepthStencilState DepthStencilState::CreateReverseDepth()
		{
			DepthStencilState descriptor;
			descriptor.m_depth.m_func = ComparisonFunc::GreaterEqual;
			return descriptor;
		}

		DepthStencilState DepthStencilState::CreateDisabled()
		{
			DepthStencilState descriptor;
			descriptor.m_depth.m_enable = false;
			return descriptor;
		}

		HashValue64 RenderStates::GetHash(HashValue64 seed) const
		{
			return TypeHash64(*this, seed);
		}

		bool RenderStates::operator==(const RenderStates& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(RenderStates)) == 0);
		}
	}
}