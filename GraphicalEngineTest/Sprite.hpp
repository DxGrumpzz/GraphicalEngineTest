#pragma once
#include <string>

#include "Graphics.hpp"
#include "Colour.hpp"


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
        PixelCount = Height * Width;
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

    /// <summary>
    /// Draw a segment from the sprite
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="xOffset"> The segment's starting offset in X axis </param>
    /// <param name="yOffset"> The segment's starting offset in Y axis </param>
    /// <param name="width"> The segment's end offset in X axis </param>
    /// <param name="height"> The segment's end offset in Y axis </param>
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


    /// <summary>
    /// Draw a segmented sprite with a chroma key 
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="xOffset"> The segment's starting offset in X axis </param>
    /// <param name="yOffset"> The segment's starting offset in Y axis </param>
    /// <param name="width"> The segment's end offset in X axis </param>
    /// <param name="height"> The segment's end offset in Y axis </param>
    /// <param name="chromaColour"> The colour/chroma key to ignore </param>
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

    /// <summary>
    /// Draw a sprite entirely and scale
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="scale"> The scaling factor </param>
    void DrawSpriteScale(int x, int y, float scale) const
    {
        DrawSpriteScale(x, y, scale, scale);
    };


    /// <summary>
    /// Draw a sprite entirely and scale vertically or horizontally
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="horizontalScale"> The horizontal scale factor </param>
    /// <param name="verticalScale"> The horizontal scale factor  </param>
    void DrawSpriteScale(int x, int y, float horizontalScale, float verticalScale) const
    {
        // Scaling is hella expensive, don't draw if scale is tool small to be seen 
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