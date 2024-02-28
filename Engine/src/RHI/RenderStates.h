#pragma once

// RHI
#include "RHI_Common.h"
#include "Limits.h"
#include "TypeHash.h"

#include <array>

namespace CGE
{
	namespace RHI
	{
        enum class FillMode : uint32_t
        {
            Solid,
            Wireframe,
            Invalid
        };
        
        enum class CullMode : uint32_t
        {
            None,
            Front,
            Back,
            Invalid
        };

        enum class BlendFactor : uint32_t
        {
            Zero,
            One,
            ColorSource,
            ColorSourceInverse,
            AlphaSource,
            AlphaSourceInverse,
            AlphaDest,
            AlphaDestInverse,
            ColorDest,
            ColorDestInverse,
            AlphaSourceSaturate,
            Factor,
            FactorInverse,
            ColorSource1,
            ColorSource1Inverse,
            AlphaSource1,
            AlphaSource1Inverse,
            Invalid
        };

        enum class BlendOp : uint32_t
        {
            Add,
            Subtract,
            SubtractReverse,
            Minimum,
            Maximum,
            Invalid
        };

        enum class DepthWriteMask : uint32_t
        {
            Zero,
            All,
            Invalid
        };

        enum class ComparisonFunc : uint32_t
        {
            Never,
            Less,
            Equal,
            LessEqual,
            Greater,
            NotEqual,
            GreaterEqual,
            Always,
            Invalid
        };

        enum class StencilOp : uint32_t
        {
            Keep,
            Zero,
            Replace,
            IncrementSaturate,
            DecrementSaturate,
            Invert,
            Increment,
            Decrement,
            Invalid
        };

		struct RasterState
		{
            bool operator==(const RasterState& rhs) const;

            FillMode m_fillMode = FillMode::Solid;
            CullMode m_cullMode = CullMode::Back;
            int32_t m_depthBias = 0;
            float m_depthBiasClamp = 0.0f;
            float m_depthBiasSlopeScale = 0.0f;
            uint32_t m_multisampleEnable = 0;
            uint32_t m_depthClipEnable = 1;
            uint32_t m_conservativeRasterEnable = 0;
            uint32_t m_forcedSampleCount = 0;
		};

        struct TargetBlendState
        {
            bool operator==(const TargetBlendState& rhs) const;

            uint32_t m_enable = 0;
            uint32_t m_writeMask = 0xF;
            BlendFactor m_blendSource = BlendFactor::One;
            BlendFactor m_blendDest = BlendFactor::Zero;
            BlendOp m_blendOp = BlendOp::Add;
            BlendFactor m_blendAlphaSource = BlendFactor::One;
            BlendFactor m_blendAlphaDest = BlendFactor::Zero;
            BlendOp m_blendAlphaOp = BlendOp::Add;
        };

        struct BlendState
        {
            bool operator==(const BlendState& rhs) const;

            uint32_t m_alphaToCoverageEnable = 0;
            uint32_t m_independentBlendEnable = 0;
            std::array<TargetBlendState, Limits::Pipeline::AttachmentColorCountMax> m_targets;
        };

        struct DepthState
        {
            bool operator==(const DepthState& rhs) const;

            uint32_t m_enable = 1;
            DepthWriteMask m_writeMask = DepthWriteMask::All;
            ComparisonFunc m_func = ComparisonFunc::Less;
        };

        struct StencilOpState
        {
            bool operator==(const StencilOpState& rhs) const;

            StencilOp m_failOp = StencilOp::Keep;
            StencilOp m_depthFailOp = StencilOp::Keep;
            StencilOp m_passOp = StencilOp::Keep;
            ComparisonFunc m_func = ComparisonFunc::Always;
        };

        struct StencilState
        {
            bool operator==(const StencilState& rhs) const;

            uint32_t m_enable = 0;
            uint32_t m_readMask = 0xFF;
            uint32_t m_writeMask = 0xFF;
            StencilOpState m_frontFace;
            StencilOpState m_backFace;
        };

        struct DepthStencilState
        {
            bool operator==(const DepthStencilState& rhs) const;

            static DepthStencilState CreateDepth();
            static DepthStencilState CreateReverseDepth();
            static DepthStencilState CreateDisabled();

            DepthState m_depth;
            StencilState m_stencil;
        };

        struct RenderStates
        {
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;
            bool operator==(const RenderStates& rhs) const;

            RasterState m_rasterState;
            BlendState m_blendState;
            DepthStencilState m_depthStencilState;
        };
	}
}