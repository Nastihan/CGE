
#include "Timer.h"

namespace CGE
{
	Timer::Timer()
	{
		m_deltaTime = std::chrono::steady_clock::now();
	}

	float Timer::Mark()
	{
		const auto old = m_deltaTime;
		m_deltaTime = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frameTime = m_deltaTime - old;
		m_appTime += frameTime;
		return frameTime.count();
	}

	float Timer::Peek() const
	{
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_deltaTime).count();
	}

	float Timer::GetAppTime() const
	{
		return m_appTime.count();
	}
}