#pragma once

// RHI
#include "ShaderResourceGroupLayout.h"
#include "ImageView.h"
#include "BufferView.h"
#include "TypeHash.h"

namespace CGE
{
	namespace RHI
	{
        enum class BindlessResourceType : uint32_t
        {
            Texture2D = 0,
            RWTexture2D,
            TextureCube,
            ByteAddressBuffer,
            RWByteAddressBuffer,
            Count
        };

        // This will be the data views used to compile and set on the command list.
        // The views must mach the ShaderResourceGroupLayout used for the srg.
        // It will call validate so it knows the user is setting the correct views to the corresponding input index and array index.
        // 
        //              index in the ShaderResourceGroupLayout
        //         0                 1                    2
        // |--------------------------------------------------------|
        // |  3 Tex2D      |    4 Tex3D       |     4 RWTex2D       |
        // |--------------------------------------------------------|
        // 
        //
        //              The validation check happens
        //
        //         0                    1                              2          ==> Input idx
        // |--------------------------------------------------------------------|
        // |  0  |  1  |  2  ||  0  |  1  |  2  |  3  ||  0  |  1  |  2  |  3   | ==> Array idx
        // |--------------------------------------------------------------------|
        //
        //                   ||  0  |  1  |  2  |  3  |  4  ||  ==> Set five Tex3D image views at input index 1 and array offet 0 (This wont work)
        // 
        //                               ||  0  |  1  || ==> Set five Tex3D image views at input index 1 and array offet 2 (This will work)
        class ShaderResourceGroupData
        {
        public:
            enum class ResourceType : uint32_t
            {
                ConstantBufferView,
                BufferView,
                ImageView,
                BufferViewUnboundedArray,
                ImageViewUnboundedArray,
                Sampler,
                Count
            };

            enum class ResourceTypeMask : uint32_t
            {
                None = 0,
                BufferViewMask = 1u << (static_cast<uint32_t>(ResourceType::BufferView)),
                ImageViewMask = 1u << (static_cast<uint32_t>(ResourceType::ImageView)),
                BufferViewUnboundedArrayMask = 1u << (static_cast<uint32_t>(ResourceType::BufferViewUnboundedArray)),
                ImageViewUnboundedArrayMask = 1u << (static_cast<uint32_t>(ResourceType::ImageViewUnboundedArray)),
                SamplerMask = 1u << (static_cast<uint32_t>(ResourceType::Sampler))
            };

            // Holds all the views for a specific type.
            struct BindlessResourceViews
            {
                BindlessResourceType m_bindlessResourceType = BindlessResourceType::Count;
                std::vector<ConstPtr<ResourceView>> m_bindlessResources;
            };

        public:
            ShaderResourceGroupData() = default;
            ~ShaderResourceGroupData() = default;

            explicit ShaderResourceGroupData(const ShaderResourceGroupLayout* shaderResourceGroupLayout);

            bool SetImageView(ShaderInputImageIndex inputIndex, const ImageView* imageView, uint32_t arrayIndex);
            bool SetImageViewArray(ShaderInputImageIndex inputIndex, std::span<const ImageView* const> imageViews, uint32_t arrayIndex = 0);
            bool SetImageViewUnboundedArray(ShaderInputImageUnboundedArrayIndex inputIndex, std::span<const ImageView* const> imageViews);

            bool SetBufferView(ShaderInputBufferIndex inputIndex, const BufferView* bufferView, uint32_t arrayIndex = 0);
            bool SetBufferViewArray(ShaderInputBufferIndex inputIndex, std::span<const BufferView* const> bufferViews, uint32_t arrayIndex = 0);
            bool SetBufferViewUnboundedArray(ShaderInputBufferUnboundedArrayIndex inputIndex, std::span<const BufferView* const> bufferViews);

            bool SetSampler(ShaderInputSamplerIndex inputIndex, const SamplerState& sampler, uint32_t arrayIndex = 0);
            bool SetSamplerArray(ShaderInputSamplerIndex inputIndex, std::span<const SamplerState> samplers, uint32_t arrayIndex = 0);

            // [todo] Have to change to properly manage the constant data pointer and do a mask update.
            // Currently we have to set this and make sure the struct matches the layout we set in ShaderResourceGroupLayout::m_inputForConstant.
            // Also I need to keep track of when this data changes to do another memcopy (Very important I set this once for now).
            void SetConstantPointer(uint8_t* bytes);
            uint8_t* GetConstantDataPointer() const;

            // [todo] constant buffer views

