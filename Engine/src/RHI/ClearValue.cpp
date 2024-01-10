
// RHI
#include "ClearValue.h"

namespace CGE
{
	namespace RHI
	{
		ClearValue ClearValue::CreateDepth(float depth)
		{
			ClearValue value;
			value.m_type = ClearValueType::DepthStencil;
			value.m_depth = depth;
			value.m_stencil = 0;
			return value;
		}

		ClearValue ClearValue::CreateStencil(uint8_t stencil)
		{
			ClearValue value;
			value.m_type = ClearValueType::DepthStencil;
			value.m_depth = 0.0;
			value.m_stencil = stencil;
			return value;
		}

		ClearValue ClearValue::CreateDepthStencil(float depth, uint8_t stencil)
		{
			ClearValue value;
			value.m_type = ClearValueType::DepthStencil;
			value.m_depth = depth;
			value.m_stencil = stencil;
			return value;
		}

		ClearValue ClearValue::CreateVector4Float(float x, float y, float z, float w)
		{
			ClearValue value;
			value.m_type = ClearValueType::Vector4Float;
			value.m_vector4Float[0] = x;
			value.m_vector4Float[1] = y;
			value.m_vector4Float[2] = z;
			value.m_vector4Float[3] = w;
			return value;
		}

		ClearValue ClearValue::CreateVector4Uint(uint32_t x, uint32_t y, uint32_t z, uint32_t w)
		{
			ClearValue value;
			value.m_type = ClearValueType::Vector4Uint;
			value.m_vector4Uint[0] = x;
			value.m_vector4Uint[1] = y;
			value.m_vector4Uint[2] = z;
			value.m_vector4Uint[3] = w;
			return value;
		}
	}
}