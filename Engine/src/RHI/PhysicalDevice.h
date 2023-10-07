#pragma once
#include "PhysicalDeviceDescriptor.h"

// std
#include <vector>
#include <memory>

namespace CGE
{
	namespace RHI
	{
		class PhysicalDevice
		{
		public:
			virtual ~PhysicalDevice() = default;
			const PhysicalDeviceDescriptor& GetDescriptor() const;
			virtual void Shutdown() = 0;

		protected:
			PhysicalDeviceDescriptor m_descriptor;
		};

		using PhysicalDeviceList = std::vector<std::shared_ptr<PhysicalDevice>>;
	}
}