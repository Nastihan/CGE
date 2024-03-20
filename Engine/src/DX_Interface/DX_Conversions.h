#pragma once

// DX12
#include "DX_CommonHeaders.h"
#include "DX_Buffer.h"

// RHI
#include "../RHI/CommandQueue.h"
#include "../RHI/Format.h"
#include "../RHI/SwapChainDescriptor.h"
#include "../RHI/MemoryEnums.h"
#include "../RHI/BufferDescriptor.h"
#include "../RHI/BufferViewDescriptor.h"

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
	}
}