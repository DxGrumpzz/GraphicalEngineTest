#pragma once
#include <string>

#include "Graphics.hpp"
#include "Colour.hpp"
#include "ISpriteEffect.hpp"


/// <summary>
/// A class responsible for drawing sprites on screen
/// </summary>
class Sprite
{

private:

    /// <summary>
    /// The graphics "engine" used to render the sprite
    /// </summary>
    Graphics& _graphics;

public:

    /// <summary>
    /// Width of the sprite
    /// </summary>
    unsigned int Width = 0;

    /// <summary>
    /// Sprite height
    /// </summary>
    unsigned int Height = 0;

    /// <summary>
    /// The sprite's pixels
    /// </summary>
    Colour* Pixels = nullptr;

    /// <summary>
    /// The number of pixels available
    /// </summary>
    size_t PixelCount = 0;

public:

    Sprite(Graphics& graphics) :
        _graphics(graphics)
    {
    };


public:


    /// <summary>
    /// Load a sprite from a file.
    /// Notice: only uncompressed, 32 bit, and 24 bit, bitmaps are supported.
    /// </summary>
    /// <param name="spriteFile"></param>
    void LoadFromFile(const std::wstring& spriteFile)
    {
        std::ifstream file(spriteFile,
                           // Read/open the file in binary mode (handles \n\r characters differently, and more)
                           std::ios::binary);

        if (file.bad() == true)
        {
            throw std::exception("Unable to open file");
        };

        // Get bitmap header
        BITMAPFILEHEADER bitmapFileHeader = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapFileHeader), sizeof(bitmapFileHeader));

        // Get bitmap info
        BITMAPINFOHEADER bitmapInfo = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapInfo), sizeof(bitmapInfo));


        // Bitmap validation, currently only non compressed, 32, and 24 bit, bitmaps are supported
        if (bitmapInfo.biCompression != BI_RGB)
            throw std::exception("Unsupported compression type");

        if (bitmapInfo.biBitCount != 32 &&
            (bitmapInfo.biBitCount != 24))
            throw std::exception("Unsupported bit-per-pixel count");


        // If height is negative the bitmap is read top to bottom
        // Otherwise bottom to top (default)
        bool isUpsideDown = bitmapInfo.biHeight < 0;

        int beginY = 0;
        int endY = 0;

        int deltaY = 0;

        if (isUpsideDown == true)
        {
            Height = -bitmapInfo.biHeight;
            beginY = 0;
            endY = Height;
            deltaY = 1;
        }
        else
        {
            Height = bitmapInfo.biHeight;
            beginY = Height - 1;
            endY = -1;
            deltaY = -1;
        };


        // Calculate bitmap row padding (If it's 24-bit bitmap )
        int bytesPerPixel = bitmapInfo.biBitCount / 8;
        int padding = (4 - (bitmapInfo.biWidth * bytesPerPixel) % 4) % 4;

        Width = bitmapInfo.biWidth;

        // Create the sprite pixels 
        PixelCount = static_cast<std::size_t>(Height) * static_cast<std::size_t>(Width);
        Pixels = new Colour[PixelCount];

        memset(Pixels, 0, PixelCount * sizeof(Colour));


        // Read bitmap 
        for (long long y = beginY; y != endY; y += deltaY)
        {
            for (std::size_t x = 0; x < bitmapInfo.biWidth; x++)
            {
                // Find pixel position 
                std::size_t pixelPosition = x + bitmapInfo.biWidth * y;


                Colour pixel = { 0 };

                pixel.Blue = file.get();
                pixel.Green = file.get();
                pixel.Red = file.get();

                // If the bitmap is 32 bit
                if (bitmapInfo.biBitCount == 32)
                    // Read alpha
                    pixel.Alpha = file.get();

                // Add pixel to sprite
                Pixels[pixelPosition] = pixel;
            };

            // If the bitmap isn't 32 bit there should usually be row padding,
            // so move reader-head to next the row
            if (bitmapInfo.biBitCount != 32)
                file.seekg(padding, std::ios::cur);
        };
    };



    /// <summary>
    /// Draw the sprite entirely
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="effects"> effect(s) to apply to the sprite draw call </param>
    void DrawSprite(int x, int y, std::vector<ISpriteEffect*> effects = { })
    {
        // Iterate through the entirety of the sprite's pixels
        for (size_t spriteX = 0; spriteX < Width; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < Height; spriteY++)
            {
                Colour spritePixel = GetPixel(spriteX, spriteY);

                // Apply effects
                ApplyEffects(spriteX + x, spriteY + y,
                             spriteX, spriteY,
                             spritePixel,
                             effects);

                // Draw the pixel
                _graphics.DrawPixel(static_cast<int>(spriteX + x), static_cast<int>(spriteY + y), spritePixel, false);
            };
        };
    };



    /// <summary>
    /// Draw a segment from the sprite
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="x0"> The segment's starting offset in X axis </param>
    /// <param name="y0"> The segment's starting offset in Y axis </param>
    /// <param name="x1"> The segment's end offset in X axis </param>
    /// <param name="y1"> The segment's end offset in Y axis </param>
    /// <param name="effects"> effect(s) to apply to the sprite draw call </param>
    void DrawSprite(int x, int y,
                    int x0, int y0,
                    int x1, int y1,
                    const std::vector<ISpriteEffect*>& effects = {})
    {
        // Iterate throught the sprite's pixels based on x0, y0, x1, y1 offsets
        for (int spriteX = 0; spriteX < static_cast<std::size_t>(x1) - static_cast<std::size_t>(x0); spriteX++)
        {
            for (int spriteY = 0; spriteY < static_cast<std::size_t>(y1) - static_cast<std::size_t>(y0); spriteY++)
            {
                // Get a copy of the pixel offsetted by beggining offsets
                Colour spritePixel = GetPixel(static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x0),
                                              static_cast<std::size_t>(spriteY) + static_cast<std::size_t>(y0));

                ApplyEffects(static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x),
                             static_cast<std::size_t>(spriteY) + static_cast<std::size_t> (y),
                             static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x0),
                             static_cast<std::size_t>(spriteY) + static_cast<std::size_t>(y0),
                             spritePixel,
                             effects);

                _graphics.DrawPixel(static_cast<int>(spriteX + x), static_cast<int>(spriteY + y), spritePixel, false);
            };
        };
    };




    /// <summary>
    /// Draw a sprite entirely and scale vertically or horizontally
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="horizontalScale"> The horizontal scale factor </param>
    /// <param name="verticalScale"> The horizontal scale factor  </param>
    void DrawSprite(int x, int y,
                    int x0, int y0,
                    int x1, int y1,
                    float horizontalScale,
                    float verticalScale,
                    const std::vector<ISpriteEffect*>& effects = { })
    {
        // Scaling is hella expensive, don't draw if scale is tool small to be seen 
        if (horizontalScale < 0.f ||
            verticalScale < 0.f)
            return;

        // If either the horizontal or vertical scalars are greater than 1. percent ...
        if (horizontalScale > 1.f ||
            verticalScale > 1.f)
        {
            // Iterate through the sprite segment
            for (int spriteX = 0; spriteX < x1 - x0; spriteX++)
            {
                for (int spriteY = 0; spriteY < y1 - y0; spriteY++)
                {

                    // To accomodate loss in number of pixels drawn, iterate through the scalars and draw "skipped" pixels
                    for (float scaleX = 0; scaleX < horizontalScale; scaleX++)
                    {
                        for (float scaleY = 0; scaleY < verticalScale; scaleY++)
                        {
                            Colour spritePixel = GetPixel(static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x0), 
                                                          static_cast<std::size_t>(spriteY) + static_cast<std::size_t>(y0));


                            ApplyEffects(x + (spriteX * horizontalScale) + scaleX,
                                         y + (spriteY * verticalScale) + scaleY,
                                         static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x0),
                                         static_cast<std::size_t>(spriteY) + static_cast<std::size_t>(y0),
                                         spritePixel,
                                         effects);


                            _graphics.DrawPixel(static_cast<int>(x + (spriteX * horizontalScale) + scaleX),
                                                static_cast<int>(y + (spriteY * verticalScale) + scaleY),
                                                spritePixel, false);
                        };
                    };

                };
            };
        }
        else
        {
            for (double spriteX = 0; spriteX < static_cast<std::size_t>(x1) - static_cast<std::size_t>(x0); spriteX++)
            {
                for (double spriteY = 0; spriteY < static_cast<std::size_t>(y1) - static_cast<std::size_t>(y0); spriteY++)
                {
                    Colour spritePixel = GetPixel(static_cast<std::size_t>(spriteX + x0), static_cast<std::size_t>(spriteY + y0));


                    ApplyEffects(static_cast<std::size_t>(x + (spriteX * horizontalScale)),
                                 static_cast<std::size_t>(y + (spriteY * verticalScale)),
                                 static_cast<std::size_t>(spriteX + x0),
                                 static_cast<std::size_t>(spriteY + y0),
                                 spritePixel,
                                 effects);


                    _graphics.DrawPixel(static_cast<int>(x + (spriteX * horizontalScale)),
                                        static_cast<int>(y + (spriteY * verticalScale)), spritePixel, false);
                };
            };
        };
    };



    void DrawSprite(int x, int y,
                    int xOffset, int yOffset,
                    int width, int height,
                    float scale,
                    const std::vector<ISpriteEffect*>& effects = { })
    {
        DrawSprite(x, y, xOffset, yOffset, width, height, scale, scale, effects);
    };



