
#include "Scissor.h"

namespace CGE
{
	namespace RHI
	{
        Scissor::Scissor(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY) : m_minX(minX), m_minY(minY), m_maxX(maxX), m_maxY(maxY) {}
	}
}