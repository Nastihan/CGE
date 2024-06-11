#pragma once

namespace CGE
{
	namespace RHI
	{
		struct DeviceFeatures
		{
			bool m_rayTracing = false;
			bool m_computeShader = false;
			bool m_unboundedArrays = false;
			// [todo] add more
		};
	}
}