#pragma once

// RHI
#include "Factory.h"
#include "BufferSystem.h"
#include "ImageSystem.h"

#include "../ImguiManager.h"
#include "../CommonMacros.h"

#include <string>

namespace CGE
{
	namespace RHI
	{
		class Graphics
		{
		public:
			Graphics(std::string bakcendAPI, Window& window);
			~Graphics() = default;
			REMOVE_COPY_AND_MOVE(Graphics);

			ResultCode RecreateSwapChain();
		public:
			RHI::Device& GetDevice() const;
			void Init();
			void Render();
			static Factory& GetFactory();
			static BufferSystem& GetBufferSystem();
			static ImageSystem& GetImageSystem();
			static ImguiManager& GetImguiManager();

			// [todo] remove
			RHI::Ptr<RHI::FrameGraphExecuter> GetFrameGraphExecuter() const;

		private:
			std::string m_backendAPI;
			static RHI::Ptr<Factory> m_factory;
			static RHI::Ptr<BufferSystem> m_bufferSystem;
			static RHI::Ptr<ImageSystem> m_imageSystem;
			static RHI::Ptr<ImguiManager> m_imguiManager;
		private:
			RHI::Ptr<RHI::PhysicalDevice> m_physicalDevice;
			RHI::Ptr<RHI::Device> m_device;
			RHI::Ptr<RHI::SwapChain> m_swapChain;
			RHI::Ptr<RHI::FrameGraphExecuter> m_frameGraphExecuter;
			// RHI::Ptr<RHI::PipelineState> m_fowradPSO;

			Window& m_window;
		};
	}
}