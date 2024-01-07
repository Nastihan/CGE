#pragma once

// DX12
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/ObjectCollector.h"

namespace CGE
{
	namespace DX12
	{
		class DX_ReleaseQueueTraits : public RHI::ObjectCollectorTraits
		{
		public:
			using MutexType = std::mutex;
			using ObjectType = ID3D12Object;
		};

		using DX_ReleaseQueue = RHI::ObjectCollector<DX_ReleaseQueueTraits>;
	}
}