public:

    Colour& GetPixel(std::size_t x, std::size_t y) const
    {
        size_t pixelDataIndexer = x + Width * y;

        Colour& pixel = Pixels[pixelDataIndexer];

        return pixel;
    };


private:

    /// <summary>
    /// Takes a list of effects and applys them to a pixel, Order matters
    /// </summary>
    /// <param name="screenX"> Position of the sprite's pixel in the X axis relative to screen </param>
    /// <param name="screenY"> Position of the sprite's pixel in the Y axis relative to screen </param>
    /// <param name="spriteX"> Position of the sprite's pixel in the X axis</param>
    /// <param name="spriteY"> Position of the sprite's pixel in the Y axis</param>
    /// <param name="spritePixel"> The pixel to affect </param>
    /// <param name="effects"> The list of effects </param>
    void ApplyEffects(std::size_t screenX, std::size_t screenY,
                      std::size_t spriteX, std::size_t spriteY,
                      Colour& spritePixel,
                      const std::vector<ISpriteEffect*>& effects)
    {
        // Check if effects available
        if (effects.size() != 0)
        {
            // Apply desired effects
            for (ISpriteEffect* effect : effects)
            {
                effect->ApplyEffect(screenX, screenY,
                                    spriteX, spriteY,
                                    spritePixel);
            };
        };
    };

};