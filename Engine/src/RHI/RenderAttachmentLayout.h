#pragma once

// RHI
#include "RHI_Common.h"
#include "Limits.h"
#include "AttachmentLoadStoreAction.h"
#include "ImageEnums.h"
#include "Format.h"

namespace CGE
{
	namespace RHI
	{
        // Used to configure VKRenderPass in Vulkan also in DX12 check DX_PipelineState::InitInternal.

        static const uint32_t InvalidRenderAttachmentIndex = Limits::Pipeline::RenderAttachmentCountMax;

        // Describes one render attachment that is part of a layout.
        struct RenderAttachmentDescriptor
        {
            bool IsValid() const;
            bool operator==(const RenderAttachmentDescriptor& other) const;
            bool operator!=(const RenderAttachmentDescriptor& other) const;

            // Position of the attachment in the layout.
            uint32_t m_attachmentIndex = InvalidRenderAttachmentIndex;
            // Position of the resolve attachment in layout (if it resolves).
            uint32_t m_resolveAttachmentIndex = InvalidRenderAttachmentIndex;
            // Load and store action of the attachment.
            AttachmentLoadStoreAction m_loadStoreAction;
        };

        // Describes a subpass input attachment.
        struct SubpassInputDescriptor
        {
            bool operator==(const SubpassInputDescriptor& other) const;
            bool operator!=(const SubpassInputDescriptor& other) const;

            // Attachment index that this subpass input references.
            uint32_t m_attachmentIndex = 0;
            // Aspects that are used by the input (needed by some implementations, like Vulkan, when creating a renderpass with a subpass input)
            RHI::ImageAspectFlags m_aspectFlags = RHI::ImageAspectFlags::None;
        };

        // Describes the attachments of one subpass as part of a render target layout.
        // It include descriptions about the render targets, subpass inputs and depth/stencil attachment.
        struct SubpassRenderAttachmentLayout
        {
            bool operator==(const SubpassRenderAttachmentLayout& other) const;
            bool operator!=(const SubpassRenderAttachmentLayout& other) const;

            // Number of render targets in the subpass.
            uint32_t m_rendertargetCount = 0;
            // Number of subpass input attachments in the subpass.
            uint32_t m_subpassInputCount = 0;
            // List of render targets used by the subpass.
            std::array<RenderAttachmentDescriptor, Limits::Pipeline::AttachmentColorCountMax> m_rendertargetDescriptors;
            // List of subpass inputs used by the subpass.
            std::array<SubpassInputDescriptor, Limits::Pipeline::AttachmentColorCountMax> m_subpassInputDescriptors = { {} };
            // Descriptor of the depth/stencil attachment. If not used, the attachment index is InvalidRenderAttachmentIndex.
            RenderAttachmentDescriptor m_depthStencilDescriptor;
            // Descriptor of the shading rate attachment. If not used, the attachment index is InvalidRenderAttachmentIndex.
            RenderAttachmentDescriptor m_shadingRateDescriptor;
        };

        // A RenderAttachmentLayout consist of a description of one or more subpasses.
        // Each subpass is a collection of render targets, subpass inputs and depth stencil attachments.
        // Each subpass corresponds to a phase in the rendering of a Pipeline.
        // Subpass outputs can be read by other subpasses as inputs.
        // A RenderAttachmentLayout may be implemented by the platform using native functionality, achieving a
        // performance gain for that specific platform. On other platforms, it may be emulated to achieve the same result
        // but without the performance benefits. For example, Vulkan supports the concept of subpass natively.
        class RenderAttachmentLayout
        {
        public:
            HashValue64 GetHash() const;
            bool operator==(const RenderAttachmentLayout& other) const;

            // Number of total attachments in the list.
            uint32_t m_attachmentCount = 0;
            // List with all attachments (renderAttachments, resolveAttachments and depth/stencil attachment).
            std::array<RHI::Format, Limits::Pipeline::RenderAttachmentCountMax> m_attachmentFormats = { {} };
            // Number of subpasses.
            uint32_t m_subpassCount = 0;
            // List with the layout of each subpass.
            std::array<SubpassRenderAttachmentLayout, Limits::Pipeline::SubpassCountMax> m_subpassLayouts;
        };

        // Describes the layout of a collection of subpasses and it defines which of the subpasses this
        // configuration will be using.
        struct RenderAttachmentConfiguration
        {
            HashValue64 GetHash() const;
            bool operator==(const RenderAttachmentConfiguration& other) const;

            // Returns the format of a render target in the subpass being used.
            Format GetRenderTargetFormat(uint32_t index) const;
            // Returns the format of a subpass input in the subpass being used.
            Format GetSubpassInputFormat(uint32_t index) const;
            // Returns the format of resolve attachment in the subpass being used.
            Format GetRenderTargetResolveFormat(uint32_t index) const;
            // Returns the format of a depth/stencil in the subpass being used.
            // Will return Format::Unkwon if the depth/stencil is not being used.
            Format GetDepthStencilFormat() const;
            // Returns the number of render targets in the subpass being used.
            uint32_t GetRenderTargetCount() const;
            // Returns the number of subpass inputs in the subpass being used.
            uint32_t GetSubpassInputCount() const;
            // Returns true if the render target is resolving in the subpass being used.
            bool DoesRenderTargetResolve(uint32_t index) const;

            // Layout of the render target attachments.
            RenderAttachmentLayout m_renderAttachmentLayout;
            // Index of the subpass being used.
            uint32_t m_subpassIndex = 0;
        };
	}
}