#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        struct Interval
        {
            Interval() = default;
            Interval(uint32_t min, uint32_t max) : m_min{ min }, m_max{ max } {}

            uint32_t m_min = 0;
            uint32_t m_max = 0;

            bool operator==(const Interval& rhs) const { return m_min == rhs.m_min && m_max == rhs.m_max; }
            bool operator!=(const Interval& rhs) const { return m_min != rhs.m_min || m_max != rhs.m_max; }
        };
	}
}