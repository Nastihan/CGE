#pragma once

#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        struct Scissor
        {
            Scissor() = default;
            Scissor(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY);

            static const int32_t DefaultScissorMin = 0;
            static const int32_t DefaultScissorMax = std::numeric_limits<int32_t>::max();

            int32_t m_minX = DefaultScissorMin;
            int32_t m_minY = DefaultScissorMin;
            int32_t m_maxX = DefaultScissorMax;
            int32_t m_maxY = DefaultScissorMax;
        };
	}
}