#pragma once
#include "intrusive_ptr.h"
#include "../CommonMacros.h"

namespace CGE
{
	namespace RHI
	{
        template<typename T>
        using Ptr = intrusive_ptr<T>;

        typedef signed   __int8  int8_t;
        typedef unsigned __int8  uint8_t;
        typedef signed   __int16 int16_t;
        typedef unsigned __int16 uint16_t;
        typedef signed   __int32 int32_t;
        typedef unsigned __int32 uint32_t;
        typedef signed   __int64 int64_t;
        typedef unsigned __int64 uint64_t;

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
    }
}