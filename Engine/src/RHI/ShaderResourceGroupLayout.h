#pragma once

// RHI
#include "RHI_Common.h"
#include "intrusive_base.h"
#include "ShaderResourceGroupLayoutDescriptor.h"
#include "Interval.h"

// std
#include <span>
#include <optional>

namespace CGE
{
    namespace RHI
    {
        namespace SrgBindingSlot
        {
            // Logical bind slots sorted by frequency of update.
            static constexpr uint32_t Draw = 0;
            static constexpr uint32_t Object = 1;
            static constexpr uint32_t Material = 2;
            static constexpr uint32_t Pass = 3;
            static constexpr uint32_t View = 4;
            static constexpr uint32_t Scene = 5;

            // Bindless will be last since the table will be updated only once.
            static constexpr uint32_t Bindless = 6;
        };

        enum class ShaderResourceGroupType : uint16_t
        {
            Draw = 0,
            Object,
            Material,
            Pass,
            View,
            Scene,
            Bindless,
            Unknown
        };

        // If you look at an hlsl shader file you will see bunch of image, buffer and sampler resources of different types and in single or array form.
        // When you want to run a GPU command that executes that shader stage you need to set the pipeline layout (root signature) to the command list and pipeline state object.
        // These reources will be divided into groups that change based on different frequencies of update (Pass, Mateiral, Object ...).
        // Each of these groups will have a ShaderResourceGroupLayout which will correspond to the shader resources in the shader stages.
        // For now I will use this class to build the root signature manually based on different passes and their corresponding shaders.
        //
        // This is a very important class used both in the pipeline layout (root signature) and the shader resource groups bindings. (both should match)
        class ShaderResourceGroupLayout : public intrusive_base
        {
        public:
            static RHI::Ptr<ShaderResourceGroupLayout> Create();
            bool IsFinalized() const;
            void Clear();
            bool Finalize();
            void SetName(const std::string& name);
            const std::string& GetName() const;
            void SetShaderResourceGroupType(ShaderResourceGroupType type);
            ShaderResourceGroupType GetShaderResourceGroupType() const;
            HashValue64 GetHash() const;
            void SetBindingSlot(uint32_t bindingSlot);
            uint32_t GetBindingSlot() const;

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
            const std::optional<ShaderInputConstantDescriptor>& GetShaderInputConstant() const;

            // Shader Input List:                                                   (A)       (B)    (C)
            // Shader Resource Group:                                               [0, 1, 2] [3, 4] [5] ==> These are the intervals (m_groupSize will be 6 here)
            // Shader Resource Group Data (ShaderResourceGroupData::m_imageViews):  [0, 1, 2, 3, 4, 5] ==> The intervals can be used to set these.
            Interval GetGroupInterval(ShaderInputBufferIndex inputIndex) const;
            Interval GetGroupInterval(ShaderInputImageIndex inputIndex) const;
            Interval GetGroupInterval(ShaderInputSamplerIndex inputIndex) const;

            uint32_t GetGroupSizeForBuffers() const;
            uint32_t GetGroupSizeForImages() const;
            uint32_t GetGroupSizeForBufferUnboundedArrays() const;
            uint32_t GetGroupSizeForImageUnboundedArrays() const;
            uint32_t GetGroupSizeForSamplers() const;

            const ShaderInputBufferDescriptor& GetShaderInput(ShaderInputBufferIndex index) const;
            const ShaderInputImageDescriptor& GetShaderInput(ShaderInputImageIndex index) const;
            const ShaderInputSamplerDescriptor& GetShaderInput(ShaderInputSamplerIndex index) const;
            const std::optional<ShaderInputConstantDescriptor>& GetShaderInput() const;
            const ShaderInputBufferUnboundedArrayDescriptor& GetShaderInput(ShaderInputBufferUnboundedArrayIndex index) const;
            const ShaderInputImageUnboundedArrayDescriptor& GetShaderInput(ShaderInputImageUnboundedArrayIndex index) const;

