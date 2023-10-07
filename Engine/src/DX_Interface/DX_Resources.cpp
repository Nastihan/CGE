#include "DX_CommonHeaders.h"
#include "DX_Resources.h"
#include "DX_Device.h"

namespace CGE
{
	namespace DX12
	{
		DX_DescriptorHeap::DX_DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT32 capacity, bool isShaderVisible) : type{ type }, capacity{ capacity },
			freeHandles{ std::move(std::make_unique<UINT32[]>(capacity)) }, size{ 0 }
		{
			//std::lock_guard lock{ mtx };

			LOCAL_HR;
			const auto& device = DX_Device::GetInstance().GetDevice();

			assert(capacity&& capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
			assert(!(type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));
			if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV || type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
			{
				isShaderVisible = false;
			}

			//Release

			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NumDescriptors = capacity;
			desc.Type = type;
			desc.NodeMask = 0;
			DX_THROW_FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
			NAME_D3D12_OBJECT(descriptorHeap, L"D3D12 Descriptor Heap");
			LOG_CONSOLE(LogLevel::Info, L"D3D12 Descriptor Heap created");

			for (UINT32 i{ 0 }; i < capacity; i++)
			{
				freeHandles[i] = i;
			}
			descriptorSize = device->GetDescriptorHandleIncrementSize(type);
			cpuBaseAddress = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			gpuBaseAddress = isShaderVisible ? descriptorHeap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
		}

		DX_DescriptorHeap::~DX_DescriptorHeap() {}

		DX_DescriptorHeap::DescriptorHandle DX_DescriptorHeap::Allocate()
		{
			std::lock_guard lock{ mtx };
			assert(descriptorHeap);
			assert(size < capacity);

			const UINT32 index{ freeHandles[size] };
			const UINT32 offset{ index * descriptorSize };
			size++;
			DescriptorHandle handle;
			handle.cpuAddress.ptr = cpuBaseAddress.ptr + offset;
			if (IsShaderVisible())
			{
				handle.gpuAddress.ptr = gpuBaseAddress.ptr + offset;
			}
#ifdef _DEBUG
			handle.container = this;
			handle.index = index;
#endif // _DEBUG

			return handle;
		}

		void DX_DescriptorHeap::Free(DescriptorHandle& handle)
		{
			if (!handle.IsValid()) return;
			std::lock_guard lock{ mtx };
			assert(descriptorHeap && size);
			assert(handle.container == this);
			assert(handle.cpuAddress.ptr >= cpuBaseAddress.ptr);
			assert((handle.cpuAddress.ptr - cpuBaseAddress.ptr) % descriptorSize == 0);
			assert(handle.index < capacity);
			const UINT32 index{ (UINT32)(handle.cpuAddress.ptr - cpuBaseAddress.ptr) / descriptorSize };
			assert(handle.index == index);

			// we will defer release so the gpu is not using the descriptor
			const UINT32 frameIndex{};
			deferredFreeIndices[frameIndex].push_back(index);
			handle = {};
		}

		constexpr D3D12_CPU_DESCRIPTOR_HANDLE DX_DescriptorHeap::GetCpuBaseAddress() const { return cpuBaseAddress; }
		constexpr D3D12_GPU_DESCRIPTOR_HANDLE DX_DescriptorHeap::GetGpuBaseAddress() const { return gpuBaseAddress; }
		constexpr UINT32 DX_DescriptorHeap::GetCapacity() const { return capacity; }
		constexpr UINT32 DX_DescriptorHeap::GetSize() const { return size; }
		constexpr UINT32 DX_DescriptorHeap::GetDescriptorSize() const { return descriptorSize; }
		constexpr bool DX_DescriptorHeap::IsShaderVisible() const { return gpuBaseAddress.ptr != 0; }
		constexpr D3D12_DESCRIPTOR_HEAP_TYPE DX_DescriptorHeap::GetType() const { return type; }
	}
}