#pragma once

// RHI
#include "RHI_Common.h"
#include "Limits.h"

// std
#include <array>

namespace CGE
{
	namespace RHI
	{
        struct SamplePosition
        {
            SamplePosition() = default;
            SamplePosition(uint8_t x, uint8_t y);

            bool operator==(const SamplePosition& other) const;
            bool operator!=(const SamplePosition& other) const;

            uint8_t m_x = 0;
            uint8_t m_y = 0;
        };

        // Used to configure the multi sample state of the pso
        struct MultisampleState
        {
            MultisampleState() = default;
            MultisampleState(uint16_t samples, uint16_t quality);

            bool operator==(const MultisampleState& other) const;
            bool operator!=(const MultisampleState& other) const;

            // [todo] Check whats this used for.
            std::array<SamplePosition, Limits::Pipeline::MultiSampleCustomLocationsCountMax> m_customPositions{};
            uint32_t m_customPositionsCount = 0;
            uint16_t m_samples = 1;
            uint16_t m_quality = 0;
        };
	}
}