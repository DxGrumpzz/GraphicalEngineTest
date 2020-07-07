#pragma once

#include "Graphics.hpp"
#include "Colour.hpp"


class Sprite
{

private:
    Graphics& _graphics;

public:
    unsigned int Width = 0;
    unsigned int Height = 0;

    Colour* Pixels = nullptr;
    size_t PixelCount = 0;

public:

    Sprite(Graphics& graphics) :
        _graphics(graphics)
    {
    };


public:

    void DrawSprite(int x, int y) const
    {
        for (size_t spriteX = 0; spriteX < Width; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < Height; spriteY++)
            {
                size_t pixelDataIndexer = spriteX + Width * spriteY;

                Colour& colour = Pixels[pixelDataIndexer];

                _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
            };
        };
    };


    void DrawSprite(int x, int y, int xOffset, int yOffset, int width, int height) const
    {

        for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
            {

                size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                Colour& colour = Pixels[pixelDataIndexer];

                _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
            };
        };

    };


    void DrawSpriteChromaKey(int x, int y, int xOffset, int yOffset, int width, int height, const Colour& chromaColour) const
    {

        for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
            {
                size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                Colour& colour = Pixels[pixelDataIndexer];

                if (colour == chromaColour)
                    continue;

                _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
            };
        };

    };


    void DrawSpriteColourChromaKey(int x, int y, int xOffset, int yOffset, int width, int height, const Colour& spriteolour, const Colour& chromaColour) const
    {

        for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
            {
                size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                Colour& colour = Pixels[pixelDataIndexer];

                if (colour == chromaColour)
                    continue;

                _graphics.DrawPixel(spriteX + x, spriteY + y, spriteolour, false);
            };
        };

    };


    void DrawSpriteScale(int x, int y, float scale)
    {
        DrawSpriteScale(x, y, scale, scale);
    };


    void DrawSpriteScale(int x, int y, float horizontalScale, float verticalScale)
    {
        if (horizontalScale < .0f ||
            verticalScale < .0f)
            return;


        if (horizontalScale > 1.f ||
            verticalScale > 1.f)
        {
            for (double spriteX = 0; spriteX < Width; spriteX++)
            {
                for (double spriteY = 0; spriteY < Height; spriteY++)
                {

                    for (size_t scaleX = 0; scaleX < horizontalScale; scaleX++)
                    {
                        for (size_t scaleY = 0; scaleY < verticalScale; scaleY++)
                        {

                            size_t pixelDataIndexer = spriteX + Width * spriteY;

                            Colour& colour = Pixels[pixelDataIndexer];

                            _graphics.DrawPixel(x + (spriteX * horizontalScale) + scaleX, y + (spriteY * verticalScale) + scaleY, colour, false);

                        };
                    };

                };
            };
        }
        else
        {
            for (double spriteX = 0; spriteX < Width; spriteX++)
            {
                for (double spriteY = 0; spriteY < Height; spriteY++)
                {
                    size_t pixelDataIndexer = spriteX + Width * spriteY;

                    Colour& colour = Pixels[pixelDataIndexer];

                    _graphics.DrawPixel(x + (spriteX * horizontalScale), y + (spriteY * verticalScale), colour, false);
                };
            };
        };
    };
};