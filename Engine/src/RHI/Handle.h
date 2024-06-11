#pragma once

// RHI
#include "RHI_Common.h"

namespace CGE
{
	namespace RHI
	{
        struct DefaultNamespaceType {};

        template <typename T = uint32_t, typename NamespaceType = DefaultNamespaceType>
        struct Handle
        {
            using IndexType = T;
            static_assert(std::is_integral<T>::value, "Integral type required for Handle<>.");

            static const constexpr T NullIndex = T(-1);
            struct NullType {};
            static constexpr NullType Null{};
            constexpr Handle(NullType) {};

            constexpr Handle() = default;
            constexpr explicit Handle(T index) : m_index{ index } {}

            template <typename U>
            constexpr explicit Handle(U index) : m_index{ static_cast<uint32_t>(index) } {}

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

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::operator==(const Handle& rhs) const
        {
            return m_index == rhs.m_index;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::operator!=(const Handle& rhs) const
        {
            return m_index != rhs.m_index;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::operator<(const Handle& rhs) const
        {
            return m_index < rhs.m_index;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::operator<=(const Handle& rhs) const
        {
            return m_index <= rhs.m_index;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::operator>(const Handle& rhs) const
        {
            return m_index > rhs.m_index;
        }

        template <typename T, typename NamespaceType>
        void Handle<T, NamespaceType>::Reset()
        {
            m_index = NullIndex;
        }

        template <typename T, typename NamespaceType>
        constexpr T Handle<T, NamespaceType>::GetIndex() const
        {
            return m_index;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::IsNull() const
        {
            return m_index == NullIndex;
        }

        template <typename T, typename NamespaceType>
        constexpr bool Handle<T, NamespaceType>::IsValid() const
        {
            return m_index != NullIndex;
        }

        template<class T>
        struct hash;

        template<typename HandleType, typename NamespaceType>
        struct hash<Handle<HandleType, NamespaceType>>
        {
            typedef size_t result_type;
            typedef Handle<HandleType, NamespaceType> argument;
            using argument_type = typename argument::IndexType;

            result_type operator()(const argument& value) const
            {
                return static_cast<result_type>(*reinterpret_cast<const argument_type*>(&value));
            }
        };
	}
}