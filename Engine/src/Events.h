#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

#include "KeyCodes.h"

namespace CGE
{
    class UpdateEventArgs;

    // Read https://www.boost.org/doc/libs/1_85_0/doc/html/signals2/tutorial.html#id-1.3.35.4.2 to understand this class.
    // Delegate template class for encapsulating event callback functions.
    template<typename ArgumentType>
    class Delegate
    {
    public:
        typedef boost::function<void(ArgumentType&, UpdateEventArgs&)> FunctionType;
        typedef boost::signals2::connection ConnectionType;
        typedef std::vector<boost::signals2::scoped_connection> ScopedConnections;

        // Add a callback to the the list
        // Returns the connection object that can be used to disconnect the 
        // subscriber from the signal.
        ConnectionType operator+=(typename const FunctionType& callback) const
        {
            return m_callbacks.connect(callback);
        }

        // Remove a callback from the list
        void operator-=(typename const FunctionType& callback) const
        {
            assert(false);
            // TODO: This isn't working yet.. Getting a compiler error:
            // Error	1	error C2666: 'boost::operator ==' : 4 overloads have similar conversions signal_template.hpp
            // WORKAROUND: Use the connection object returned when the subscriber was initially connected
            // to disconnect the subscriber.
            m_callbacks.disconnect<FunctionType>(callback);
        }

        void operator-=(ConnectionType& con)
        {
            m_callbacks.disconnect(con); // This doesn't seem to work either!?
            if (con.connected())
            {
                con.disconnect(); // This is stupid, then any connection passed to this function will be disconnected, even if it was never registered with this signal.
            }
        }

        // Invoke this event with the argument
        void operator()(typename ArgumentType& argument, UpdateEventArgs& uArgs)
        {
            m_callbacks(argument, uArgs);
        }

    private:
        typedef boost::signals2::signal<void(ArgumentType&, UpdateEventArgs&)> Callbacks;
        mutable Callbacks m_callbacks;
    };

    // Base class for all event args
    class EventArgs
    {
    public:
        EventArgs() = default;
    };
    typedef Delegate<EventArgs> Event;

    class UpdateEventArgs : public EventArgs
    {
    public:
        typedef EventArgs base;
        UpdateEventArgs(float fDeltaTime, float fTotalTime) : m_elapsedTime(fDeltaTime), m_totalTime(fTotalTime) {}

        float m_elapsedTime;
        float m_totalTime;
    };
    typedef Delegate<UpdateEventArgs> UpdateEvent;

    class KeyEventArgs : public EventArgs
    {
    public:
        enum KeyState
        {
            Released = 0,
            Pressed = 1
        };

        typedef EventArgs base;
        KeyEventArgs(int key, KeyState state, bool control, bool shift, bool alt, unsigned int c = 0)
            : m_key(key)
            , m_char(c)
            , m_state(state)
            , m_control(control)
            , m_shift(shift)
            , m_alt(alt)
        {}

        int m_key; // The glfw Key Code that was pressed or released.
        unsigned int m_char; // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
        KeyState m_state; // Was the key pressed or released.
        bool m_control; // Is the Control modifier pressed
        bool m_shift; // Is the Shift modifier pressed
        bool m_alt; // Is the Alt modifier pressed
    };
    typedef Delegate<KeyEventArgs> KeyboardEvent;

    class MouseMotionEventArgs : public EventArgs
    {
    public:
        typedef EventArgs base;
        MouseMotionEventArgs(bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
            : m_leftButton(leftButton)
            , m_middleButton(middleButton)
            , m_rightButton(rightButton)
            , m_control(control)
            , m_shift(shift)
            , m_x(x)
            , m_y(y)
        {}

        bool m_leftButton; // Is the left mouse button down?
        bool m_middleButton; // Is the middle mouse button down?
        bool m_rightButton; // Is the right mouse button down?
        bool m_control; // Is the CTRL key down?
        bool m_shift; // Is the Shift key down?

        int m_x; // The X-position of the cursor relative to the upper-left corner of the client area.
        int m_y; // The Y-position of the cursor relative to the upper-left corner of the client area.
        int m_relX; // How far the mouse moved since the last event.
        int m_relY; // How far the mouse moved since the last event.
    };
    typedef Delegate<MouseMotionEventArgs> MouseMotionEvent;

    class MouseButtonEventArgs : public EventArgs
    {
    public:
        enum MouseButton
        {
            None = 0,
            Left = 1,
            Right = 2,
            Middel = 3
        };
        enum ButtonState
        {
            Released = 0,
            Pressed = 1
        };

        typedef EventArgs base;
        MouseButtonEventArgs(MouseButton buttonID, ButtonState state, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
            : m_button(buttonID)
            , m_state(state)
            , m_leftButton(leftButton)
            , m_middleButton(middleButton)
            , m_rightButton(rightButton)
            , m_control(control)
            , m_shift(shift)
            , m_x(x)
            , m_y(y)
        {}

        MouseButton m_button; // The mouse button that was pressed or released.
        ButtonState m_state; // Was the button pressed or released?
        bool m_leftButton; // Is the left mouse button down?
        bool m_middleButton; // Is the middle mouse button down?
        bool m_rightButton; // Is the right mouse button down?
        bool m_control; // Is the CTRL key down?
        bool m_shift; // Is the Shift key down?

        int m_x; // The X-position of the cursor relative to the upper-left corner of the client area.
        int m_y; // The Y-position of the cursor relative to the upper-left corner of the client area.
    };
    typedef Delegate<MouseButtonEventArgs> MouseButtonEvent;

    class MouseWheelEventArgs : public EventArgs
    {
    public:
        typedef EventArgs base;
        MouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
            : m_wheelDelta(wheelDelta)
            , m_leftButton(leftButton)
            , m_middleButton(middleButton)
            , m_rightButton(rightButton)
            , m_control(control)
            , m_shift(shift)
            , m_x(x)
            , m_y(y)
        {}

        float m_wheelDelta; // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.
        bool m_leftButton; // Is the left mouse button down?
        bool m_middleButton; // Is the middle mouse button down?
        bool m_rightButton; // Is the right mouse button down?
        bool m_control; // Is the CTRL key down?
        bool m_shift; // Is the Shift key down?

        int m_x; // The X-position of the cursor relative to the upper-left corner of the client area.
        int m_y; // The Y-position of the cursor relative to the upper-left corner of the client area.
    };
    typedef Delegate<MouseWheelEventArgs> MouseWheelEvent;

    class ResizeEventArgs : public EventArgs
    {
    public:
        typedef EventArgs base;
        ResizeEventArgs(int width, int height) :  m_width(width), m_height(height) {}

        // The new width of the window
        int m_width;
        // The new height of the window.
        int m_height;
    };
    typedef Delegate<ResizeEventArgs> ResizeEvent;
}