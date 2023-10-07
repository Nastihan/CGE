#pragma once

namespace CGE
{
	namespace RHI
	{
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
	}
}