#pragma once

// RHI
#include "RHI_Common.h"
#include "TypeHash.h"
#include "Handle.h"
#include "SamplerState.h"
#include "ShaderStages.h"

namespace CGE
{
    namespace RHI
    {
        enum class ShaderInputType : uint32_t
        {
            Buffer = 0,
            Image,
            Sampler,
            Constant,
            Count
        };
        static const uint32_t ShaderInputTypeCount = static_cast<uint32_t>(ShaderInputType::Count);

        enum class ShaderInputBufferAccess : uint32_t
        {
            Constant = 0,
            Read,
            ReadWrite // RW types in hlsl
        };

        inline ShaderInputBufferAccess operator|(ShaderInputBufferAccess a, ShaderInputBufferAccess b)
        {
            return static_cast<ShaderInputBufferAccess>(static_cast<int>(a) | static_cast<int>(b));
        }
        inline ShaderInputBufferAccess operator&(ShaderInputBufferAccess a, ShaderInputBufferAccess b)
        {
            return static_cast<ShaderInputBufferAccess>(static_cast<int>(a) & static_cast<int>(b));
        }

        enum class ShaderInputBufferType : uint32_t
        {
            Unknown = 0,
            Constant, // cbuffer in hlsl
            Structured, // StructuredBuffer<Light> or RWStructuredBuffer<Light> in hlsl
            Typed, // Buffer<float4> or RWBuffer<float4> in hlsl
            Raw, // ByteAddressBuffer or RWByteAddressBuffer in hlsl (These buffers are indexed in bytes)
            AccelerationStructure // RaytracingAccelerationStructure in hlsl
        };

        static const uint32_t UndefinedRegisterSlot = static_cast<uint32_t>(-1);

        class ShaderInputBufferDescriptor final
        {
        public:
            ShaderInputBufferDescriptor() = default;
            ShaderInputBufferDescriptor(const std::string& name,
                ShaderInputBufferAccess access,
                ShaderInputBufferType type,
                uint32_t bufferCount,
                uint32_t strideSize,
                uint32_t registerId,
                uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputBufferType m_type = ShaderInputBufferType::Unknown;
            ShaderInputBufferAccess m_access = ShaderInputBufferAccess::Read;
            uint32_t m_count = 0;

            // [todo] Not sure where to use ??
            uint32_t m_strideSize = 0;

            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        enum class ShaderInputImageAccess : uint32_t
        {
            Read = 0,
            ReadWrite // RW types in hlsl
        };

        inline ShaderInputImageAccess operator|(ShaderInputImageAccess a, ShaderInputImageAccess b)
        {
            return static_cast<ShaderInputImageAccess>(static_cast<int>(a) | static_cast<int>(b));
        }
        inline ShaderInputImageAccess operator&(ShaderInputImageAccess a, ShaderInputImageAccess b)
        {
            return static_cast<ShaderInputImageAccess>(static_cast<int>(a) & static_cast<int>(b));
        }

        enum class ShaderInputImageType : uint32_t
        {
            Unknown = 0,
            Image1D, // Texture1D<float4> or RWTexture1D<float4> in hlsl
            Image1DArray, // Texture1DArray<float4> or RWTexture1DArray<float4> in hlsl
            Image2D, // Texture2D<float4> or RWTexture2D<float4> in hlsl
            Image2DArray, // Texture2DArray<float4> or RWTexture2DArray<float4> in hlsl
            Image2DMultisample, // Texture2DMS<float4> or RWTexture2DMS<float4> in hlsl
            Image2DMultisampleArray, // Texture2DMSArray<float4> or RWTexture2DMSArray<float4> in hlsl
            Image3D, // Texture3D<float4> or RWTexture3D<float4> in hlsl
            ImageCube, // TextureCube<float4> in hlsl
            ImageCubeArray // TextureCubeArray<float4> in hlsl
        };

        class ShaderInputImageDescriptor final
        {
        public:
            ShaderInputImageDescriptor() = default;
            ShaderInputImageDescriptor(const std::string& name,
                ShaderInputImageAccess access,
                ShaderInputImageType type,
                uint32_t imageCount,
                uint32_t registerId,
                uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputImageType m_type = ShaderInputImageType::Unknown;
            ShaderInputImageAccess m_access = ShaderInputImageAccess::Read;
            uint32_t m_count = 0;
            
            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputBufferUnboundedArrayDescriptor final
        {
        public:
            ShaderInputBufferUnboundedArrayDescriptor() = default;
            ShaderInputBufferUnboundedArrayDescriptor(const std::string& name,
                ShaderInputBufferAccess access,
                ShaderInputBufferType type,
                uint32_t strideSize,
                uint32_t registerId,
                uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputBufferType m_type = ShaderInputBufferType::Unknown;
            ShaderInputBufferAccess m_access = ShaderInputBufferAccess::Read;

            // [todo] Not sure where to use ??
            uint32_t m_strideSize = 0;

            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputImageUnboundedArrayDescriptor final
        {
        public:
            ShaderInputImageUnboundedArrayDescriptor() = default;
            ShaderInputImageUnboundedArrayDescriptor(const std::string& name,
                ShaderInputImageAccess access,
                ShaderInputImageType type,
                uint32_t registerId,
                uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputImageType m_type = ShaderInputImageType::Unknown;
            ShaderInputImageAccess m_access = ShaderInputImageAccess::Read;
            
            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputSamplerDescriptor final
        {
        public:
            ShaderInputSamplerDescriptor() = default;
            ShaderInputSamplerDescriptor(const std::string& name, uint32_t samplerCount, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            uint32_t m_count = 0;
            
            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        // Used for root constants and push constants.
        // The way atom does it it will spawn one root constant from all the inline constant descriptors you push.
        // [todo] For now I wont do that and I will allow only one root constant for each srg.
        class ShaderInputConstantDescriptor final
        {
        public:
            ShaderInputConstantDescriptor() = default;
            ShaderInputConstantDescriptor(const std::string& name, uint32_t constantByteOffset, uint32_t constantByteCount, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputBufferType m_type = ShaderInputBufferType::Constant;
            uint32_t m_constantByteOffset = 0;
            uint32_t m_constantByteCount = 0;

            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputStaticSamplerDescriptor final
        {
        public:
            ShaderInputStaticSamplerDescriptor() = default;
            ShaderInputStaticSamplerDescriptor(const std::string& name, const SamplerState& samplerState, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            SamplerState m_samplerState;
            
            // [todo] Used if I want to offset the ranges in the shader (D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND is used)
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        using ShaderInputBufferIndex = Handle<uint32_t, ShaderInputBufferDescriptor>;
        using ShaderInputImageIndex = Handle<uint32_t, ShaderInputImageDescriptor>;
        using ShaderInputBufferUnboundedArrayIndex = Handle<uint32_t, ShaderInputBufferUnboundedArrayDescriptor>;
        using ShaderInputImageUnboundedArrayIndex = Handle<uint32_t, ShaderInputImageUnboundedArrayDescriptor>;
        using ShaderInputSamplerIndex = Handle<uint32_t, ShaderInputSamplerDescriptor>;
        using ShaderInputConstantIndex = Handle<uint32_t, ShaderInputConstantDescriptor>;
        using ShaderInputStaticSamplerIndex = Handle<uint32_t, ShaderInputStaticSamplerDescriptor>;
    }
}