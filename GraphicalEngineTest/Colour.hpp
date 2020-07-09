#pragma once
#include <cstdint>


/// <summary>
/// Simple pixel colour data structure
/// </summary>
struct Colour
{
    std::uint8_t Red;
    std::uint8_t Green;
    std::uint8_t Blue;

    std::uint8_t Alpha;


public:

    bool CompareNonAlpha(const Colour& colour)
    {
        if ((Red == colour.Red) &&
            (Green == colour.Green) &&
            (Blue == colour.Blue))
            return true;
        else
            return false;
    };

public:

    bool operator == (const Colour& colour)
    {
        if ((Red == colour.Red) &&
            (Green == colour.Green) &&
            (Blue == colour.Blue) &&
            (Alpha == colour.Alpha))
            return true;
        else
            return false;
    };


};