#pragma once
#include "VK_CommonHeaders.h"
#include <vector>

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
		void InitDevice();

		std::vector<const char*> GetRequiredExtensions();
		VkPhysicalDevice PickDevice();
		int RateDevice(VkPhysicalDevice device);

	public: 

	private:
		static VK_Device VKDeviceInstance;

		VkInstance instance = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;

		const std::vector<const char*> instanceExtensions =
		{
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // Enable seting names
		};

	};
}