﻿/*
Copyright (c) 2003-2006 Gino van den Bergen / Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _MIN_MAX_H_
#define _MIN_MAX_H_

template <class T>
TFSIMD_FORCE_INLINE const T& tfMin(const T& a, const T& b)
{
    return a < b ? a : b;
}

template <class T>
TFSIMD_FORCE_INLINE const T& tfMax(const T& a, const T& b)
{
    return  a > b ? a : b;
}

template <class T>
TFSIMD_FORCE_INLINE void tfSetMin(T& a, const T& b)
{
    if (b < a)
    {
        a = b;
    }
}

template <class T>
TFSIMD_FORCE_INLINE void tfSetMax(T& a, const T& b)
{
    if (a < b)
    {
        a = b;
    }
}

#endif
