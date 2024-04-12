
// DX12
#include "DX_ImageView.h"
#include "DX_Device.h"
#include "DX_Image.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
        RHI::Ptr<DX_ImageView> DX_ImageView::Create()
        {
            return new DX_ImageView();
        }

        const DX_Image& DX_ImageView::GetImage() const
        {
            return static_cast<const DX_Image&>(Base::GetImage());
        }

        DXGI_FORMAT DX_ImageView::GetFormat() const
        {
            return m_format;
        }

        ID3D12Resource* DX_ImageView::GetMemory() const
        {
            return m_memory;
        }

        DX_DescriptorHandle DX_ImageView::GetReadDescriptor() const
        {
            return m_readDescriptor;
        }

        DX_DescriptorHandle DX_ImageView::GetReadWriteDescriptor() const
        {
            return m_readWriteDescriptor;
        }

        DX_DescriptorHandle DX_ImageView::GetClearDescriptor() const
        {
            return m_clearDescriptor;
        }

        DX_DescriptorHandle DX_ImageView::GetColorDescriptor() const
        {
            return m_colorDescriptor;
        }

        DX_DescriptorHandle DX_ImageView::GetDepthStencilDescriptor(RHI::ScopeAttachmentAccess access) const
        {
            return m_depthStencilDescriptor;
        }

        RHI::ResultCode DX_ImageView::InitInternal(RHI::Device& device, const RHI::Resource& resourceBase)
        {
            const DX_Image& image = static_cast<const DX_Image&>(resourceBase);

            RHI::ImageViewDescriptor viewDescriptor = GetDescriptor();
            viewDescriptor.m_mipSliceMin = std::max(viewDescriptor.m_mipSliceMin, static_cast<uint16_t>(image.GetStreamedMipLevel()));
            viewDescriptor.m_mipSliceMax = std::min(viewDescriptor.m_mipSliceMax, static_cast<uint16_t>(image.GetDescriptor().m_mipLevels - 1));

            // By default, if no bind flags are specified on the view descriptor, attempt to create all views that are compatible with the underlying image's bind flags
            // If bind flags are specified on the view descriptor, only create the views for the specified bind flags.
            bool hasOverrideBindFlags = viewDescriptor.m_overrideBindFlags != RHI::ImageBindFlags::None;
            const RHI::ImageBindFlags bindFlags = hasOverrideBindFlags ? viewDescriptor.m_overrideBindFlags : image.GetDescriptor().m_bindFlags;

            m_format = ConvertImageViewFormat(image, viewDescriptor);
            m_memory = image.GetMemoryView().GetMemory();

            DX_DescriptorContext& context = static_cast<DX_Device&>(device).GetDescriptorContext();

            if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderRead)))
            {
                context.CreateShaderResourceView(image, viewDescriptor, m_readDescriptor, m_staticReadDescriptor);
            }

            if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::ShaderWrite)))
            {
                context.CreateUnorderedAccessView(image, viewDescriptor, m_readWriteDescriptor, m_clearDescriptor, m_staticReadWriteDescriptor);
            }

            if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::Color)))
            {
                context.CreateRenderTargetView(image, viewDescriptor, m_colorDescriptor);
            }

            if (RHI::CheckBitsAny(static_cast<uint32_t>(bindFlags), static_cast<uint32_t>(RHI::ImageBindFlags::DepthStencil)))
            {
                context.CreateDepthStencilView(image, viewDescriptor, m_depthStencilDescriptor, m_depthStencilReadDescriptor);
            }

            m_hash = RHI::TypeHash64(viewDescriptor.GetHash(), m_hash);
            m_hash = RHI::TypeHash64(m_format, m_hash);

            return RHI::ResultCode::Success;
        }

        RHI::ResultCode DX_ImageView::InvalidateInternal()
        {
            return InitInternal(GetDevice(), GetResource());
        }

        void DX_ImageView::ShutdownInternal()
        {
            DX_Device& device = static_cast<DX_Device&>(GetDevice());

            DX_DescriptorContext& context = device.GetDescriptorContext();
            context.ReleaseDescriptor(m_readDescriptor);
            context.ReleaseDescriptor(m_readWriteDescriptor);
            context.ReleaseDescriptor(m_clearDescriptor);
            context.ReleaseDescriptor(m_colorDescriptor);
            context.ReleaseDescriptor(m_depthStencilDescriptor);
            context.ReleaseDescriptor(m_depthStencilReadDescriptor);
            context.ReleaseStaticDescriptor(m_staticReadDescriptor);
            context.ReleaseStaticDescriptor(m_staticReadWriteDescriptor);
            m_memory = nullptr;
        }

        uint32_t DX_ImageView::GetBindlessReadIndex() const
        {
            return m_staticReadDescriptor.m_index;
        }

        uint32_t DX_ImageView::GetBindlessReadWriteIndex() const
        {
            return m_staticReadWriteDescriptor.m_index;
        }
	}
}