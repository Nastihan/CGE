#pragma once

// RHI
#include "RHI_Common.h"
#include "intrusive_base.h"
#include "ShaderResourceGroupLayoutDescriptor.h"

// std
#include <span>

namespace CGE
{
    namespace RHI
    {
        enum class ShaderResourceGroupType : uint16_t
        {
            Object = 0,
            Pass,
            Scene,
            Unknown
        };

        // For now I will use this class to build the root signature manually based on different passes and thier corresponding shaders.
        // [todo] Later I need to build based on input shaders using reflection.
        class ShaderResourceGroupLayout : public intrusive_base
        {
        public:
            static RHI::Ptr<ShaderResourceGroupLayout> Create();
            void Clear();
            void SetShaderResourceGroupType(ShaderResourceGroupType type);
            ShaderResourceGroupType GetShaderResourceGroupType() const;
            HashValue64 GetHash() const;

            void AddStaticSampler(const ShaderInputStaticSamplerDescriptor& sampler);
            void AddShaderInput(const ShaderInputBufferDescriptor& buffer);
            void AddShaderInput(const ShaderInputImageDescriptor& image);
            void AddShaderInput(const ShaderInputBufferUnboundedArrayDescriptor& bufferUnboundedArray);
            void AddShaderInput(const ShaderInputImageUnboundedArrayDescriptor& imageUnboundedArray);
            void AddShaderInput(const ShaderInputSamplerDescriptor& sampler);
            void AddShaderInput(const ShaderInputConstantDescriptor& constant);

            std::span<const ShaderInputStaticSamplerDescriptor> GetStaticSamplers() const;
            std::span<const ShaderInputBufferDescriptor> GetShaderInputListForBuffers() const;
            std::span<const ShaderInputImageDescriptor> GetShaderInputListForImages() const;
            std::span<const ShaderInputSamplerDescriptor> GetShaderInputListForSamplers() const;
            std::span<const ShaderInputConstantDescriptor> GetShaderInputListForConstants() const;
            std::span<const ShaderInputBufferUnboundedArrayDescriptor> GetShaderInputListForBufferUnboundedArrays() const;
            std::span<const ShaderInputImageUnboundedArrayDescriptor> GetShaderInputListForImageUnboundedArrays() const;

        private:
            ShaderResourceGroupLayout() = default;

            ShaderResourceGroupType m_srgType { ShaderResourceGroupType::Unknown };
            HashValue64 m_hash = HashValue64{ 0 };

            std::vector<ShaderInputStaticSamplerDescriptor> m_staticSamplers;
            std::vector<ShaderInputBufferDescriptor> m_inputsForBuffers;
            std::vector<ShaderInputImageDescriptor> m_inputsForImages;
            std::vector<ShaderInputSamplerDescriptor> m_inputsForSamplers;
            std::vector<ShaderInputBufferUnboundedArrayDescriptor> m_inputsForBufferUnboundedArrays;
            std::vector<ShaderInputImageUnboundedArrayDescriptor>  m_inputsForImageUnboundedArrays;
        };
    }
}