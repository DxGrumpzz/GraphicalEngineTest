#pragma once

#include "Sprite.hpp"
#include "Vector2D.hpp"
#include "Graphics.hpp"


/// <summary>
/// A warpper class for the Sprite, used to mainly draw stings from bitmaps
/// </summary>
class FontSheet
{

private:

    Graphics& _graphics;

    Sprite _sprite;

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


public:

    FontSheet(Graphics& graphics,
              int glyphWidh,
              int glyphHeight) :
        _graphics(graphics),

        _sprite(graphics),

        _glyphWidth(glyphWidh),
        _glyphHeight(glyphHeight)
    {
    };


public:

    /// <summary>
    /// Load a font bitmap from a file
    /// </summary>
    /// <param name="spriteFile"></param>
    void LoadFromFile(const std::wstring& spriteFile)
    {
        _sprite.LoadFromFile(spriteFile);

        // Calculate the number of charater columns and rows
        _numberOfColumns = _sprite.Width / _glyphWidth;
        _numberOfRows = _sprite.Height / _glyphHeight;
    };


    /// <summary>
    /// Draw a string somewhere on screen
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="text"> The text to draw </param>
    void DrawString(int x, int y, const std::string& text)
    {
        // Starting X position used when going down a line to restore original position
        int startingX = x;

        // A somewhat deceiving name, 
        // Used to to calculate the "jump" forward when drawing a character
        int characterCounter = 0;

        for (size_t a = 0; a < text.size(); a++)
        {
            char currentChar = text[a];

            // Check if current character is a newline
            if (currentChar == '\n')
            {
                // Reset X position
                x = startingX;

                // Move down a "line"
                y += _glyphHeight;

                // Reset character counter
                characterCounter = 0;

                // Move to next iteration so we don't draw the newline character
                continue;
            };

            // Draw the string
            DrawChar(x + (characterCounter * _glyphWidth), y,
                     currentChar);

            characterCounter++;
        };
    };


    /// <summary>
    /// Draw a string somewhere on screen, and scale 
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="text"> The text to draw </param>
    void DrawString(int x, int y,
                    const std::string& text,
                    float horizontalScale, float verticalScale)
    {
        // Don't bother scaling if bitmap is too smol to see
        if (horizontalScale < 0.f ||
            verticalScale < 0.f)
            return;

        // Drawing scaled string from a bitmap is a little different to drawing regular scaled sprites,
        // We need to accomodate for the translation that occurs between each character

        if (horizontalScale > 1.f ||
            verticalScale > 1.f)
        {
            int startingX = x;
            int characterCounter = 0;

            for (size_t a = 0; a < text.size(); a++)
            {
                char currentChar = text[a];

                if (currentChar == '\n')
                {
                    x = startingX;
                    y += (_glyphHeight * static_cast<int>(verticalScale));

                    characterCounter = 0;
                    continue;
                };


                Vector2D characterPos = GetCharacterPos(currentChar);

                float x0 = _glyphWidth * characterPos.X;
                float y0 = _glyphHeight * characterPos.Y;

                float x1 = (_glyphWidth * characterPos.X) + _glyphWidth;
                float y1 = (_glyphHeight * characterPos.Y) + _glyphHeight;


                for (int spriteX = 0; spriteX < x1 - x0; spriteX++)
                {
                    for (int spriteY = 0; spriteY < y1 - y0; spriteY++)
                    {

                        // To accomodate loss in number of pixels drawn, iterate through the scalars and draw "skipped" pixels
                        for (double scaleX = 0; scaleX < horizontalScale; scaleX++)
                        {
                            for (double scaleY = 0; scaleY < verticalScale; scaleY++)
                            {
                                Colour spritePixel = _sprite.GetPixel(static_cast<std::size_t>(spriteX) + static_cast<std::size_t>(x0), 
                                                                      static_cast<std::size_t>(spriteY) + static_cast<std::size_t>(y0));

                                _graphics.DrawPixel(static_cast<int>((x + (spriteX * horizontalScale) + scaleX) + (characterCounter * _glyphWidth) * horizontalScale), 
                                                    static_cast<int>(y + (spriteY * verticalScale) + scaleY), 
                                                    spritePixel, false);
                            };
                        };

                    };
                };

                characterCounter++;
            };
        }
        else
        {
            int startingX = x;
            int characterCounter = 0;

            for (size_t a = 0; a < text.size(); a++)
            {
                char currentChar = text[a];

                if (currentChar == '\n')
                {
                    x = startingX;
                    y += (_glyphHeight * static_cast<int>(verticalScale));

                    characterCounter = 0;
                    continue;
                };

                Vector2D characterPos = GetCharacterPos(currentChar);

                float x0 = _glyphWidth * characterPos.X;
                float y0 = _glyphHeight * characterPos.Y;

                float x1 = (_glyphWidth * characterPos.X) + _glyphWidth;
                float y1 = (_glyphHeight * characterPos.Y) + _glyphHeight;


                for (double spriteX = 0; spriteX < static_cast<double>(x1) - static_cast<double>(x0); spriteX++)
                {
                    for (double spriteY = 0; spriteY < static_cast<double>(y1) - static_cast<double>(y0); spriteY++)
                    {
                        Colour spritePixel = _sprite.GetPixel(static_cast<std::size_t>(spriteX + x0), 
                                                              static_cast<std::size_t>(spriteY + y0));

                        _graphics.DrawPixel(static_cast<int>(x + (spriteX * horizontalScale) + (characterCounter * _glyphWidth) * horizontalScale), 
                                            static_cast<int>(y + (spriteY * verticalScale)), 
                                            spritePixel, false);
                    };
                };

                characterCounter++;
            };

        };
    };

    /// <summary>
    /// Draw a single character
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="character"></param>
    void DrawChar(int x, int y, char character)
    {
        // Find character position relative to sprite
        Vector2D characterPos = GetCharacterPos(character);

        _sprite.DrawSprite(x, y,
                           _glyphWidth * static_cast<int>(characterPos.X), _glyphHeight * static_cast<int>(characterPos.Y),
                           (_glyphWidth * static_cast<int>(characterPos.X)) + _glyphWidth, (_glyphHeight * static_cast<int>(characterPos.Y)) + _glyphHeight);
    };


    /// <summary>
    /// Draw a single character, and scale
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="character"></param>
    /// <param name="horizontalScale"></param>
    /// <param name="verticalScale"></param>
    void DrawChar(int x, int y,
                  char character,
                  float horizontalScale, float verticalScale)
    {
        // Find character position relative to sprite
        Vector2D characterPos = GetCharacterPos(character);

        _sprite.DrawSprite(x, y,
                           _glyphWidth * static_cast<int>(characterPos.X), _glyphHeight * static_cast<int>(characterPos.Y),
                           (_glyphWidth * static_cast<int>(characterPos.X)) + _glyphWidth, (_glyphHeight * static_cast<int>(characterPos.Y)) + _glyphHeight,
                           horizontalScale,
                           verticalScale);
    };

private:

    /// <summary>
    /// Find position of an English character on the bitmap
    /// </summary>
    /// <param name="character"></param>
    /// <returns></returns>
    Vector2D GetCharacterPos(char character)
    {
        // I wish I understood why this works

        int glyphIndex = character - ' ';

        int glpyhX = glyphIndex % _numberOfColumns;
        int glpyhY = glyphIndex / _numberOfColumns;

        return { static_cast<float>(glpyhX), static_cast<float>(glpyhY) };
    };

};