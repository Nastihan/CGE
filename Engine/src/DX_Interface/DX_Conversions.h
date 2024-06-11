#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Buffer.h"
#include "DX_Image.h"

// RHI
#include "../RHI/CommandQueue.h"
#include "../RHI/Format.h"
#include "../RHI/SwapChainDescriptor.h"
#include "../RHI/MemoryEnums.h"
#include "../RHI/BufferDescriptor.h"
#include "../RHI/BufferViewDescriptor.h"
#include "../RHI/ClearValue.h"
#include "../RHI/ShaderStages.h"
#include "../RHI/SamplerState.h"
#include "../RHI/ShaderResourceGroupLayoutDescriptor.h"
#include "../RHI/InputStreamLayout.h"
#include "../RHI/RenderStates.h"

namespace CGE
{
	namespace DX12
	{
		D3D12_COMMAND_LIST_TYPE ConvertHardwareQueueClass(RHI::HardwareQueueClass type);

		std::wstring s2ws(const std::string& str);

		DXGI_FORMAT ConvertFormat(RHI::Format format);
		RHI::Format ConvertFormat(DXGI_FORMAT format);

		DXGI_SCALING ConvertScaling(RHI::Scaling scaling);

		D3D12_HEAP_TYPE ConvertHeapType(RHI::HeapMemoryLevel heapMemoryLevel, RHI::HostMemoryAccess hostMemoryAccess);

		D3D12_RESOURCE_STATES ConvertInitialResourceState(RHI::HeapMemoryLevel heapMemoryLevel, RHI::HostMemoryAccess hostMemoryAccess);

		void ConvertBufferDescriptor(const RHI::BufferDescriptor& descriptor, D3D12_RESOURCE_DESC& resourceDesc);
		D3D12_RESOURCE_FLAGS ConvertBufferBindFlags(RHI::BufferBindFlags bufferFlags);
		void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceView);
		void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessView);
		void ConvertBufferView(const DX_Buffer& buffer, const RHI::BufferViewDescriptor& bufferViewDescriptor, D3D12_CONSTANT_BUFFER_VIEW_DESC& constantBufferView);

		D3D12_CLEAR_VALUE ConvertClearValue(RHI::Format format, RHI::ClearValue clearValue);
		void ConvertImageDescriptor(const RHI::ImageDescriptor& descriptor, D3D12_RESOURCE_DESC& resourceDesc);
		DXGI_FORMAT ConvertImageViewFormat(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor);
		D3D12_RESOURCE_DIMENSION ConvertImageDimension(RHI::ImageDimension dimension);
		D3D12_RESOURCE_FLAGS ConvertImageBindFlags(RHI::ImageBindFlags bindFlags);
		void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_RENDER_TARGET_VIEW_DESC& renderTargetView);
		void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_DEPTH_STENCIL_VIEW_DESC& depthStencilView);
		void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_SHADER_RESOURCE_VIEW_DESC& shaderResourceView);
		void ConvertImageView(const DX_Image& image, const RHI::ImageViewDescriptor& imageViewDescriptor, D3D12_UNORDERED_ACCESS_VIEW_DESC& unorderedAccessView);
		uint16_t ConvertImageAspectToPlaneSlice(RHI::ImageAspect aspect);

		D3D12_SHADER_VISIBILITY ConvertShaderStageMask(RHI::ShaderStageMask mask);

		// Sampler
		D3D12_FILTER_TYPE ConvertFilterMode(RHI::FilterMode mode);
		D3D12_FILTER_REDUCTION_TYPE ConvertReductionType(RHI::ReductionType reductionType);
		D3D12_TEXTURE_ADDRESS_MODE ConvertAddressMode(RHI::AddressMode addressMode);
		void ConvertBorderColor(RHI::BorderColor color, float outputColor[4]);
		D3D12_STATIC_BORDER_COLOR ConvertBorderColor(RHI::BorderColor color);
		D3D12_COMPARISON_FUNC ConvertComparisonFunc(RHI::ComparisonFunc func);
		void ConvertSamplerState(const RHI::SamplerState& state, D3D12_SAMPLER_DESC& samplerDesc);
		void ConvertStaticSampler(const RHI::SamplerState& state, uint32_t shaderRegister, uint32_t shaderRegisterSpace, D3D12_SHADER_VISIBILITY shaderVisibility, D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc);

		D3D12_DESCRIPTOR_RANGE_TYPE ConvertShaderInputBufferAccess(RHI::ShaderInputBufferAccess access);
		D3D12_DESCRIPTOR_RANGE_TYPE ConvertShaderInputImageAccess(RHI::ShaderInputImageAccess access);

		D3D12_SRV_DIMENSION ConvertSRVDimension(RHI::ShaderInputImageType type);
		D3D12_UAV_DIMENSION ConvertUAVDimension(RHI::ShaderInputImageType type);

		D3D12_PRIMITIVE_TOPOLOGY ConvertTopology(RHI::PrimitiveTopology topology);

		std::vector<D3D12_INPUT_ELEMENT_DESC> ConvertInputElements(const RHI::InputStreamLayout& layout);
		D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertToTopologyType(RHI::PrimitiveTopology type);
		D3D12_BLEND_DESC ConvertBlendState(const RHI::BlendState& blend);
		D3D12_BLEND_OP ConvertBlendOp(RHI::BlendOp op);
		D3D12_BLEND ConvertBlendFactor(RHI::BlendFactor factor);
		uint8_t ConvertColorWriteMask(uint8_t writeMask);
		D3D12_RASTERIZER_DESC ConvertRasterState(const RHI::RasterState& raster);
		D3D12_CULL_MODE ConvertCullMode(RHI::CullMode mode);
		D3D12_FILL_MODE ConvertFillMode(RHI::FillMode mode);
		D3D12_DEPTH_STENCIL_DESC ConvertDepthStencilState(const RHI::DepthStencilState& depthStencil);
		D3D12_STENCIL_OP ConvertStencilOp(RHI::StencilOp op);
		D3D12_DEPTH_WRITE_MASK ConvertDepthWriteMask(RHI::DepthWriteMask mask);
	}
}