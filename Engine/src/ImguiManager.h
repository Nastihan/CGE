#pragma once

#include "RHI/Device.h"
#include "RHI/DeviceObject.h"
#include "RHI/CommandList.h"

#include <wrl.h>
#include <d3d12.h>

namespace CGE
{
	// Just using this class to manage initilization order
	class ImguiManager : public RHI::DeviceObject
	{
	public:
		ImguiManager() = default;
		virtual ~ImguiManager() = default;

		void Init(RHI::Device& device);
		void Shutdown() override;
		void FrameBegin();
		void BuildCommandList(RHI::CommandList& commandList);
		void PushSpawnableWindow(std::function<void()> spawnableWindow);
	protected:
		virtual void InitInternal(RHI::Device& device) = 0;
		virtual void FrameBeginInternal() = 0;
		virtual void BuildCommandListInternal(RHI::CommandList& commandList) = 0;

	protected:
		std::vector<std::function<void()>> m_spawnableWindows;
	};
}