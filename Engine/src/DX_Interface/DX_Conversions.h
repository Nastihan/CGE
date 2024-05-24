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

namespace CGE
{
	namespace DX12
	{
		D3D12_COMMAND_LIST_TYPE ConvertHardwareQueueClass(RHI::HardwareQueueClass type);

		std::wstring s2ws(const std::string& str);

		DXGI_FORMAT ConvertFormat(RHI::Format format);

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
	}
}