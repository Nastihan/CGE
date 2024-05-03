#pragma once

// RHI
#include "RHI_Common.h"
#include "TypeHash.h"
#include "Limits.h"

namespace CGE
{
	namespace RHI
	{
		enum class FilterMode : uint32_t
		{
			Point = 0,
			Linear
		};

		enum class ReductionType : uint32_t
		{
			Filter = 0,
			Comparison,
			Minimum,
			Maximum
		};

		enum class AddressMode : uint32_t
		{
			Wrap = 0,
			Mirror,
			Clamp,
			Border,
			MirrorOnce
		};

		enum class ComparisonFunc : uint32_t
		{
			Never = 0,
			Less,
			Equal,
			LessEqual,
			Greater,
			NotEqual,
			GreaterEqual,
			Always,
			Invalid
		};

		enum class BorderColor : uint32_t
		{
			OpaqueBlack = 0,
			TransparentBlack,
			OpaqueWhite
		};

		// Class used to initilize static 
		class SamplerState
		{
		public:
			SamplerState() = default;
			HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

			static SamplerState Create(FilterMode filterModeMinMag, FilterMode filterModeMip, AddressMode addressMode, BorderColor borderColor = BorderColor::TransparentBlack);
			static SamplerState CreateAnisotropic(uint32_t anisotropyMax, AddressMode addressMode);

			uint32_t m_anisotropyMax = 1;
			uint32_t m_anisotropyEnable = 0;
			FilterMode m_filterMin = FilterMode::Point;
			FilterMode m_filterMag = FilterMode::Point;
			FilterMode m_filterMip = FilterMode::Point;
			ReductionType m_reductionType = ReductionType::Filter;
			ComparisonFunc m_comparisonFunc = ComparisonFunc::Always;
			AddressMode m_addressU = AddressMode::Wrap;
			AddressMode m_addressV = AddressMode::Wrap;
			AddressMode m_addressW = AddressMode::Wrap;
			float m_mipLodMin = 0.0f;
			float m_mipLodMax = static_cast<float>(RHI::Limits::Image::MipCountMax);
			float m_mipLodBias = 0.0f;
			BorderColor m_borderColor = BorderColor::TransparentBlack;
		};
	}
}