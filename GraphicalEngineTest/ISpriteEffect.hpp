#pragma once
#include "Colour.hpp"


class ISpriteEffect
{

public:

    virtual Colour& ApplyEffect(int screenX, int screenY,
                                int spritePixelX, int spritePixelY,
                                Colour& pixel) = 0;

};