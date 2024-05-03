
// RHI
#include "MultisampleState.h"

namespace CGE
{
	namespace RHI
	{
        SamplePosition::SamplePosition(uint8_t x, uint8_t y) : m_x(x), m_y(y)
        {
            assert(m_x < Limits::Pipeline::MultiSampleCustomLocationGridSize && m_y < Limits::Pipeline::MultiSampleCustomLocationGridSize);
        }

        bool SamplePosition::operator != (const SamplePosition& other) const
        {
            return !(*this == other);
        }

        bool SamplePosition::operator == (const SamplePosition& other) const
        {
            return m_x == other.m_x && m_y == other.m_y;
        }

        MultisampleState::MultisampleState(uint16_t samples, uint16_t quality) : m_samples{ samples }, m_quality{ quality } {}

        bool MultisampleState::operator == (const MultisampleState& other) const
        {
            return m_customPositions == other.m_customPositions &&
                m_customPositionsCount == other.m_customPositionsCount &&
                m_samples == other.m_samples &&
                m_quality == other.m_quality;
        }

        bool MultisampleState::operator != (const MultisampleState& other) const
        {
            return !(*this == other);
        }
	}
}