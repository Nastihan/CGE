
// DX12
#include "DX_Device.h"
#include "DX_PhysicalDevice.h"
#include "DX_CommandList.h"
#include "DX_MemoryView.h"
#include "DX_Conversions.h"

// RHI
#include "../RHI/SwapChain.h"

// std
#include <iostream>

#include "D3dx12.h"

using namespace Microsoft::WRL;

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<RHI::Device> DX_Device::Create()
		{
			return new DX_Device();
		}

		RHI::ResultCode DX_Device::InitInternal(RHI::PhysicalDevice& physicalDevice)
		{
			LOCAL_HR;
			DX_PhysicalDevice& dxPhysicalDevice = static_cast<DX_PhysicalDevice&>(physicalDevice);
			if (RHI::ISDEBUG)
			{
				EnableD3DDebugLayer();
				EnableGPUBasedValidation();
			}
			
			DX_THROW_FAILED(D3D12CreateDevice(dxPhysicalDevice.GetAdapter(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_device.GetAddressOf())));

			if (RHI::ISDEBUG)
			{
				EnableDebugDeviceFeatures();
				EnableBreakOnD3DError();
			}

			m_dxgiFactory = dxPhysicalDevice.GetFactory();
			m_dxgiAdapter = dxPhysicalDevice.GetAdapter();

			return RHI::ResultCode::Success;
		}

		void DX_Device::ShutdownInternal()
		{
			m_releaseQueue.Shutdown();
		}

		void DX_Device::EnableD3DDebugLayer()
		{
			LOCAL_HR;
			ComPtr<ID3D12Debug> debugController;
			DX_THROW_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}

		void DX_Device::EnableGPUBasedValidation()
		{
			LOCAL_HR;
			ComPtr<ID3D12Debug1> debugController1;
			DX_THROW_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController1)))
			debugController1->SetEnableGPUBasedValidation(TRUE);
			debugController1->SetEnableSynchronizedCommandQueueValidation(TRUE);

			ComPtr<ID3D12Debug2> debugController2;
			DX_THROW_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController2)))
			debugController2->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);
		}

		void DX_Device::EnableDebugDeviceFeatures()
		{
			LOCAL_HR;
			ComPtr<ID3D12DebugDevice> debugDevice;
			DX_THROW_FAILED(m_device->QueryInterface(debugDevice.GetAddressOf()))
			debugDevice->SetFeatureMask(D3D12_DEBUG_FEATURE_ALLOW_BEHAVIOR_CHANGING_DEBUG_AIDS | D3D12_DEBUG_FEATURE_CONSERVATIVE_RESOURCE_STATE_TRACKING);
		}

		void DX_Device::EnableBreakOnD3DError()
		{
			LOCAL_HR;
			ComPtr<ID3D12InfoQueue> infoQueue;
			DX_THROW_FAILED(m_device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}

		std::string DX_Device::GetDeviceRemovedReason() const
		{
			ID3D12Device* removedDevice = m_device.Get();
			HRESULT removedReason = removedDevice->GetDeviceRemovedReason();

			switch (removedReason)
			{
			case DXGI_ERROR_DEVICE_HUNG:
				return "Direct3D 12 Device Hung";
			case DXGI_ERROR_DEVICE_REMOVED:
				return "Direct3D 12 Device Removed";
			case DXGI_ERROR_DEVICE_RESET:
				return "Direct3D 12 Device Reset";
			case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
				return "DXGI Driver Internal Error";
			case DXGI_ERROR_INVALID_CALL:
				return "DXGI Invalid Call";
			case DXGI_ERROR_ACCESS_DENIED:
				return "DXGI Access Denied";
			case S_OK:
				return "The method succeeded without an error.";
			default:
				return "Unknown DXGI Error code, please check output log";
			}
		}

		void DX_Device::OnDeviceRemoved()
		{
			//[todo]
		}

		ID3D12DeviceX* DX_Device::GetDevice() const
		{
			return m_device.Get();
		}

		const wrl::ComPtr<IDXGIFactoryX>& DX_Device::GetDxgiFactory() const
		{
			return m_dxgiFactory;
		}

		bool DX_Device::DXAssertSuccess(HRESULT hr)
		{
			return DX12::DXAssertSuccess(hr);
		}

		RHI::ResultCode DX_Device::BeginFrameInternal()
		{
			return RHI::ResultCode::Success;
		}

		void DX_Device::EndFrameInternal()
		{
			m_commandQueueContext.End();
			m_commandListAllocator.Collect();
			m_releaseQueue.Collect();
		}

		RHI::ResultCode DX_Device::InitializeLimits()
		{
			{
				DX_ReleaseQueue::Descriptor releaseQueueDescriptor;
				releaseQueueDescriptor.m_collectLatency = RHI::Limits::Device::FrameCountMax - 1;
				m_releaseQueue.Init(releaseQueueDescriptor);
			}

			DX_CommandListAllocator::Descriptor commandListAllocatorDescriptor;
			commandListAllocatorDescriptor.m_dxDevice = this;
			commandListAllocatorDescriptor.m_frameCountMax = RHI::Limits::Device::FrameCountMax;
			m_commandListAllocator.Init(commandListAllocatorDescriptor);

			m_commandQueueContext.Init(*this);

			m_descriptorContext = std::make_shared<DX_DescriptorContext>();
			m_descriptorContext->Init(m_device.Get());

			return RHI::ResultCode::Success;
		}

		DX_CommandQueueContext& DX_Device::GetCommandQueueContext()
		{
			return m_commandQueueContext;
		}

		DX_DescriptorContext& DX_Device::GetDescriptorContext()
		{
			return *m_descriptorContext;
		}

		DX_CommandList* DX_Device::AcquireCommandList(RHI::HardwareQueueClass hardwareQueueClass)
		{
			return m_commandListAllocator.Allocate(hardwareQueueClass);
		}

		void DX_Device::WaitForIdleInternal()
		{
			m_commandQueueContext.WaitForIdle();
			m_releaseQueue.Collect(true);
		}

		DX_MemoryView DX_Device::CreateBufferCommitted(const RHI::BufferDescriptor& bufferDescriptor, D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType)
		{
			D3D12_RESOURCE_DESC resourceDesc = {};
			ConvertBufferDescriptor(bufferDescriptor, resourceDesc);
			CD3DX12_HEAP_PROPERTIES heapProperties(heapType);

			wrl::ComPtr<ID3D12Resource> resource;
			DXAssertSuccess(m_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, nullptr, IID_PPV_ARGS(resource.GetAddressOf())));

			D3D12_RESOURCE_ALLOCATION_INFO allocationInfo;
			allocationInfo.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
			allocationInfo.SizeInBytes = RHI::AlignUp(resourceDesc.Width, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

			return DX_MemoryView(resource.Get(), 0, allocationInfo.SizeInBytes, allocationInfo.Alignment, DX_MemoryViewType::Buffer);
		}

		void DX_Device::QueueForRelease(RHI::Ptr<ID3D12Object> dxObject)
		{
			m_releaseQueue.QueueForCollect(std::move(dxObject));
		}

		void DX_Device::QueueForRelease(const DX_MemoryView memoryView)
		{
			m_releaseQueue.QueueForCollect(memoryView.GetMemory());
		}
	}
}