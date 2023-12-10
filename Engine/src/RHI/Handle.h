#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        //! Handle a simple wrapper around an integral type, which adds the formal concept of a 'Null' value. It
        //! is designed to accommodate a zero-based 'index' where a value of 0 is considered valid. As such, the null value
        //! is equal to -1 casted to the type.
        //!
        //! @tparam T
        //!  An integral type held by the Handle container. A value of -1 (or max value for unsigned types) is reserved for
        //!  the null index.
        //!
        //! @tparam NamespaceType
        //!  An optional typename used to create a compile-time unique variant of Handle. This disallows trivial
        //!  copying of unrelated 'types'. Useful to make a handle variant typed to a client class.
        //!
        //! Sample Usage:
        //! @code{.cpp}
        //!     class Foo;
        //!     using FooHandle = Handle<uint16_t, Foo>;
        //!     FooHandle fooHandle;
        //!
        //!     class Bar;
        //!     using BarHandle = Handle<uint16_t, Bar>;
        //!     BarHandle barHandle;
        //!
        //!     fooHandle = barHandle; // Error! Different types!
        //!     fooHandle.IsNull();    // true
        //!     fooHandle.GetIndex();  // FooHandle::NullIndex
        //!
        //!     fooHandle = 15;
        //!     fooHandle.GetIndex();  // 15
        //!     fooHandle.IsNull();    // false
        //! @endcode
        template <typename T = uint32_t>
        struct Handle
        {
            using IndexType = T;
            static_assert(std::is_integral<T>::value);

            // two's complement (max value)
            static const constexpr T NullIndex = T(-1);
            struct NullType {};
            static constexpr NullType Null{};
            constexpr Handle(NullType) {};

            constexpr Handle() = default;
            constexpr explicit Handle(T index) : m_index{ index } {}

            constexpr bool operator==(const Handle& rhs) const;
            constexpr bool operator!=(const Handle& rhs) const;
            constexpr bool operator<(const Handle& rhs) const;
            constexpr bool operator>(const Handle& rhs) const;
            constexpr bool operator<=(const Handle& rhs) const;

            /// Resets the handle to NullIndex.
            void Reset();

            /// Returns the index currently stored in the handle.
            constexpr T GetIndex() const;

            /// Returns whether the handle is equal to NullIndex.
            constexpr bool IsNull() const;

            /// Returns whether the handle is NOT equal to NullIndex.
            constexpr bool IsValid() const;

            T m_index = NullIndex;
        };

        template <typename T>
        constexpr bool Handle<T>::operator==(const Handle& rhs) const
        {
            return m_index == rhs.m_index;
        }

        template <typename T>
        constexpr bool Handle<T>::operator!=(const Handle& rhs) const
        {
            return m_index != rhs.m_index;
        }

        template <typename T>
        constexpr bool Handle<T>::operator<(const Handle& rhs) const
        {
            return m_index < rhs.m_index;
        }

        template <typename T>
        constexpr bool Handle<T>::operator<=(const Handle& rhs) const
        {
            return m_index <= rhs.m_index;
        }

        template <typename T>
        constexpr bool Handle<T>::operator>(const Handle& rhs) const
        {
            return m_index > rhs.m_index;
        }

        template <typename T>
        void Handle<T>::Reset()
        {
            m_index = NullIndex;
        }

        template <typename T>
        constexpr T Handle<T>::GetIndex() const
        {
            return m_index;
        }

        template <typename T>
        constexpr bool Handle<T>::IsNull() const
        {
            return m_index == NullIndex;
        }

        template <typename T>
        constexpr bool Handle<T>::IsValid() const
        {
            return m_index != NullIndex;
        }
	}
}