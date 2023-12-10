#pragma once

#include "../CommonMacros.h"
#include "Factory.h"

#include <string>

namespace CGE
{
	namespace RHI
	{
		class Graphics
		{
		public:
			Graphics(std::string bakcendAPI, const Window& window);
			~Graphics() = default;
			REMOVE_COPY_AND_MOVE(Graphics);
		public:
			void Init();
			void Render();

		private:
			std::string m_backendAPI;
			RHI::Ptr<Factory> m_factory;
		private:
			RHI::Ptr<RHI::PhysicalDevice> m_physicalDevice;
			RHI::Ptr<RHI::Device> m_device;
			RHI::Ptr<RHI::SwapChain> m_swapChain;

			const Window& m_window;
		};
	}
}