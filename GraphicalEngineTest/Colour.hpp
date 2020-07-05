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
};