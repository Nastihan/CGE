#pragma once

// DX!2
#include "DX_CommonHeaders.h"

// RHI
#include "../RHI/CommandQueue.h"
#include "../RHI/Format.h"
#include "../RHI/SwapChainDescriptor.h"

namespace CGE
{
	namespace DX12
	{
		D3D12_COMMAND_LIST_TYPE ConvertHardwareQueueClass(RHI::HardwareQueueClass type);

		std::wstring s2ws(const std::string& str);

		DXGI_FORMAT ConvertFormat(RHI::Format format);

		DXGI_SCALING ConvertScaling(RHI::Scaling scaling);
	}
}