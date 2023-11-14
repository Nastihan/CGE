#pragma once
#include "DX_CommonHeaders.h"
#include "../RHI/CommandQueue.h"

namespace CGE
{
	namespace DX12
	{
		D3D12_COMMAND_LIST_TYPE ConvertHardwareQueueClass(RHI::HardwareQueueClass type);

		std::wstring s2ws(const std::string& str);
	}
}