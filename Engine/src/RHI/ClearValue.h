#pragma once

// RHI
#include "RHI_Common.h"

// std
#include <array>

namespace CGE
{
	namespace RHI
	{
		enum class ClearValueType : uint32_t
		{
			Vector4Float = 0,
			Vector4Uint,
			DepthStencil
		};
		struct ClearValue
		{
			static ClearValue CreateDepth(float depth);
			static ClearValue CreateStencil(uint8_t stencil);
			static ClearValue CreateDepthStencil(float depth, uint8_t stencil);
			static ClearValue CreateVector4Float(float x, float y, float z, float w);
			static ClearValue CreateVector4Uint(uint32_t x, uint32_t y, uint32_t z, uint32_t w);

			ClearValueType m_type = ClearValueType::Vector4Float;
			float m_depth = 0.0f;
			uint8_t m_stencil = 0;
			std::array<float, 4> m_vector4Float;
			std::array<uint32_t, 4> m_vector4Uint;
		};
	}
}
