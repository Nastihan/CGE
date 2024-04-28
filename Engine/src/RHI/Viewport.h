#pragma once

namespace CGE
{
	namespace RHI
	{
        struct Viewport
        {
            Viewport() = default;
            Viewport(float minX, float maxX, float minY, float maxY, float minZ = 0.0f, float maxZ = 1.0f);

            float GetWidth() const;
            float GetHeight() const;
            float GetDepth() const;

            float m_minX = 0.0f;
            float m_maxX = 0.0f;
            float m_minY = 0.0f;
            float m_maxY = 0.0f;
            float m_minZ = 0.0f;
            float m_maxZ = 1.0f;
        };
	}

    inline float RHI::Viewport::GetWidth() const
    {
        return m_maxX - m_minX;
    }

    inline float RHI::Viewport::GetHeight() const
    {
        return m_maxY - m_minY;
    }

    inline float RHI::Viewport::GetDepth() const
    {
        return m_maxZ - m_minZ;
    }
}