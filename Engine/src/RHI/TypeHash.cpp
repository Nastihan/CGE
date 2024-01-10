
// RHI
#include "TypeHash.h"

#ifdef COMPILE_CITY_CC
#include "city.cc"
#endif

#include <city.h>


namespace CGE
{
	namespace RHI
	{
        HashValue32 TypeHash32(const uint8_t* buffer, uint64_t length)
        {
            assert(buffer, "TypeHash32() - buffer cannot be null");
            return static_cast<HashValue32>(CityHash32(reinterpret_cast<const char*>(buffer), length));
        }

        HashValue64 TypeHash64(const uint8_t* buffer, uint64_t length)
        {
            assert(buffer, "TypeHash64() - buffer cannot be null");
            return static_cast<HashValue64>(CityHash64(reinterpret_cast<const char*>(buffer), length));
        }

        HashValue64 TypeHash64(const uint8_t* buffer, uint64_t length, HashValue64 seed)
        {
            assert(buffer, "TypeHash64() - buffer cannot be null");
            return static_cast<HashValue64>(CityHash64WithSeed(reinterpret_cast<const char*>(buffer), length, static_cast<uint64_t>(seed)));
        }

        HashValue64 TypeHash64(const uint8_t* buffer, uint64_t length, HashValue64 seed1, HashValue64 seed2)
        {
            assert(buffer, "TypeHash64() - buffer cannot be null");
            return static_cast<HashValue64>(CityHash64WithSeeds(reinterpret_cast<const char*>(buffer), length, static_cast<uint64_t>(seed1), static_cast<uint64_t>(seed2)));
        }
	}
}