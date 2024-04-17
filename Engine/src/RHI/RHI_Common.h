#pragma once
#include "intrusive_ptr.h"
#include "../CommonMacros.h"
#include "MathTypedefs.h"

namespace CGE
{
	namespace RHI
	{
        template<typename T>
        using Ptr = intrusive_ptr<T>;

        template <typename T>
        using ConstPtr = intrusive_ptr<const T>;

        typedef signed   __int8  int8_t;
        typedef unsigned __int8  uint8_t;
        typedef signed   __int16 int16_t;
        typedef unsigned __int16 uint16_t;
        typedef signed   __int32 int32_t;
        typedef unsigned __int32 uint32_t;
        typedef signed   __int64 int64_t;
        typedef unsigned __int64 uint64_t;

        using u32 = uint32_t;
        using u64 = unsigned long long;

        enum class ResultCode : uint32_t
        {
            // The operation succeeded.
            Success = 0,

            // The operation failed with an unknown error.
            Fail,

            // The operation failed due being out of memory.
            OutOfMemory,

            // The operation failed because the feature is unimplemented on the particular platform.
            Unimplemented,

            // The operation failed because the API object is not in a state to accept the call.
            InvalidOperation,

            // The operation failed due to invalid arguments.
            InvalidArgument,

            // The operation is not ready
            NotReady
        };

        #ifdef _DEBUG
        static constexpr bool ISDEBUG = true;
        #else
        static constexpr bool ISDEBUG = false;
        #endif
	}
    // Functions
    namespace RHI
    {
        //! Aligns value up to the given alignment. It doesn't require the alignment to be a power of two
        //! AlignUpNPOT(13, 4) => 16
        template <typename T> constexpr T AlignUpNPOT(T value, size_t alignment)
        {
            return value + (value % alignment > 0 ? (alignment - (value % alignment)) : 0);
        }

        //! Aligns value up to the given bit mask. Assumes mask is power of two minus 1.
        template <typename T> constexpr T AlignUpWithMask(T value, size_t mask)
        {
            return (T)(((size_t)value + mask) & ~mask);
        }

        //! Aligns value up to the given alignment. Assumes alignment is power of two.
        template <typename T> constexpr T AlignUp(T value, size_t alignment)
        {
            return AlignUpWithMask(value, alignment - 1);
        }

        //! Returns whether value is divisible by divisor.
        template <typename T> bool IsDivisible(T value, T divisor)
        {
            return (value / divisor) * divisor == value;
        }

        //! Returns whether all the set bits in bits are set in v.
        template <typename T>
        inline bool CheckBitsAll(T v, T bits)
        {
            return (v & bits) == bits;
        }

        //! Returns whether any of the set bits in bits are set in v.
        template <typename T>
        inline bool CheckBitsAny(T v, T bits)
        {
            return (v & bits) != (T)0;
        }

        //! Sets any zero bits in bits in v to 1.
        template <typename T>
        inline T SetBits(T v, T bits)
        {
            return v | bits;
        }

        //! Resets any non-zero bits in bits in v to 0.
        template <typename T>
        inline T ResetBits(T v, T bits)
        {
            return v & (~bits);
        }

        //! Returns whether the value is aligned to the given alignment.
        template <typename T> inline bool IsAligned(T value, size_t alignment)
        {
            return 0 == ((size_t)value & (alignment - 1));
        }

        //! Reset any zero bits in bits in v to 0.
        template <typename T>
        inline T FilterBits(T v, T bits)
        {
            return v & bits;
        }

        //! Returns the number of bits set in v.
        inline uint8_t CountBitsSet(uint8_t v)
        {
            uint8_t c = v;
            c = ((c >> 1) & 0x55) + (c & 0x55);
            c = ((c >> 2) & 0x33) + (c & 0x33);
            c = ((c >> 4) & 0x0f) + (c & 0x0f);
            return c;
        }

        //! Returns the number of bits set in v.
        inline uint8_t CountBitsSet(uint16_t v)
        {
            return
                CountBitsSet((uint8_t)((v) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 8) & 0xff));
        }

        //! Returns the number of bits set in v.
        inline uint8_t CountBitsSet(uint32_t v)
        {
            return
                CountBitsSet((uint8_t)((v) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 8) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 16) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 24) & 0xff));
        }

        //! Returns the number of bits set in v.
        inline uint8_t CountBitsSet(uint64_t v)
        {
            return
                CountBitsSet((uint8_t)((v) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 8) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 16) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 24) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 32) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 40) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 48) & 0xff)) +
                CountBitsSet((uint8_t)((v >> 56) & 0xff));
        }
    }
}