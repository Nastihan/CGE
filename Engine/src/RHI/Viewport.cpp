
// RHI
#include "Viewport.h"

namespace CGE
{
	namespace RHI
	{
        Viewport::Viewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) : m_minX(minX), m_maxX(maxX), m_minY(minY), m_maxY(maxY), m_minZ(minZ), m_maxZ(maxZ) {}
	}
}