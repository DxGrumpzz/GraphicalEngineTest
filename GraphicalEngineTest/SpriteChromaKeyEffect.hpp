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

    virtual Colour& ApplyEffect(std::size_t screenX, std::size_t screenY,
                                std::size_t spritePixelX, std::size_t spritePixelY,
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
