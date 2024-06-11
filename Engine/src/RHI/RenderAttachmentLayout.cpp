

// RHI
#include "RenderAttachmentLayout.h"

namespace CGE
{
	namespace RHI
	{
        bool RenderAttachmentDescriptor::IsValid() const
        {
            return m_attachmentIndex != InvalidRenderAttachmentIndex;
        }

        bool RenderAttachmentDescriptor::operator==(const RenderAttachmentDescriptor& other) const
        {
            return (m_attachmentIndex == other.m_attachmentIndex) && (m_resolveAttachmentIndex == other.m_resolveAttachmentIndex) && (m_loadStoreAction == other.m_loadStoreAction);
        }

        bool RenderAttachmentDescriptor::operator!=(const RenderAttachmentDescriptor& other) const
        {
            return !(*this == other);
        }

        bool SubpassInputDescriptor::operator==(const SubpassInputDescriptor& other) const
        {
            return (m_attachmentIndex == other.m_attachmentIndex) && (m_aspectFlags == other.m_aspectFlags);
        }

        bool SubpassInputDescriptor::operator!=(const SubpassInputDescriptor& other) const
        {
            return !(*this == other);
        }

        bool SubpassRenderAttachmentLayout::operator==(const SubpassRenderAttachmentLayout& other) const
        {
            if ((m_rendertargetCount != other.m_rendertargetCount) || (m_subpassInputCount != other.m_subpassInputCount) || (m_depthStencilDescriptor != other.m_depthStencilDescriptor))
            {
                return false;
            }

            for (uint32_t i = 0; i < m_rendertargetCount; ++i)
            {
                if (m_rendertargetDescriptors[i] != other.m_rendertargetDescriptors[i])
                {
                    return false;
                }
            }

            for (uint32_t i = 0; i < m_subpassInputCount; ++i)
            {
                if (m_subpassInputDescriptors[i] != other.m_subpassInputDescriptors[i])
                {
                    return false;
                }
            }

            return true;
        }

        bool SubpassRenderAttachmentLayout::operator!=(const SubpassRenderAttachmentLayout& other) const
        {
            return !(*this == other);
        }

        HashValue64 RenderAttachmentLayout::GetHash() const
        {
            return TypeHash64(*this);
        }

        bool RenderAttachmentLayout::operator==(const RenderAttachmentLayout& other) const
        {
            if ((m_attachmentCount != other.m_attachmentCount) || (m_subpassCount != other.m_subpassCount))
            {
                return false;
            }

            for (uint32_t i = 0; i < m_attachmentCount; ++i)
            {
                if (m_attachmentFormats[i] != other.m_attachmentFormats[i])
                {
                    return false;
                }
            }

            for (uint32_t i = 0; i < m_subpassCount; ++i)
            {
                if (m_subpassLayouts[i] != other.m_subpassLayouts[i])
                {
                    return false;
                }
            }

            return true;
        }

        HashValue64 RenderAttachmentConfiguration::GetHash() const
        {
            HashValue64 hash = m_renderAttachmentLayout.GetHash();
            hash = TypeHash64(m_subpassIndex, hash);
            return hash;
        }

        bool RenderAttachmentConfiguration::operator==(const RenderAttachmentConfiguration& other) const
        {
            return (m_renderAttachmentLayout == other.m_renderAttachmentLayout) && (m_subpassIndex == other.m_subpassIndex);
        }

        Format RenderAttachmentConfiguration::GetRenderTargetFormat(uint32_t index) const
        {
            const auto& subpassAttachmentLayout = m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex];
            return m_renderAttachmentLayout.m_attachmentFormats[subpassAttachmentLayout.m_rendertargetDescriptors[index].m_attachmentIndex];
        }

        Format RenderAttachmentConfiguration::GetSubpassInputFormat(uint32_t index) const
        {
            const auto& subpassAttachmentLayout = m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex];
            return m_renderAttachmentLayout.m_attachmentFormats[subpassAttachmentLayout.m_subpassInputDescriptors[index].m_attachmentIndex];
        }

        Format RenderAttachmentConfiguration::GetRenderTargetResolveFormat(uint32_t index) const
        {
            const auto& subpassAttachmentLayout = m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex];
            if (subpassAttachmentLayout.m_rendertargetDescriptors[index].m_resolveAttachmentIndex != InvalidRenderAttachmentIndex)
            {
                return m_renderAttachmentLayout.m_attachmentFormats[subpassAttachmentLayout.m_rendertargetDescriptors[index].m_resolveAttachmentIndex];
            }
            return Format::Unknown;
        }

        Format RenderAttachmentConfiguration::GetDepthStencilFormat() const
        {
            const auto& subpassAttachmentLayout = m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex];
            return subpassAttachmentLayout.m_depthStencilDescriptor.IsValid() ?
                m_renderAttachmentLayout.m_attachmentFormats[subpassAttachmentLayout.m_depthStencilDescriptor.m_attachmentIndex] : RHI::Format::Unknown;
        }

        uint32_t RenderAttachmentConfiguration::GetRenderTargetCount() const
        {
            return m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex].m_rendertargetCount;
        }

        uint32_t RenderAttachmentConfiguration::GetSubpassInputCount() const
        {
            return m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex].m_subpassInputCount;
        }

        bool RenderAttachmentConfiguration::DoesRenderTargetResolve(uint32_t index) const
        {
            return m_renderAttachmentLayout.m_subpassLayouts[m_subpassIndex].m_rendertargetDescriptors[index].m_resolveAttachmentIndex != InvalidRenderAttachmentIndex;
        }
	}
}