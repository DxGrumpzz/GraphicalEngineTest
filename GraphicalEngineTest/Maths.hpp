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
        int index = x + length * y;

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


    /// <summary>
    /// Takes a real number (of type TNormalzied), and converts it to an integer (of type TReturnNumber)
    /// </summary>
    /// <typeparam name="T"> The integer type to return </typeparam>
    /// <typeparam name="TNormalized"> The real number type to convert from </typeparam>
    /// <param name="normalizedNumber"></param>
    /// <param name="min"></param>
    /// <param name="max"></param>
    /// <returns></returns>
    template<class TReturn, class TNormalized>
    inline TReturn DenormalizeNumber(TNormalized normalizedNumber, TReturn min, TReturn max)
    {
        TReturn denormalized = normalizedNumber * (max - min) + min;

        return denormalized;
    };


};