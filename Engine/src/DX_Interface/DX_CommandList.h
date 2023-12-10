#pragma once
#include "../RHI/CommandList.h"
#include "DX_CommandListBase.h"

namespace CGE
{
	namespace DX12
	{
		class DX_CommandList : public RHI::CommandList, public DX_CommandListBase
		{
		public:
			static RHI::Ptr<DX_CommandList> Create();
			bool IsInitilized() const;
			void Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator);
			void Shutdown() override;

		private:
			DX_CommandList() = default;
		};
	}
}