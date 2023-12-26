#pragma once

// DX12
#include "DX_CommandListBase.h"

// RHI
#include "../RHI/CommandList.h"
#include "../RHI/ClearValue.h"

namespace CGE
{
	namespace DX12
	{
		class DX_CommandList : public RHI::CommandList, public DX_CommandListBase
		{
		public:
			struct ImageClearRequest
			{
				RHI::ClearValue m_clearValue;
				D3D12_CLEAR_FLAGS m_clearFlags = (D3D12_CLEAR_FLAGS)0;
				// [todo] Replace with imageview
				DX_DescriptorHandle handle;
			};
		public:
			static RHI::Ptr<DX_CommandList> Create();
			bool IsInitilized() const;
			void Init(DX_Device& device, RHI::HardwareQueueClass hardwareQueueClass, ID3D12CommandAllocator* commandAllocator, const std::shared_ptr<DX_DescriptorContext>& descriptorContext);
			void Shutdown() override;

			void Close() override;
			void ClearRenderTarget(const ImageClearRequest& request);

		private:
			DX_CommandList() = default;

		private:
			std::shared_ptr<DX_DescriptorContext> m_descriptorContext;
		};
	}
}