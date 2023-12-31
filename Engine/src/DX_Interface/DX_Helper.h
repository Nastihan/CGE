#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

inline void DX_THROW_FAILED(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw std::exception();
    }
}