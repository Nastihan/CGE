#pragma once

// RHI
#include "RHI_Common.h"
#include "TypeHash.h"
#include "Handle.h"
#include "SamplerState.h"

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
            ReadWrite
        };

        enum class ShaderInputBufferType : uint32_t
        {
            Unknown = 0,
            Constant,
            Structured,
            Typed,
            Raw,
            AccelerationStructure
        };

        static const uint32_t UndefinedRegisterSlot = static_cast<uint32_t>(-1);

        class ShaderInputBufferDescriptor final
        {
        public:
            ShaderInputBufferDescriptor() = default;
            ShaderInputBufferDescriptor(const std::string& name, ShaderInputBufferAccess access, ShaderInputBufferType type, uint32_t bufferCount, uint32_t strideSize, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the buffer input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputBufferType m_type = ShaderInputBufferType::Unknown;
            ShaderInputBufferAccess m_access = ShaderInputBufferAccess::Read;
            uint32_t m_count = 0;
            uint32_t m_strideSize = 0;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        enum class ShaderInputImageAccess : uint32_t
        {
            Read = 0,
            ReadWrite
        };

        enum class ShaderInputImageType : uint32_t
        {
            Unknown = 0,
            Image1D,
            Image1DArray,
            Image2D,
            Image2DArray,
            Image2DMultisample,
            Image2DMultisampleArray,
            Image3D,
            ImageCube,
            ImageCubeArray,
            SubpassInput
        };

        class ShaderInputImageDescriptor final
        {
        public:
            ShaderInputImageDescriptor() = default;
            ShaderInputImageDescriptor(const std::string& name, ShaderInputImageAccess access, ShaderInputImageType type, uint32_t imageCount, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputImageType m_type = ShaderInputImageType::Unknown;
            ShaderInputImageAccess m_access = ShaderInputImageAccess::Read;
            uint32_t m_count = 0;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputBufferUnboundedArrayDescriptor final
        {
        public:
            ShaderInputBufferUnboundedArrayDescriptor() = default;
            ShaderInputBufferUnboundedArrayDescriptor(const std::string& name, ShaderInputBufferAccess access, ShaderInputBufferType type, uint32_t strideSize, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputBufferType m_type = ShaderInputBufferType::Unknown;
            ShaderInputBufferAccess m_access = ShaderInputBufferAccess::Read;
            uint32_t m_strideSize = 0;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputImageUnboundedArrayDescriptor final
        {
        public:
            ShaderInputImageUnboundedArrayDescriptor() = default;
            ShaderInputImageUnboundedArrayDescriptor(const std::string& name, ShaderInputImageAccess access, ShaderInputImageType type, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            ShaderInputImageType m_type = ShaderInputImageType::Unknown;
            ShaderInputImageAccess m_access = ShaderInputImageAccess::Read;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputSamplerDescriptor final
        {
        public:
            ShaderInputSamplerDescriptor() = default;
            ShaderInputSamplerDescriptor(const std::string& name, uint32_t samplerCount, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            uint32_t m_count = 0;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputConstantDescriptor final
        {
        public:
            ShaderInputConstantDescriptor() = default;
            ShaderInputConstantDescriptor(const std::string& name, uint32_t constantByteOffset, uint32_t constantByteCount, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            uint32_t m_constantByteOffset = 0;
            uint32_t m_constantByteCount = 0;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };

        class ShaderInputStaticSamplerDescriptor final
        {
        public:
            ShaderInputStaticSamplerDescriptor() = default;
            ShaderInputStaticSamplerDescriptor(const std::string& name, const SamplerState& samplerState, uint32_t registerId, uint32_t spaceId);
            HashValue64 GetHash(HashValue64 seed = HashValue64{ 0 }) const;

            // The name id used to reflect the image input.
            // [todo] Empty for now need to reflect from shader.
            std::string m_name;
            SamplerState m_samplerState;
            uint32_t m_registerId = UndefinedRegisterSlot;
            uint32_t m_spaceId = UndefinedRegisterSlot;
        };
    }
}