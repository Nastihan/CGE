#include "DX_CommandList.h"

namespace CGE
{
	namespace DX12
	{
		RHI::Ptr<DX_CommandList> DX_CommandList::Create()
		{
			return new DX_CommandList();
		}
		bool DX_CommandList::IsInitilized() const
		{
			return DX_CommandListBase::IsInitilized();
		}
		void DX_CommandList::Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator)
		{
			DX_CommandListBase::Init(device, hardwareQueueClass, commandAllocator);
		}
		void DX_CommandList::Shutdown() {}
	}
}