#pragma once
#include "Colour.hpp"


class ISpriteEffect
{

public:

    virtual Colour& ApplyEffect(std::size_t screenX, std::size_t screenY,
                                std::size_t spritePixelX, std::size_t spritePixelY,
                                Colour& pixel) = 0;

};