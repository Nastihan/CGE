#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_MemoryView.h"
#include "DX_Descriptor.h"
#include "DX_DescriptorContext.h"

// RHI
#include "../RHI/ShaderResourceGroup.h"
#include "../RHI/Limits.h"

// std
#include <array>

namespace CGE
{
	namespace DX12
	{
        // Each srg in the DX12 backend will become these root parameters:
        // 1. SRV_UAV_CBV table
        // 2. Dynamic sampler table
        // 3. Unbounded buffer table
        // 4. Unbounded image table
        // The command list will access these and set them before GPU command.
        struct DX_ShaderResourceGroupCompiledData
        {
            GpuDescriptorHandle m_gpuViewsDescriptorHandle = {};
            static const uint32_t MaxUnboundedArrays = 2;
            GpuDescriptorHandle m_gpuUnboundedArraysDescriptorHandles[MaxUnboundedArrays] = {};
            GpuDescriptorHandle m_gpuSamplersDescriptorHandle = {};

            // [todo] Use for ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView
            GpuVirtualAddress m_gpuConstantBufferAddress = {};
            CpuVirtualAddress m_cpuConstantBufferAddress = {};

            // Using for ID3D12GraphicsCommandList::SetGraphicsRoot32BitConstants
            CpuVirtualAddress m_rootConstant = {};
            uint32_t m_rootConstantSize;
        };

        class DX_ShaderResourceGroup final : public RHI::ShaderResourceGroup
        {
            using Base = RHI::ShaderResourceGroup;
        public:
            static RHI::Ptr<DX_ShaderResourceGroup> Create();
            const DX_ShaderResourceGroupCompiledData& GetCompiledData() const;

        private:
            DX_ShaderResourceGroup() = default;

            // RHI::ShaderResourceGroup
            RHI::ResultCode InitInternal(const RHI::ShaderResourceGroupData& shaderResourceGroupData) override;
            RHI::ResultCode CompileInternal() override;

            template<typename T, typename U>
            std::vector<DX_DescriptorHandle> GetSRVsFromImageViews(const std::span<const RHI::ConstPtr<T>>& imageViews, D3D12_SRV_DIMENSION dimension);
            template<typename T, typename U>
            std::vector<DX_DescriptorHandle> GetUAVsFromImageViews(const std::span<const RHI::ConstPtr<T>>& bufferViews, D3D12_UAV_DIMENSION dimension);
            std::vector<DX_DescriptorHandle> GetCBVsFromBufferViews(const std::span<const RHI::ConstPtr<RHI::BufferView>>& bufferViews);

            friend class DescriptorContext;

            // The array of compiled SRG data, N buffered for CPU updates.
            uint32_t m_compiledDataIndex = 0;
            std::array<DX_ShaderResourceGroupCompiledData, RHI::Limits::Device::FrameCountMax> m_compiledData;

            //                                          Shader Visible D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV heap
            //                         This region is m_viewsDescriptorTable (The size is (InputListForBuffers + InputListForImages) * FrameCountMax))
            // 
            // ||-------------------------------------------------------------------------------------------------------------------------------------------||
            // ||       |  Buffer     Buffer  |  RWBuffer  |  Tex2D   Tex2D   Tex2D  |  Tex3D   Tex3D  |   RWTex2D  RWTex2D   RWTex2D   RWTex2D     |       ||
            // ||  ...  |                                  |                         |                 |                                            |       ||
            // ||       |  Buffer offset                   |  Image offset           |                 |   Image offset + interval.m_min            |       ||
            // ||-------------------------------------------------------------------------------------------------------------------------------------------||
            DX_DescriptorTable m_viewsDescriptorTable;
            uint32_t m_descriptorTableBufferOffset = 0;
            uint32_t m_descriptorTableImageOffset = 0;
            DX_MemoryView m_constantMemoryView;
            DX_DescriptorTable m_samplersDescriptorTable;
            std::array<DX_DescriptorTable, DX_ShaderResourceGroupCompiledData::MaxUnboundedArrays * RHI::Limits::Device::FrameCountMax> m_unboundedDescriptorTables;

            DX_DescriptorContext* m_descriptorContext = nullptr;
        };
	}
}