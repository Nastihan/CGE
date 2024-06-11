
// RHI
#include "ShaderResourceGroupData.h"
#include "Interval.h"
#include "Image.h"
#include "Buffer.h"
#include "BufferPool.h"

// std
#include <array>

namespace CGE
{
	namespace RHI
	{
        ShaderResourceGroupData::ShaderResourceGroupData(const ShaderResourceGroupLayout* layout) : m_shaderResourceGroupLayout(layout)
        {
            m_imageViews.resize(layout->GetGroupSizeForImages());
            m_bufferViews.resize(layout->GetGroupSizeForBuffers());
            m_samplers.resize(layout->GetGroupSizeForSamplers());
        }

        bool ShaderResourceGroupData::SetImageView(ShaderInputImageIndex inputIndex, const ImageView* imageView, uint32_t arrayIndex)
        {
            std::array<const ImageView*, 1> imageViews = { {imageView} };
            return SetImageViewArray(inputIndex, imageViews, arrayIndex);
        }

        bool ShaderResourceGroupData::SetImageViewArray(ShaderInputImageIndex inputIndex, std::span<const ImageView* const> imageViews, uint32_t arrayIndex)
        {
            // We will send in the end array index (arrayIndex + imageViews.size() - 1) to validate.
            if (GetLayout()->ValidateAccess(inputIndex, static_cast<uint32_t>(arrayIndex + imageViews.size() - 1)))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                bool isValidAll = true;
                for (size_t i = 0; i < imageViews.size(); ++i)
                {
                    const bool isValid = ValidateSetImageView(inputIndex, imageViews[i], arrayIndex + i);
                    if (isValid)
                    {
                        m_imageViews[interval.m_min + arrayIndex + i] = imageViews[i];
                    }
                    isValidAll &= isValid;
                }
                if (!imageViews.empty())
                {
                    EnableResourceTypeCompilation(ResourceTypeMask::ImageViewMask);
                }
                return isValidAll;
            }
            return false;
        }

        bool ShaderResourceGroupData::SetImageViewUnboundedArray(ShaderInputImageUnboundedArrayIndex inputIndex, std::span<const ImageView* const> imageViews)
        {
            if (GetLayout()->ValidateAccess(inputIndex))
            {
                m_imageViewsUnboundedArray.clear();
                bool isValidAll = true;
                for (size_t i = 0; i < imageViews.size(); ++i)
                {
                    const bool isValid = ValidateImageViewAccess<ShaderInputImageUnboundedArrayIndex, ShaderInputImageUnboundedArrayDescriptor>(inputIndex, imageViews[i], static_cast<uint32_t>(i));
                    if (isValid)
                    {
                        m_imageViewsUnboundedArray.push_back(imageViews[i]);
                    }
                    isValidAll &= isValid;
                }

                if (!imageViews.empty())
                {
                    EnableResourceTypeCompilation(ResourceTypeMask::ImageViewUnboundedArrayMask);
                }
                return isValidAll;
            }
            return false;
        }

        bool ShaderResourceGroupData::SetBufferView(ShaderInputBufferIndex inputIndex, const BufferView* bufferView, uint32_t arrayIndex)
        {
            std::array<const BufferView*, 1> bufferViews = { {bufferView} };
            return SetBufferViewArray(inputIndex, bufferViews, arrayIndex);
        }

