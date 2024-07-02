
#include "Light.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"

namespace CGE
{
    namespace Scene
    {
        Light::Light()
            : m_positionWS(0, 0, 0, 1)
            , m_directionWS(0, 0, -1, 0)
            , m_positionVS(0, 0, 0, 1)
            , m_directionVS(0, 0, 1, 0)
            , m_color(1, 1, 1, 1)
            , m_spotlightAngle(45.0f)
            , m_range(100.0f)
            , m_intensity(1.0f)
            , m_enabled(true)
            , m_selected(false)
            , m_type(LightType::Point) {}
    }
}