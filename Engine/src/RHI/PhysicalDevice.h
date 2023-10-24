#pragma once
#include "RHI_Common.h"
#include "PhysicalDeviceDescriptor.h"
#include "Object.h"

// std
#include <vector>
#include <memory>

namespace CGE
{
	namespace RHI
	{
		class PhysicalDevice : public Object
		{
		public:
			virtual ~PhysicalDevice() = default;
			const PhysicalDeviceDescriptor& GetDescriptor() const;

		protected:
			PhysicalDeviceDescriptor m_descriptor;
		};
		using PhysicalDeviceList = std::vector<RHI::Ptr<PhysicalDevice>>;
	}
}