#pragma once
#include "DX_CommonHeaders.h"
#include "DX_Commander.h"
#include <mutex>
#include <vector>

namespace CGE
{
	namespace DX12
	{
		class DX_DescriptorHeap
		{
		private:
			struct DescriptorHandle
			{
				D3D12_CPU_DESCRIPTOR_HANDLE cpuAddress{};
				D3D12_GPU_DESCRIPTOR_HANDLE gpuAddress{};

				constexpr bool IsValid() const { return cpuAddress.ptr != 0; }
				constexpr bool IsShaderVisible() const { return gpuAddress.ptr != 0; }

#ifdef _DEBUG
			private:
				friend class DX_DescriptorHeap;
				DX_DescriptorHeap* container{ nullptr };
				UINT32 index{ UINT32_ERROR };
#endif // _DEBUG

			};
		public:
			DX_DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT32 capacity, bool isShaderVisible);
			~DX_DescriptorHeap();
			REMOVE_COPY_AND_MOVE(DX_DescriptorHeap);
		public:
			DescriptorHandle Allocate();
			void Free(DescriptorHandle& handle);
			constexpr D3D12_CPU_DESCRIPTOR_HANDLE GetCpuBaseAddress() const;
			constexpr D3D12_GPU_DESCRIPTOR_HANDLE GetGpuBaseAddress() const;
			constexpr UINT32 GetCapacity() const;
			constexpr UINT32 GetSize() const;
			constexpr UINT32 GetDescriptorSize() const;
			constexpr bool IsShaderVisible() const;
			constexpr D3D12_DESCRIPTOR_HEAP_TYPE GetType() const;
		private:
			wrl::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
			// cpu and gpu base address
			D3D12_CPU_DESCRIPTOR_HANDLE cpuBaseAddress{};
			D3D12_GPU_DESCRIPTOR_HANDLE gpuBaseAddress{};
			UINT32 capacity{ 0 };
			UINT32 size{ 0 };
			UINT32 descriptorSize{ 0 };
			std::unique_ptr<UINT32[]> freeHandles;
			const D3D12_DESCRIPTOR_HEAP_TYPE type{};
			// loading and creating resources might happen on different threads
			std::mutex mtx{};
			std::vector<UINT32> deferredFreeIndices[DX_Commander::FRAME_BUFFER_COUNT]{};
		};
	}
}