            ShaderInputBufferIndex FindShaderInputBufferIndex(const std::string& name) const;
            ShaderInputImageIndex FindShaderInputImageIndex(const std::string& name) const;

            bool ValidateAccess(RHI::ShaderInputBufferIndex inputIndex, uint32_t arrayIndexEnd) const;
            bool ValidateAccess(RHI::ShaderInputImageIndex inputIndex, uint32_t arrayIndexEnd) const;
            bool ValidateAccess(RHI::ShaderInputSamplerIndex inputIndex, uint32_t arrayIndexEnd) const;

            bool ValidateAccess(RHI::ShaderInputBufferUnboundedArrayIndex inputIndex) const;
            bool ValidateAccess(RHI::ShaderInputImageUnboundedArrayIndex inputIndex) const;

        private:
            ShaderResourceGroupLayout() = default;

            template<typename IndexType>
            bool ValidateAccess(IndexType inputIndex, uint32_t arrayIndex, size_t inputIndexLimit, const char* inputArrayTypeName) const;

            template<typename IndexType>
            bool ValidateAccess(IndexType inputIndex, size_t inputIndexLimit, const char* inputArrayTypeName) const;

            template<typename ShaderInputDescriptorT, typename ShaderInputIndexT>
            bool FinalizeShaderInputGroup(const std::vector<ShaderInputDescriptorT>& shaderInputDescriptors, std::vector<Interval>& intervals, uint32_t& groupSize
            , std::unordered_map<std::string, ShaderInputIndexT>& reflectionMap);

            template<typename ShaderInputDescriptorT, typename ShaderInputIndexT>
            bool FinalizeUnboundedArrayShaderInputGroup(const std::vector<ShaderInputDescriptorT>& shaderInputDescriptors, uint32_t& groupSize);

        private:
            std::string m_name;

            ShaderResourceGroupType m_srgType { ShaderResourceGroupType::Unknown };

            // The logical binding slot used by all groups in this layout. This is sorted by frequency of update (most to least).
            // Object = 0, Pass = 1 and Scene = 2. (Check SrgBindingSlot)
            Handle<uint32_t> m_bindingSlot;
            HashValue64 m_hash = HashValue64{ 0 };

            std::vector<ShaderInputStaticSamplerDescriptor> m_staticSamplers;
            std::vector<ShaderInputBufferDescriptor> m_inputsForBuffers;
            std::vector<ShaderInputImageDescriptor> m_inputsForImages;
            std::vector<ShaderInputSamplerDescriptor> m_inputsForSamplers;
            std::vector<ShaderInputBufferUnboundedArrayDescriptor> m_inputsForBufferUnboundedArrays;
            std::vector<ShaderInputImageUnboundedArrayDescriptor> m_inputsForImageUnboundedArrays;
            std::optional<ShaderInputConstantDescriptor> m_inputForConstant;

            // ShaderResourceGroupLayout::GetGroupInterval function comments
            // The total size of the flat resource table for each type of resource.
            uint32_t m_groupSizeForBuffers = 0;
            uint32_t m_groupSizeForImages = 0;
            uint32_t m_groupSizeForSamplers = 0;
            uint32_t m_groupSizeForBufferUnboundedArrays = 0;
            uint32_t m_groupSizeForImageUnboundedArrays = 0;

            // ShaderResourceGroupLayout::GetGroupInterval function comments
            std::vector<Interval> m_intervalsForBuffers;
            std::vector<Interval> m_intervalsForImages;
            std::vector<Interval> m_intervalsForSamplers;

            std::unordered_map<std::string, ShaderInputBufferIndex> m_nameToIdxReflectionForBuffers;
            std::unordered_map<std::string, ShaderInputImageIndex> m_nameToIdxReflectionForImages;
            std::unordered_map<std::string, ShaderInputSamplerIndex> m_nameToIdxReflectionForSamplers;
        };
    }
}