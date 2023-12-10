#include "DX_CommandListBase.h"
#include "DX_Conversions.h"

namespace CGE
{
	namespace DX12
	{
		DX_CommandListBase::~DX_CommandListBase() {}
		void DX_CommandListBase::Reset(ID3D12CommandAllocator* commandAllocator)
		{
			m_commandList->Reset(commandAllocator, nullptr);
			m_isRecording = true;
		}

		void DX_CommandListBase::Close()
		{
			assert(m_isRecording, "Attempting to close command list that isn't in a recording state");
			m_isRecording = false;
			DXAssertSuccess(m_commandList->Close());
		}

		bool DX_CommandListBase::IsRocording() const
		{
			return m_isRecording;
		}

		ID3D12GraphicsCommandList* DX_CommandListBase::GetCommandList()
		{
			return m_commandList.Get();
		}

		const ID3D12GraphicsCommandList* DX_CommandListBase::GetCommandList() const
		{
			return m_commandList.Get();
		}

		RHI::HardwareQueueClass DX_CommandListBase::GetHardwareQueueClass() const
		{
			return m_hardwareQueueClass;
		}

		void DX_CommandListBase::Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator)
		{
			Base::Init(device);
			m_hardwareQueueClass = hardwareQueueClass;

			device.DXAssertSuccess(device.GetDevice()->CreateCommandList(1, ConvertHardwareQueueClass(hardwareQueueClass), commandAllocator, nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
			m_isRecording = true;
		}

		void DX_CommandListBase::SetNameInternal(const std::string& name)
		{
			std::wstring wName = s2ws(name);
			GetCommandList()->SetName(wName.c_str());
		}
	}
}