
// RHI
#include "ShaderResourceGroup.h"

namespace CGE
{
	namespace RHI
	{
		ResultCode ShaderResourceGroup::Init(Device& device, const ShaderResourceGroupData& shaderResourceGroupData)
		{
            DeviceObject::Init(device);
            assert(shaderResourceGroupData.GetLayout());
			SetData(shaderResourceGroupData);
			m_bindingSlot = GetLayout()->GetBindingSlot();
            ResetUpdateMaskForModifiedViews();
			return InitInternal(shaderResourceGroupData);
		}

		ResultCode ShaderResourceGroup::Compile()
		{
            // Modify m_rhiUpdateMask in case a view was modified. This can happen if a view is invalidated
            ResetUpdateMaskForModifiedViews();

            // Check if any part of the Srg was updated before trying to compile it
            if (IsAnyResourceTypeUpdated())
            {
                ResultCode resultCode = CompileInternal();
                return resultCode;
            }
            return ResultCode::Success;
		}

		const ShaderResourceGroupData& ShaderResourceGroup::GetData() const
		{
			return m_shaderResourceGroupData;
		}

		const ShaderResourceGroupLayout* ShaderResourceGroup::GetLayout() const
		{
			return m_shaderResourceGroupData.GetLayout();
		}

		uint32_t ShaderResourceGroup::GetBindingSlot() const
		{
			return m_bindingSlot;
		}

        void ShaderResourceGroup::EnableRhiResourceTypeCompilation(const ShaderResourceGroupData::ResourceTypeMask resourceTypeMask)
        {
            m_rhiUpdateMask = RHI::SetBits(m_rhiUpdateMask, static_cast<uint32_t>(resourceTypeMask));
        }

        HashValue64 ShaderResourceGroup::GetViewHash(const std::string& viewName)
        {
            return m_viewHash[viewName];
        }

        void ShaderResourceGroup::UpdateViewHash(const std::string& viewName, const HashValue64 viewHash)
        {
            if (m_viewHash.find(viewName) == m_viewHash.end())
            {
                m_viewHash.insert({ viewName, viewHash });
            }
            else
            {
                m_viewHash[viewName] = viewHash;
            }
        }

		void ShaderResourceGroup::SetData(const ShaderResourceGroupData& data)
		{
            m_shaderResourceGroupData = data;
		}

        template<typename T>
        HashValue64 ShaderResourceGroup::GetViewHash(std::span<const RHI::ConstPtr<T>> views)
        {
            HashValue64 viewHash = HashValue64{ 0 };
            for (size_t i = 0; i < views.size(); ++i)
            {
                if (views[i])
                {
                    viewHash = TypeHash64(views[i]->GetHash(), viewHash);
                }
            }
            return viewHash;
        }

        template<typename T>
        void ShaderResourceGroup::UpdateMaskBasedOnViewHash(const std::string& shaderInputDescriptorName, 
            std::span<const RHI::ConstPtr<T>> views, ShaderResourceGroupData::ResourceType resourceType)
        {
            // Recalculate the view hashesh
            HashValue64 viewHash = GetViewHash<T>(views);
            const auto& it = m_viewHash.find(shaderInputDescriptorName);
            if (it == m_viewHash.end() || it->second != viewHash)
            {
                EnableRhiResourceTypeCompilation(static_cast<ShaderResourceGroupData::ResourceTypeMask>(1u << (static_cast<uint32_t>(resourceType))));
                UpdateViewHash(shaderInputDescriptorName, viewHash);
            }
        }

		void ShaderResourceGroup::ResetUpdateMaskForModifiedViews()
		{
            const RHI::ShaderResourceGroupLayout& groupLayout = *GetLayout();
            uint32_t shaderInputIndex = 0;

            //Check image views
            for (const RHI::ShaderInputImageDescriptor& shaderInputImage : groupLayout.GetShaderInputListForImages())
            {
                const RHI::ShaderInputImageIndex imageInputIndex(shaderInputIndex);
                UpdateMaskBasedOnViewHash<RHI::ImageView>(shaderInputImage.m_name, m_shaderResourceGroupData.GetImageViewArray(imageInputIndex),
                    ShaderResourceGroupData::ResourceType::ImageView);
                ++shaderInputIndex;
            }

            shaderInputIndex = 0;
            //Check buffer views
            for (const RHI::ShaderInputBufferDescriptor& shaderInputBuffer : groupLayout.GetShaderInputListForBuffers())
            {
                const RHI::ShaderInputBufferIndex bufferInputIndex(shaderInputIndex);
                UpdateMaskBasedOnViewHash<RHI::BufferView>(shaderInputBuffer.m_name, m_shaderResourceGroupData.GetBufferViewArray(bufferInputIndex),
                    ShaderResourceGroupData::ResourceType::BufferView);
                ++shaderInputIndex;
            }

            shaderInputIndex = 0;
            //Check unbounded image views
            for (const RHI::ShaderInputImageUnboundedArrayDescriptor& shaderInputImageUnboundedArray : groupLayout.GetShaderInputListForImageUnboundedArrays())
            {
                const RHI::ShaderInputImageUnboundedArrayIndex imageUnboundedArrayInputIndex(shaderInputIndex);
                UpdateMaskBasedOnViewHash<RHI::ImageView>(shaderInputImageUnboundedArray.m_name, m_shaderResourceGroupData.GetImageViewUnboundedArray(imageUnboundedArrayInputIndex),
                    ShaderResourceGroupData::ResourceType::ImageViewUnboundedArray);
                ++shaderInputIndex;
            }

            shaderInputIndex = 0;
            //Check unbounded buffer views
            for (const RHI::ShaderInputBufferUnboundedArrayDescriptor& shaderInputBufferUnboundedArray : groupLayout.GetShaderInputListForBufferUnboundedArrays())
            {
                const RHI::ShaderInputBufferUnboundedArrayIndex bufferUnboundedArrayInputIndex(shaderInputIndex);
                UpdateMaskBasedOnViewHash<RHI::BufferView>(shaderInputBufferUnboundedArray.m_name, m_shaderResourceGroupData.GetBufferViewUnboundedArray(bufferUnboundedArrayInputIndex),
                    ShaderResourceGroupData::ResourceType::BufferViewUnboundedArray);
                ++shaderInputIndex;
            }
		}

        bool ShaderResourceGroup::IsAnyResourceTypeUpdated() const
        {
            return m_rhiUpdateMask != 0;
        }

        bool ShaderResourceGroup::IsResourceTypeEnabledForCompilation(uint32_t resourceTypeMask) const
        {
            return RHI::CheckBitsAny(m_rhiUpdateMask, resourceTypeMask);
        }
	}
}