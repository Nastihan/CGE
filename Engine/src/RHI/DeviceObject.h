#pragma once
#include "Device.h"

namespace CGE
{
	namespace RHI
	{
		class DeviceObject : public Object
		{
		public:
			virtual ~DeviceObject() = default;
			bool IsInitialized() const;
			Device& GetDevice() const;
		protected:
			DeviceObject() = default;
			void Init(Device& device);
			void Shutdown() override;
		private:
			RHI::Ptr<Device> m_device = nullptr;
		};
	}
}