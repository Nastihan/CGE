
// RHI
#include "PlatformLimitsDescriptor.h"

namespace CGE
{
	namespace RHI
	{
		RHI::Ptr<PlatformLimitsDescriptor> PlatformLimitsDescriptor::Create()
		{
			return new PlatformLimitsDescriptor();
		}

		void PlatformLimitsDescriptor::LoadPlatformLimitsDescriptor(const char* rhiName) {}
	}
}