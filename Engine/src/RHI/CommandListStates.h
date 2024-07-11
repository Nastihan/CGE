#pragma once

// RHI
#include "Viewport.h"
#include "Scissor.h"

namespace CGE
{
	namespace RHI
	{
        // State of a property that affects the render target attachments in a command list.
        template<class T>
        struct CommandListRenderTargetsState
        {
            using StateList = std::vector<T>;
            void Set(std::span<const T> newElements)
            {
                m_states = StateList(newElements.begin(), newElements.end());
                m_isDirty = true;
            }

            bool IsValid() const
            {
                return !m_states.empty();
            }

            // List with the state for each render target.
            StateList m_states;
            // Whether the states have already been applied to the command list.
            bool m_isDirty = false;
        };

        using CommandListScissorState = CommandListRenderTargetsState<RHI::Scissor>;
        using CommandListViewportState = CommandListRenderTargetsState<RHI::Viewport>;
	}
}