        bool ShaderResourceGroupData::SetBufferViewArray(ShaderInputBufferIndex inputIndex, std::span<const BufferView* const> bufferViews, uint32_t arrayIndex)
        {
            if (GetLayout()->ValidateAccess(inputIndex, static_cast<uint32_t>(arrayIndex + bufferViews.size() - 1)))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                bool isValidAll = true;
                for (size_t i = 0; i < bufferViews.size(); ++i)
                {
                    const bool isValid = ValidateSetBufferView(inputIndex, bufferViews[i], arrayIndex);
                    if (isValid)
                    {
                        m_bufferViews[interval.m_min + arrayIndex + i] = bufferViews[i];
                    }
                    isValidAll &= isValid;
                }

                if (!bufferViews.empty())
                {
                    EnableResourceTypeCompilation(ResourceTypeMask::BufferViewMask);
                }
                return isValidAll;
            }
            return false;
        }

        bool ShaderResourceGroupData::SetBufferViewUnboundedArray(ShaderInputBufferUnboundedArrayIndex inputIndex, std::span<const BufferView* const> bufferViews)
        {
            if (GetLayout()->ValidateAccess(inputIndex))
            {
                m_bufferViewsUnboundedArray.clear();
                bool isValidAll = true;
                for (size_t i = 0; i < bufferViews.size(); ++i)
                {
                    const bool isValid = ValidateBufferViewAccess<ShaderInputBufferUnboundedArrayIndex, ShaderInputBufferUnboundedArrayDescriptor>(inputIndex, bufferViews[i], static_cast<uint32_t>(i));
                    if (isValid)
                    {
                        m_bufferViewsUnboundedArray.push_back(bufferViews[i]);
                    }
                    isValidAll &= isValid;
                }

                if (!bufferViews.empty())
                {
                    EnableResourceTypeCompilation(ResourceTypeMask::BufferViewUnboundedArrayMask);
                }
                return isValidAll;
            }
            return false;
        }

        bool ShaderResourceGroupData::SetSampler(ShaderInputSamplerIndex inputIndex, const SamplerState& sampler, uint32_t arrayIndex)
        {
            return SetSamplerArray(inputIndex, std::span<const SamplerState>(&sampler, 1), arrayIndex);
        }

        bool ShaderResourceGroupData::SetSamplerArray(ShaderInputSamplerIndex inputIndex, std::span<const SamplerState> samplers, uint32_t arrayIndex)
        {
            if (GetLayout()->ValidateAccess(inputIndex, static_cast<uint32_t>(arrayIndex + samplers.size() - 1)))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                for (size_t i = 0; i < samplers.size(); ++i)
                {
                    m_samplers[interval.m_min + arrayIndex + i] = samplers[i];
                }

                if (!samplers.empty())
                {
                    EnableResourceTypeCompilation(ResourceTypeMask::SamplerMask);
                }
                return true;
            }
            return false;
        }

        void ShaderResourceGroupData::SetConstantPointer(uint8_t* bytes)
        {
            m_rootConstants = bytes;
        }

        uint8_t* ShaderResourceGroupData::GetConstantDataPointer() const
        {
            return m_rootConstants;
        }

        const RHI::ConstPtr<RHI::ImageView>& ShaderResourceGroupData::GetImageView(RHI::ShaderInputImageIndex inputIndex, uint32_t arrayIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex, arrayIndex))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                return m_imageViews[interval.m_min + arrayIndex];
            }
            return nullptr;
        }

        std::span<const RHI::ConstPtr<RHI::ImageView>> ShaderResourceGroupData::GetImageViewArray(RHI::ShaderInputImageIndex inputIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex, 0))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                return std::span<const RHI::ConstPtr<RHI::ImageView>>(&m_imageViews[interval.m_min], interval.m_max - interval.m_min);
            }
            return {};
        }

        std::span<const RHI::ConstPtr<RHI::ImageView>> ShaderResourceGroupData::GetImageViewUnboundedArray(RHI::ShaderInputImageUnboundedArrayIndex inputIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex))
            {
                return std::span<const RHI::ConstPtr<RHI::ImageView>>(m_imageViewsUnboundedArray.data(), m_imageViewsUnboundedArray.size());
            }
            return {};
        }

        const RHI::ConstPtr<RHI::BufferView>& ShaderResourceGroupData::GetBufferView(RHI::ShaderInputBufferIndex inputIndex, uint32_t arrayIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex, arrayIndex))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                return m_bufferViews[interval.m_min + arrayIndex];
            }
            return nullptr;
        }

        std::span<const RHI::ConstPtr<RHI::BufferView>> ShaderResourceGroupData::GetBufferViewArray(RHI::ShaderInputBufferIndex inputIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex, 0))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                return std::span<const RHI::ConstPtr<RHI::BufferView>>(&m_bufferViews[interval.m_min], interval.m_max - interval.m_min);
            }
            return {};
        }

        std::span<const RHI::ConstPtr<RHI::BufferView>> ShaderResourceGroupData::GetBufferViewUnboundedArray(RHI::ShaderInputBufferUnboundedArrayIndex inputIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex))
            {
                return std::span<const RHI::ConstPtr<RHI::BufferView>>(m_bufferViewsUnboundedArray.data(), m_bufferViewsUnboundedArray.size());
            }
            return {};
        }

        const RHI::SamplerState ShaderResourceGroupData::GetSampler(RHI::ShaderInputSamplerIndex inputIndex, uint32_t arrayIndex) const
        {
            if (GetLayout()->ValidateAccess(inputIndex, arrayIndex))
            {
                const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
                return m_samplers[interval.m_min + arrayIndex];
            }
            return {};
        }

        std::span<const RHI::SamplerState> ShaderResourceGroupData::GetSamplerArray(RHI::ShaderInputSamplerIndex inputIndex) const
        {
            const Interval interval = GetLayout()->GetGroupInterval(inputIndex);
            return std::span<const RHI::SamplerState>(&m_samplers[interval.m_min], interval.m_max - interval.m_min);
        }

        void ShaderResourceGroupData::ResetViews()
        {
            m_imageViews.assign(m_imageViews.size(), nullptr);
            m_bufferViews.assign(m_bufferViews.size(), nullptr);
            m_imageViewsUnboundedArray.assign(m_imageViewsUnboundedArray.size(), nullptr);
            m_bufferViewsUnboundedArray.assign(m_bufferViewsUnboundedArray.size(), nullptr);
        }

        void ShaderResourceGroupData::SetBindlessViews(
            ShaderInputBufferIndex indirectResourceBufferIndex,
            const RHI::BufferView* indirectResourceBuffer,
            std::span<const ImageView* const> imageViews,
            uint32_t* outIndices,
            std::span<bool> isViewReadOnly,
            uint32_t arrayIndex)
        {
            BufferPoolDescriptor desc = static_cast<const BufferPool*>(indirectResourceBuffer->GetBuffer().GetPool())->GetDescriptor();
            assert(desc.m_heapMemoryLevel == HeapMemoryLevel::Device, "Indirect buffer that contains indices to the bindless resource views should be device as that is protected against triple buffering.");

            auto key = std::make_pair(indirectResourceBufferIndex, arrayIndex);
            auto it = m_bindlessResourceViews.find(key);
            if (it == m_bindlessResourceViews.end())
            {
                it = m_bindlessResourceViews.try_emplace(key).first;
            }
            else
            {
                // Release existing views
                it->second.m_bindlessResources.clear();
            }

            assert(imageViews.size() == isViewReadOnly.size(), "Mismatch sizes. For each view we need to know if it is read only or readwrite");
            size_t i = 0;
            for (const ImageView* imageView : imageViews)
            {
                it->second.m_bindlessResources.push_back(imageView);
                BindlessResourceType resourceType = BindlessResourceType::Texture2D;
                //Update the indirect buffer with view indices
                if (isViewReadOnly[i])
                {
                    outIndices[i] = imageView->GetBindlessReadIndex();
                }
                else
                {
                    resourceType = BindlessResourceType::RWTexture2D;
                    outIndices[i] = imageView->GetBindlessReadWriteIndex();
                }
                it->second.m_bindlessResourceType = resourceType;
                ++i;
            }
            SetBufferView(indirectResourceBufferIndex, indirectResourceBuffer);
        }

        void ShaderResourceGroupData::SetBindlessViews(
            ShaderInputBufferIndex indirectResourceBufferIndex,
            const RHI::BufferView* indirectResourceBuffer,
            std::span<const BufferView* const> bufferViews,
            uint32_t* outIndices,
            std::span<bool> isViewReadOnly,
            uint32_t arrayIndex)
        {
            BufferPoolDescriptor desc = static_cast<const BufferPool*>(indirectResourceBuffer->GetBuffer().GetPool())->GetDescriptor();
            assert(desc.m_heapMemoryLevel == HeapMemoryLevel::Device, "Indirect buffer that contains indices to the bindless resource views should be device as that is protected against triple buffering.");

            auto key = std::make_pair(indirectResourceBufferIndex, arrayIndex);
            auto it = m_bindlessResourceViews.find(key);
            if (it == m_bindlessResourceViews.end())
            {
                it = m_bindlessResourceViews.try_emplace(key).first;
            }
            else
            {
                // Release existing views
                it->second.m_bindlessResources.clear();
            }

            assert(bufferViews.size() == isViewReadOnly.size(), "Mismatch sizes. For each view we need to know if it is read only or readwrite");

            size_t i = 0;
            for (const BufferView* bufferView : bufferViews)
            {
                it->second.m_bindlessResources.push_back(bufferView);
                BindlessResourceType resourceType = BindlessResourceType::ByteAddressBuffer;
                //Update the indirect buffer with view indices
                if (isViewReadOnly[i])
                {
                    outIndices[i] = bufferView->GetBindlessReadIndex();
                }
                else
                {
                    resourceType = BindlessResourceType::RWByteAddressBuffer;
                    outIndices[i] = bufferView->GetBindlessReadWriteIndex();
                }
                it->second.m_bindlessResourceType = resourceType;
                ++i;
            }

            SetBufferView(indirectResourceBufferIndex, indirectResourceBuffer);
        }

        const uint32_t ShaderResourceGroupData::GetBindlessViewsSize() const
        {
            return static_cast<uint32_t>(m_bindlessResourceViews.size());
        }

        const ShaderResourceGroupLayout* ShaderResourceGroupData::GetLayout() const
        {
            return m_shaderResourceGroupLayout.get();
        }

        bool ShaderResourceGroupData::ValidateSetImageView(ShaderInputImageIndex inputIndex, const ImageView* imageView, uint32_t arrayIndex) const
        {
            if (!GetLayout()->ValidateAccess(inputIndex, arrayIndex))
            {
                return false;
            }
            if (imageView)
            {
                if (!ValidateImageViewAccess<ShaderInputImageIndex, ShaderInputImageDescriptor>(inputIndex, imageView, arrayIndex))
                {
                    return false;
                }
            }
            return true;
        }

        bool ShaderResourceGroupData::ValidateSetBufferView(ShaderInputBufferIndex inputIndex, const BufferView* bufferView, uint32_t arrayIndex) const
        {
            if (!GetLayout()->ValidateAccess(inputIndex, arrayIndex))
            {
                return false;
            }
            if (bufferView)
            {
                if (!ValidateBufferViewAccess<ShaderInputBufferIndex, ShaderInputBufferDescriptor>(inputIndex, bufferView, arrayIndex))
                {
                    return false;
                }
            }
            return true;
        }

        template<typename TShaderInput, typename TShaderInputDescriptor>
        bool ShaderResourceGroupData::ValidateImageViewAccess(TShaderInput inputIndex, const ImageView* imageView, uint32_t arrayIndex) const
        {
            const TShaderInputDescriptor& shaderInputImage = GetLayout()->GetShaderInput(inputIndex);
            
            if (!imageView)
            {
                assert(false, "Image view is null");
                return false;
            }

            const ImageViewDescriptor& imageViewDescriptor = imageView->GetDescriptor();
            const Image& image = imageView->GetImage();
            const ImageDescriptor& imageDescriptor = image.GetDescriptor();

            // The image must have the correct bind flags for the slot.
            const bool isValidAccess =
                (shaderInputImage.m_access == ShaderInputImageAccess::Read && CheckBitsAll(imageDescriptor.m_bindFlags, ImageBindFlags::ShaderRead)) ||
                (shaderInputImage.m_access == ShaderInputImageAccess::ReadWrite && CheckBitsAll(imageDescriptor.m_bindFlags, ImageBindFlags::ShaderReadWrite));

            if (!isValidAccess)
            {
                assert(false, "Image view and slots access flags dont match");
                return false;
            }

            auto checkImageType = [&imageDescriptor, &shaderInputImage, arrayIndex](ImageDimension expected)
            {
                if (imageDescriptor.m_dimension != expected)
                {
                    assert(false, "The image view and slots dimentions dont match.");
                    return false;
                }
                return true;
            };

            switch (shaderInputImage.m_type)
            {
            case ShaderInputImageType::Unknown:
                // Unable to validate.
                break;

            case ShaderInputImageType::Image1DArray:
            case ShaderInputImageType::Image1D:
                if (!checkImageType(ImageDimension::Image1D))
                {
                    return false;
                }
                break;

            case ShaderInputImageType::Image2DArray:
            case ShaderInputImageType::Image2D:
                if (!checkImageType(ImageDimension::Image2D))
                {
                    return false;
                }
                break;

            case ShaderInputImageType::Image2DMultisample:
            case ShaderInputImageType::Image2DMultisampleArray:
                if (!checkImageType(ImageDimension::Image2D))
                {
                    return false;
                }
                break;

            case ShaderInputImageType::Image3D:
                if (!checkImageType(ImageDimension::Image3D))
                {
                    return false;
                }
                break;

            case ShaderInputImageType::ImageCube:
            case ShaderInputImageType::ImageCubeArray:
                if (!checkImageType(ImageDimension::Image2D))
                {
                    return false;
                }
                break;

            default:
                assert(false, "Invalid image type");
                return false;
            }

            return true;
        }

        template<typename TShaderInput, typename TShaderInputDescriptor>
        bool ShaderResourceGroupData::ValidateBufferViewAccess(TShaderInput inputIndex, const BufferView* bufferView, uint32_t arrayIndex) const
        {
            const TShaderInputDescriptor& shaderInputBuffer = GetLayout()->GetShaderInput(inputIndex);
            const BufferViewDescriptor& bufferViewDescriptor = bufferView->GetDescriptor();
            const Buffer& buffer = bufferView->GetBuffer();
            const BufferDescriptor& bufferDescriptor = buffer.GetDescriptor();

            const bool isValidAccess =
                (shaderInputBuffer.m_access == ShaderInputBufferAccess::Constant && CheckBitsAll(bufferDescriptor.m_bindFlags, BufferBindFlags::Constant)) ||
                (shaderInputBuffer.m_access == ShaderInputBufferAccess::Read && CheckBitsAll(bufferDescriptor.m_bindFlags, BufferBindFlags::ShaderRead)) ||
                (shaderInputBuffer.m_access == ShaderInputBufferAccess::ReadWrite && CheckBitsAll(bufferDescriptor.m_bindFlags, BufferBindFlags::ShaderReadWrite));

            if (!isValidAccess)
            {
                assert(false, "Buffer view and slots access flags dont match");
                return false;
            }

            if (shaderInputBuffer.m_type == ShaderInputBufferType::Constant)
            {
                // For Constant type the stride (full constant buffer) must be larger or equal than the buffer view size (element size x element count).
                if (!(shaderInputBuffer.m_strideSize >= (bufferViewDescriptor.m_elementSize * bufferViewDescriptor.m_elementCount)))
                {
                    assert(false, "The slots size stride must be larger than or equal to the buffers size");
                    return false;
                }
            }
            else
            {
                // For any other type the buffer view's element size should match the stride.
                if (shaderInputBuffer.m_strideSize != bufferViewDescriptor.m_elementSize)
                {
                    assert(false, "The slots size stride must be equal to the buffers size");
                    return false;
                }
            }

            bool isValidType = true;
            switch (shaderInputBuffer.m_type)
            {
            case ShaderInputBufferType::Unknown:
                // Unable to validate.
                break;

            case ShaderInputBufferType::Constant:
                // Element format is not relevant for viewing a buffer as a constant buffer, any format would be valid.
                break;

            case ShaderInputBufferType::Structured:
                isValidType &= bufferViewDescriptor.m_elementFormat == Format::Unknown;
                break;

            case ShaderInputBufferType::Typed:
                isValidType &= bufferViewDescriptor.m_elementFormat != Format::Unknown;
                break;

            case ShaderInputBufferType::Raw:
                isValidType &= bufferViewDescriptor.m_elementFormat == Format::R32_UINT;
                break;

            case ShaderInputBufferType::AccelerationStructure:
                isValidType &= bufferViewDescriptor.m_elementFormat == Format::R32_UINT;
                break;

            default:
                assert(false, "Invalid image type");
                return false;
            }

            if (!isValidType)
            {
                assert(false, "The buffer type and slot dont match");
                return false;
            }

            return true;
        }

        uint32_t ShaderResourceGroupData::GetUpdateMask() const
        {
            return m_updateMask;
        }

        void ShaderResourceGroupData::ResetUpdateMask()
        {
            m_updateMask = 0;
        }

        void ShaderResourceGroupData::EnableResourceTypeCompilation(ResourceTypeMask resourceTypeMask)
        {
            m_updateMask = RHI::SetBits(m_updateMask, static_cast<uint32_t>(resourceTypeMask));
        }
	}
}