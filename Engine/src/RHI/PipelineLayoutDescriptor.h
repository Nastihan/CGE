#pragma once

// RHI
#include "RHI_Common.h"
#include "ShaderStages.h"
#include "TypeHash.h"
#include "intrusive_base.h"
#include "Limits.h"
#include "ShaderResourceGroupLayout.h"

// std
#include <array>

namespace CGE
{
	namespace RHI
	{
        struct ResourceBindingInfo
        {
            ResourceBindingInfo() = default;
            ResourceBindingInfo(const RHI::ShaderStageMask& mask, uint32_t registerId, uint32_t spaceId) : m_shaderStageMask{ mask }, m_registerId{ registerId }, m_spaceId{ spaceId } {}
            HashValue64 GetHash() const;

            using Register = uint32_t;
            static const Register InvalidRegister = ~0u;

            RHI::ShaderStageMask m_shaderStageMask = RHI::ShaderStageMask::None;
            Register m_registerId = InvalidRegister;
            uint32_t m_spaceId = InvalidRegister;
        };

        struct ShaderResourceGroupBindingInfo
        {
            ShaderResourceGroupBindingInfo() = default;
            HashValue64 GetHash() const;

            // Unused for now.
            std::unordered_map<std::string, ResourceBindingInfo> m_resourcesRegisterMap;
        };

        // This class will be input into platform specific pipeline layout classes (check DX_PipelineLayout::Init function)
        // This will be used to initilize the pipeline layout.
        // In this engine we bundle up resource bindings based on frequency of update (change) called shader resource group.
        // m_shaderResourceGroupLayoutsInfo - Each srg will have one ShaderResourceGroupLayout(All the binding slots) paired with
        // a ShaderResourceGroupBindingInfo that is just a map of name to ResourceBindingInfo for fast access.
        class PipelineLayoutDescriptor : public intrusive_base
        {
        public:
            virtual ~PipelineLayoutDescriptor() = default;

            bool IsFinalized() const;
            void Reset();
            // Add ordered by frequency of update.
            void AddShaderResourceGroupLayoutInfo(const ShaderResourceGroupLayout& layout, const ShaderResourceGroupBindingInfo& shaderResourceGroupInfo);
            RHI::ResultCode Finalize(const std::string name);
            const std::string& GetName() const;

            size_t GetShaderResourceGroupLayoutCount() const;
            const ShaderResourceGroupLayout* GetShaderResourceGroupLayout(size_t index) const;
            const ShaderResourceGroupLayout* GetShaderResourceGroupLayout(ShaderResourceGroupType srgType) const;
            const ShaderResourceGroupBindingInfo& GetShaderResourceGroupBindingInfo(size_t index) const;
            HashValue64 GetHash() const;
            uint32_t GetShaderResourceGroupIndexFromBindingSlot(uint32_t bindingSlot) const;

        protected:
            PipelineLayoutDescriptor();

        private:
            virtual void ResetInternal();
            virtual ResultCode FinalizeInternal();
            virtual HashValue64 GetHashInternal(HashValue64 seed) const;

            void SetName(const std::string& name);

            static constexpr HashValue64 InvalidHash = ~HashValue64{ 0 };
            using ShaderResourceGroupLayoutInfo = std::pair<Ptr<ShaderResourceGroupLayout>, ShaderResourceGroupBindingInfo>;

            // Each srg gets one. The should be sorted based on frequency on update.
            std::vector<ShaderResourceGroupLayoutInfo> m_shaderResourceGroupLayoutsInfo;
            std::array<uint32_t, RHI::Limits::Pipeline::ShaderResourceGroupCountMax> m_bindingSlotToIndex{};
            HashValue64 m_hash = InvalidHash;

            std::string m_name;
        };
	}
}