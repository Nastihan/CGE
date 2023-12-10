#pragma once
#include "DX_CommonHeaders.h"
#include "DX_CommandQueue.h"
#include "DX_Device.h"
#include "../RHI/DeviceObject.h"

namespace CGE
{
	namespace DX12
	{
		class Device;

		class DX_CommandListBase : public RHI::DeviceObject
		{
			using Base = RHI::DeviceObject;

		public:
			virtual ~DX_CommandListBase() = 0;
			DX_CommandListBase(const DX_CommandListBase&) = delete;
			virtual void Reset(ID3D12CommandAllocator* commandAllocator);
			virtual void Close();
			bool IsRocording() const;
			ID3D12GraphicsCommandList* GetCommandList();
			const ID3D12GraphicsCommandList* GetCommandList() const;
			RHI::HardwareQueueClass GetHardwareQueueClass() const;

		protected:
			void Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator);

			DX_CommandListBase() = default;

		private:
			void SetNameInternal(const std::string& name) override;

		private:
			RHI::HardwareQueueClass m_hardwareQueueClass;
			wrl::ComPtr<ID3D12GraphicsCommandListX> m_commandList;
			bool m_isRecording = false;
		};
	}
}