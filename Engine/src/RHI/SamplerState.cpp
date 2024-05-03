
// RHI
#include "SamplerState.h"

namespace CGE
{
	namespace RHI
	{
        HashValue64 SamplerState::GetHash(HashValue64 seed) const
        {
            return TypeHash64(*this, seed);
        }

        SamplerState SamplerState::Create(FilterMode filterModeMinMag, FilterMode filterModeMip, AddressMode addressMode, BorderColor borderColor)
        {
            SamplerState descriptor;
            descriptor.m_filterMin = descriptor.m_filterMag = filterModeMinMag;
            descriptor.m_filterMip = filterModeMip;
            descriptor.m_addressU = descriptor.m_addressV = descriptor.m_addressW = addressMode;
            descriptor.m_borderColor = borderColor;
            return descriptor;
        }

        SamplerState SamplerState::CreateAnisotropic(uint32_t anisotropyMax, AddressMode addressMode)
        {
            SamplerState descriptor;
            descriptor.m_anisotropyEnable = 1;
            descriptor.m_anisotropyMax = anisotropyMax;
            descriptor.m_addressU = descriptor.m_addressV = descriptor.m_addressW = addressMode;
            return descriptor;
        }
	}
}