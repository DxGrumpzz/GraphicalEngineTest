#pragma once

#include <cstdint>


namespace Maths
{

    inline constexpr float PI = 3.14159265358f;


    inline float DegreesToRadians(float degrees)
    {
        return degrees * (PI / 180);
    };

    inline float RadiansToDegrees(float radians)
    {
        return radians * (180 / PI);
    };


    inline int Convert2DTo1D(int x, int y, int length)
    {
        std::uint64_t index = static_cast<std::uint64_t>(x) + static_cast<std::uint64_t>(length) * static_cast<std::uint64_t>(y);

        return index;
    };


    /// <summary>
    /// Normalizes a given value
    /// </summary>
    /// <param name="value"></param>
    /// <param name="min"></param>
    /// <param name="max"></param>
    /// <returns></returns>
    float Normalize(int value, int min, int max)
    {
        float normalized = (static_cast<float>(value) - static_cast<float>(min)) / (static_cast<float>(max) - static_cast<float>(min));

        return normalized;
    };