            const ConstPtr<ImageView>& GetImageView(ShaderInputImageIndex inputIndex, uint32_t arrayIndex) const;
            std::span<const ConstPtr<ImageView>> GetImageViewArray(ShaderInputImageIndex inputIndex) const;
            std::span<const ConstPtr<ImageView>> GetImageViewUnboundedArray(ShaderInputImageUnboundedArrayIndex inputIndex) const;

            const ConstPtr<BufferView>& GetBufferView(ShaderInputBufferIndex inputIndex, uint32_t arrayIndex) const;
            std::span<const ConstPtr<BufferView>> GetBufferViewArray(ShaderInputBufferIndex inputIndex) const;
            std::span<const ConstPtr<BufferView>> GetBufferViewUnboundedArray(ShaderInputBufferUnboundedArrayIndex inputIndex) const;

            const SamplerState GetSampler(ShaderInputSamplerIndex inputIndex, uint32_t arrayIndex) const;
            std::span<const SamplerState> GetSamplerArray(ShaderInputSamplerIndex inputIndex) const;

            // Update the indirect buffer view with the indices of all the image views which reside in the global gpu heap.
            // Ideally higher level code can access bindless heap indices directly from the view and populate any indirect buffer directly.
            void SetBindlessViews(
                ShaderInputBufferIndex indirectResourceBufferIndex,
                const RHI::BufferView* indirectResourceBuffer,
                std::span<const ImageView* const> imageViews,
                uint32_t* outIndices,
                std::span<bool> isViewReadOnly,
                uint32_t arrayIndex = 0);
            void SetBindlessViews(
                ShaderInputBufferIndex indirectResourceBufferIndex,
                const RHI::BufferView* indirectResourceBuffer,
                std::span<const BufferView* const> bufferViews,
                uint32_t* outIndices,
                std::span<bool> isViewReadOnly,
                uint32_t arrayIndex = 0);
            const uint32_t GetBindlessViewsSize() const;

            void ResetViews();
            const ShaderResourceGroupLayout* GetLayout() const;
            void ResetUpdateMask();
            uint32_t GetUpdateMask() const;
            void EnableResourceTypeCompilation(ResourceTypeMask resourceTypeMask);

        private:
            bool ValidateSetImageView(ShaderInputImageIndex inputIndex, const ImageView* imageView, uint32_t arrayIndex) const;
            bool ValidateSetBufferView(ShaderInputBufferIndex inputIndex, const BufferView* bufferView, uint32_t arrayIndex) const;

            // [todo] FrameAttachments for RW access
            template<typename TShaderInput, typename TShaderInputDescriptor>
            bool ValidateImageViewAccess(TShaderInput inputIndex, const ImageView* imageView, uint32_t arrayIndex) const;
            template<typename TShaderInput, typename TShaderInputDescriptor>
            bool ValidateBufferViewAccess(TShaderInput inputIndex, const BufferView* bufferView, uint32_t arrayIndex) const;

        private:
            ConstPtr<ShaderResourceGroupLayout> m_shaderResourceGroupLayout;

            // The backing data store of bound resources for the shader resource group.
            // The good thing about the way I wrote the view abstractions is each resource view will cache all view types to the resource. (Read-only, RW, bindless read/RW)
            // We can pull based on the layouts needs. (Check DX_ShaderResourceGroup::CompileInternal to see whats going on)
            // These are flat so we need to get a handle to a range based on the layouts at the specific index using ShaderResourceGroupLayout::GetGroupInterval)
            std::vector<ConstPtr<ImageView>> m_imageViews;
            std::vector<ConstPtr<BufferView>> m_bufferViews;
            std::vector<SamplerState> m_samplers;
            std::vector<ConstPtr<ImageView>> m_imageViewsUnboundedArray;
            std::vector<ConstPtr<BufferView>> m_bufferViewsUnboundedArray;

            // [todo] for now I will set it directly here. This should match the size and count of the resource group layout (root parameter).
            uint8_t* m_rootConstants = nullptr;

            // We neee to keep track of which srgs we need to recompile and set the tables again for binding.
            uint32_t m_updateMask = 0;

            // [todo] Move this later
            struct pair_hash
            {
                template <class T1, class T2>
                RHI::HashValue64 operator()(const std::pair<T1, T2>& p) const 
                {
                    HashValue64 hash = HashValue64{ 0 };
                    hash = RHI::TypeHash64(p.first, hash);
                    hash = RHI::TypeHash64(p.second, hash);
                    return hash;
                }
            };
            // The key will be a pair of the [Input index, Array Index] of the indirect buffer which holds all the bindless views indicies.
            // The value will be of type BindlessResourceViews which is all the views of a specific BindlessResourceType.
            std::unordered_map<std::pair<ShaderInputBufferIndex, uint32_t>, BindlessResourceViews, pair_hash> m_bindlessResourceViews;
        };
	}
}