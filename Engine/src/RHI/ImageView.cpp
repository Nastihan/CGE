
// RHI
#include "ImageView.h"
#include "Image.h"

namespace CGE
{
	namespace RHI
	{
        ResultCode ImageView::Init(const Image& image, const ImageViewDescriptor& viewDescriptor)
        {
            if (!ValidateForInit(image, viewDescriptor))
            {
                return ResultCode::InvalidOperation;
            }

            m_descriptor = viewDescriptor;
            m_hash = image.GetHash();
            m_hash = TypeHash64(m_descriptor.GetHash(), m_hash);
            return ResourceView::Init(image);
        }

        bool ImageView::ValidateForInit(const Image& image, const ImageViewDescriptor& viewDescriptor) const
        {
            return !IsInitialized() && image.IsInitialized() && CheckBitsAll(static_cast<uint32_t>(image.GetDescriptor().m_bindFlags), static_cast<uint32_t>(viewDescriptor.m_overrideBindFlags));
        }

        const ImageViewDescriptor& ImageView::GetDescriptor() const
        {
            return m_descriptor;
        }

        const Image& ImageView::GetImage() const
        {
            return static_cast<const Image&>(GetResource());
        }

        bool ImageView::IsFullView() const
        {
            const ImageDescriptor& imageDescriptor = GetImage().GetDescriptor();
            return m_descriptor.m_arraySliceMin == 0 &&
                (m_descriptor.m_arraySliceMax + 1) >= imageDescriptor.m_arraySize &&
                m_descriptor.m_mipSliceMin == 0 &&
                (m_descriptor.m_mipSliceMax + 1) >= imageDescriptor.m_mipLevels;
        }

        HashValue64 ImageView::GetHash() const
        {
            return m_hash;
        }
	}
}