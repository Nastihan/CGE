#pragma once
#include "VK_CommonHeaders.h"

namespace CGE
{
	class VK_Device
	{
	public:
		static const VK_Device& GetInstance();
		
		~VK_Device();
	private:
		VK_Device();
		VK_Device(const VK_Device&) = delete;
		VK_Device(VK_Device&&) = delete;
		VK_Device& operator=(const VK_Device&) = delete;
		VK_Device& operator=(VK_Device&&) = delete;
	private:
		void InitInstance();

	public: 

	private:
		static VK_Device VKDeviceInstance;
		VkInstance instance{};

	};
}