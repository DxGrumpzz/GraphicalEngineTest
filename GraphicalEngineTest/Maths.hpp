#pragma once

namespace Maths
{
    inline constexpr float PI = 3.14159265358;


    float DegreesToRadians(float degrees)
    {
        return degrees * (PI / 180);
    };
    
    float RadiansToDegrees(float radians)
    {
        return radians* (180 / PI);
    };


}