#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace CGE
{
    namespace Scene
    {
        __declspec(align(16)) struct Light
        {
            enum class LightType : uint32_t
            {
                Point = 0,
                Spot = 1,
                Directional = 2
            };

            /**
            * Position for point and spot lights (World space).
            */
            glm::vec4 m_positionWS;
            //--------------------------------------------------------------( 16 bytes )
            /**
            * Direction for spot and directional lights (World space).
            */
            glm::vec4 m_directionWS;
            //--------------------------------------------------------------( 16 bytes )
            /**
            * Position for point and spot lights (View space).
            */
            glm::vec4 m_positionVS;
            //--------------------------------------------------------------( 16 bytes )
            /**
            * Direction for spot and directional lights (View space).
            */
            glm::vec4 m_directionVS;
            //--------------------------------------------------------------( 16 bytes )
            /**
             * Color of the light. Diffuse and specular colors are not separated.
             */
            glm::vec4 m_color;
            //--------------------------------------------------------------( 16 bytes )
            /**
             * The half angle of the spotlight cone.
             */
            float m_spotlightAngle;
            /**
             * The range of the light.
             */
            float m_range;

            /**
             * The intensity of the light.
             */
            float m_intensity;

            /**
             * Disable or enable the light.
             */
            uint32_t m_enabled;
            //--------------------------------------------------------------(16 bytes )

            /**
             * True if the light is selected in the editor.
             */
            uint32_t m_selected;
            /**
             * The type of the light.
             */
            LightType m_type;

            glm::vec2 m_padding;
            //--------------------------------------------------------------(16 bytes )
            //--------------------------------------------------------------( 16 * 7 = 112 bytes )
            Light();
        };
    }
}