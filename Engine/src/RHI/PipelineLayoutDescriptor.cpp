
// RHI
#include "PipelineLayoutDescriptor.h"

namespace CGE
{
	namespace RHI
	{
        HashValue64 ResourceBindingInfo::GetHash() const
        {
            HashValue64 hash = TypeHash64(static_cast<uint32_t>(m_shaderStageMask));
            hash = TypeHash64(m_registerId, hash);
            return hash;
        }

        HashValue64 ShaderResourceGroupBindingInfo::GetHash() const
        {
            HashValue64 seed = HashValue64{ 0 };
            for (const auto& resourceInfo : m_resourcesRegisterMap)
            {
                seed = TypeHash64(resourceInfo.second, seed);
            }
            return seed;
        }

        bool PipelineLayoutDescriptor::IsFinalized() const
        {
            return m_hash != InvalidHash;
        }

        void PipelineLayoutDescriptor::Reset()
        {
            m_hash = InvalidHash;
            m_shaderResourceGroupLayoutsInfo.clear();
            m_bindingSlotToIndex.fill(RHI::Limits::Pipeline::ShaderResourceGroupCountMax);
            ResetInternal();
        }

        void PipelineLayoutDescriptor::AddShaderResourceGroupLayoutInfo(const ShaderResourceGroupLayout& layout, const ShaderResourceGroupBindingInfo& shaderResourceGroupInfo)
        {
            m_bindingSlotToIndex[layout.GetBindingSlot()] = static_cast<uint32_t>(m_shaderResourceGroupLayoutsInfo.size());
            m_shaderResourceGroupLayoutsInfo.push_back({ const_cast<ShaderResourceGroupLayout*>(&layout), shaderResourceGroupInfo });
        }

        ResultCode PipelineLayoutDescriptor::Finalize(const std::string name)
        {
            ResultCode resultCode = FinalizeInternal();
            if (resultCode == ResultCode::Success)
            {
                HashValue64 seed = HashValue64{ 0 };
                for (const ShaderResourceGroupLayoutInfo& layoutInfo : m_shaderResourceGroupLayoutsInfo)
                {
                    seed = TypeHash64(layoutInfo.first->GetHash(), seed);
                    seed = TypeHash64(layoutInfo.second.GetHash(), seed);
                }
                for (const auto& index : m_bindingSlotToIndex)
                {
                    seed = TypeHash64(index, seed);
                }
                m_hash = GetHashInternal(seed);
            }
            SetName(name);
            return resultCode;
        }

        const std::string& PipelineLayoutDescriptor::GetName() const
        {
            return m_name;
        }

        size_t PipelineLayoutDescriptor::GetShaderResourceGroupLayoutCount() const
        {
            assert(IsFinalized(), "Accessor called on a non-finalized pipeline layout. This is not permitted.");
            return m_shaderResourceGroupLayoutsInfo.size();
        }

        const ShaderResourceGroupLayout* PipelineLayoutDescriptor::GetShaderResourceGroupLayout(size_t index) const
        {
            assert(IsFinalized(), "Accessor called on a non-finalized pipeline layout. This is not permitted.");
            return m_shaderResourceGroupLayoutsInfo[index].first.get();
        }

        const ShaderResourceGroupBindingInfo& PipelineLayoutDescriptor::GetShaderResourceGroupBindingInfo(size_t index) const
        {
            assert(IsFinalized(), "Accessor called on a non-finalized pipeline layout. This is not permitted.");
            return m_shaderResourceGroupLayoutsInfo[index].second;
        }

        HashValue64 PipelineLayoutDescriptor::GetHash() const
        {
            assert(IsFinalized(), "Accessor called on a non-finalized pipeline layout. This is not permitted.");
            return m_hash;
        }

        uint32_t PipelineLayoutDescriptor::GetShaderResourceGroupIndexFromBindingSlot(uint32_t bindingSlot) const
        {
            assert(IsFinalized(), "Accessor called on a non-finalized pipeline layout. This is not permitted.");
            return m_bindingSlotToIndex[bindingSlot];
        }

        void PipelineLayoutDescriptor::ResetInternal() {}

        ResultCode PipelineLayoutDescriptor::FinalizeInternal()
        {
            return ResultCode::Success;
        }

        HashValue64 PipelineLayoutDescriptor::GetHashInternal(HashValue64 seed) const
        {
            return seed;
        }

        void PipelineLayoutDescriptor::SetName(const std::string& name)
        {
            m_name = name;
        }
	}
}