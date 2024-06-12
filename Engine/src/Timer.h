#pragma once

#include <chrono>

namespace CGE
{
	class Timer
	{
	public:
		Timer();
		float Mark();
		float Peek() const;
		float GetAppTime() const;
		
	private:
		std::chrono::steady_clock::time_point m_deltaTime;
		std::chrono::duration<float> m_appTime;
	};
}