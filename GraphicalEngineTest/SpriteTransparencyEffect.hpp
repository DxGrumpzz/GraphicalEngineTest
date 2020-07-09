#pragma once

#include "ISpriteEffect.hpp"
#include "Graphics.hpp"


class SpriteTransparencyEffect : public ISpriteEffect
{

private:

    Graphics& _graphics;

public:
    float Alpha;


public:
    SpriteTransparencyEffect(
        Graphics& graphics,
        float alpha = 1.0f) :
        Alpha(alpha),

        _graphics(graphics)
    {
    };


public:

    virtual Colour& ApplyEffect(int screenX, int screenY,
                                int spritePixelX, int spritePixelY,
                                Colour& pixel) override
    {

        // Simple boundry check
        if (Alpha < 0.f)
            Alpha = 0.0001f;
        else if (Alpha > 1.f)
            Alpha = 1.0f;


        Colour& screenPixel = _graphics.GetPixel(screenX, screenY);

        pixel.Red = pixel.Red * Alpha + (screenPixel.Red + Alpha) * (1 - Alpha);
        pixel.Green = pixel.Green * Alpha + (screenPixel.Green + Alpha) * (1 - Alpha);
        pixel.Blue = pixel.Blue * Alpha + (screenPixel.Blue + Alpha) * (1 - Alpha);

        return pixel;
    };

};
