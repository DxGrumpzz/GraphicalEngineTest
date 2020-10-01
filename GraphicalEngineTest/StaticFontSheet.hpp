#pragma once
#include <cstdint>
#include <string>

#include "Sprite.hpp"
#include "Vector2D.hpp"

#include "Graphics.hpp"

struct StaticBitmap
{
    Colour* PixelBuffer = nullptr;

    std::size_t PixelBufferLength = 0;

    std::size_t PixelBufferWidth = 0;
    std::size_t PixelBufferHeight = 0;

};


/// <summary>
/// A class that will generate static Font sheets as a pixel buffers
/// </summary>
class StaticFontSheet
{
private:

    /// <summary>
    /// The width of a single character
    /// </summary>
    int _glyphWidth;

    /// <summary>
    /// The height of a single character
    /// </summary>
    int _glyphHeight;


    /// <summary>
    /// The number of character coloumns in the bitmap
    /// </summary>
    int _numberOfColumns = 0;

    /// <summary>
    /// The number of character rows in the bitmap
    /// </summary>
    int _numberOfRows = 0;

    Sprite _sprite;

    Graphics& _graphics;

public:

    StaticFontSheet(const std::wstring& spriteFile,
                    class Graphics& graphics,
                    int glyphWidh,
                    int glyphHeight) :
        _glyphWidth(glyphWidh),
        _glyphHeight(glyphHeight),

        _sprite(graphics),

        _graphics(graphics)
    {
        _sprite.LoadFromFile(spriteFile);

        // Calculate the number of charater columns and rows
        _numberOfColumns = _sprite.Width / _glyphWidth;
        _numberOfRows = _sprite.Height / _glyphHeight;
    };


public:

    StaticBitmap GenerateString(const std::string& string)
    {
        int rows = 1;
        int width = 0;
        int tempWidth = 0;

        for (const wchar_t& currentChar : string)
        {
            if (currentChar == L'\n')
            {
                if (tempWidth > width)
                    width = tempWidth;

                tempWidth = 0;
                rows++;
            };
            tempWidth++;
        };



        const int bufferWidth = width * _glyphWidth;
        const int bufferHeight = rows * _glyphHeight;

        Colour* pixelBuffer = new Colour[bufferWidth * bufferHeight] { 0 };

        int aTemp = 0;
        int line = 0;

        for (int a = 0; a < string.size(); a++)
        {
            const char& character = string[a];
            const std::pair<int, int>& characterPos = GetCharacterPos(character);

            if (character == L'\n')
            {
                aTemp = 0;
                line++;
                continue;
            };

            for (int x = 0; x < _glyphWidth; x++)
            {
                for (int y = 0; y < _glyphHeight; y++)
                {
                    const int pixelBufferIndex = Maths::Convert2DTo1D(x + (_glyphWidth * aTemp),
                                                                      y + (_glyphHeight * line), bufferWidth);

                    const int spriteIndex = Maths::Convert2DTo1D(x + (_glyphWidth * characterPos.first),
                                                                 y + (_glyphHeight * characterPos.second),
                                                                 _sprite.Width);

                    pixelBuffer[pixelBufferIndex] = _sprite.GetPixel(spriteIndex);
                };
            };

            aTemp++;
        };

        StaticBitmap staticBitmap = { 0 };
        staticBitmap.PixelBuffer = pixelBuffer;

        staticBitmap.PixelBufferWidth = bufferWidth;
        staticBitmap.PixelBufferHeight = bufferHeight;

        staticBitmap.PixelBufferLength = staticBitmap.PixelBufferWidth * staticBitmap.PixelBufferHeight;

        return  staticBitmap;
    };



    Colour* GenerateChar(char character)
    {
        Colour* pixelBuffer = new Colour[_glyphWidth * _glyphHeight] { 0 };

        const std::pair<int, int>& characterPos = GetCharacterPos(character);


        for (int x = 0; x < _glyphWidth; x++)
        {
            for (int y = 0; y < _glyphHeight; y++)
            {
                const int pixelBufferIndex = Maths::Convert2DTo1D(x, y, _glyphWidth);

                const int spriteIndex = Maths::Convert2DTo1D(x + (_glyphWidth * characterPos.first),
                                                             y + (_glyphHeight * characterPos.second),
                                                             _sprite.Width);

                pixelBuffer[pixelBufferIndex] = _sprite.GetPixel(spriteIndex);
            };
        };

        return pixelBuffer;
    };

private:

    std::pair<int, int> GetCharacterPos(char character)
    {
        // I wish I understood why this works

        int glyphIndex = character - ' ';

        int glpyhX = glyphIndex % _numberOfColumns;
        int glpyhY = glyphIndex / _numberOfColumns;

        return { glpyhX, glpyhY };
    };
};