#pragma once

#include "ISpriteEffect.hpp"
#include "Colour.hpp"
#include "Sprite.hpp"
#include "Graphics.hpp"


class SpriteChromaKeyEffect : public ISpriteEffect
{

private:

    Colour _chromaKey;
    const Sprite& _sprite;
    Graphics& _graphics;

public:
    SpriteChromaKeyEffect(const Colour& chromaKey,
                          const Sprite& sprite,
                          Graphics& graphics) :
        _chromaKey(chromaKey),
        _sprite(sprite),
        _graphics(graphics)
    {
    };


public:

    virtual Colour& ApplyEffect(int screenX, int screenY,
                                int spritePixelX, int spritePixelY,
                                Colour& pixel) override
    {
        Colour spritePixel = _sprite.GetPixel(spritePixelX, spritePixelY);

        if (spritePixel.CompareNonAlpha(_chromaKey))
        {
            Colour& screenPixel = _graphics.GetPixel(screenX, screenY);
            pixel = screenPixel;
        };

        return pixel;
    };

};
