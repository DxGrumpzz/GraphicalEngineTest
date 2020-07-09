#pragma once
#include <fstream>
#include <math.h>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"
#include "Sprite.hpp"

#include "ISpriteEffect.hpp"
#include "SpriteChromaKeyEffect.hpp"
#include "SpriteTransparencyEffect.hpp"


class BitmapScene : public IScene
{
private:

    Graphics& _graphics;
    Window& _window;

    Sprite _sprite;



    Vector2D _p0 = { 0, 0 };
    Colour _p0Colour = { 255, 0, 255, 255 };

    float _alpha = 1.0f;

    Vector2D _p1 = { 100, 100 };

    int _pSize = 50;

    float _horizontalScale = 1.0f;
    float _verticalScale = 1.0f;



    int _textureX = 7;
    int _textureY = 12;

    int _textureWidth = 54;
    int _textureHeight = 49;

public:

    BitmapScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _sprite(graphics)
    {
        std::wstring bitmapPath = L"Resources\\0001.bmp";
        //std::wstring bitmapPath = L"Resources\\0003.bmp";
        // std::wstring bitmapPath = L"Resources\\Vikings_smallenemies_004.bmp";

        _sprite.LoadFromFile(bitmapPath);
    };


public:


    void DrawP(const Vector2D& position, const Colour& colour)
    {
        for (int x = 0; x < _pSize; x++)
        {
            for (int y = 0; y < _pSize; y++)
            {
                _graphics.DrawPixel(position.X + x, position.Y + y, colour);
            };
        };
    };

    float Normalize(int val, int max, int min)
    {
        return (val - min) / (max - min);
    }


    void DrawPTransparent(const Vector2D& position, const Colour& colour)
    {
        float normalizedAlpha = Normalize(colour.Alpha, 255, 0);

        DrawPTransparentNormalized(position, colour, normalizedAlpha);
    };


    void DrawPTransparentNormalized(const Vector2D& position, const Colour& colour, float alpha)
    {
        std::size_t windowWidth = _window.GetWindowWidth();
        Colour* pixels = _graphics.GetPixels();

        // Simple boundry check 
        if (alpha < 0.f)
            alpha = 0.0001f;
        else if (alpha > 1.f)
            alpha = 1.0f;

        for (int x = 0; x < _pSize; x++)
        {
            for (int y = 0; y < _pSize; y++)
            {
                std::size_t pixelIndex = (position.X + x) + windowWidth * (position.Y + y);

                const Colour& pixel = pixels[pixelIndex];

                Colour blend = { 0 };

                blend.Red = colour.Red * alpha + pixel.Red * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);
                blend.Green = colour.Green * alpha + pixel.Green * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);
                blend.Blue = colour.Blue * alpha + pixel.Blue * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);

                _graphics.DrawPixel(position.X + x, position.Y + y, blend);
            };
        };
    };


    void DrawSpriteTransparent(int x, int y, const Sprite& _sprite, float alpha)
    {
        std::size_t windowWidth = _window.GetWindowWidth();
        Colour* screenPixels = _graphics.GetPixels();

        // Simple boundry check 
        if (alpha < 0.f)
            alpha = 0.0001f;
        else if (alpha > 1.f)
            alpha = 1.0f;

        for (size_t spriteX = 0; spriteX < _sprite.Width; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < _sprite.Height; spriteY++)
            {
                size_t spritePixelIndex = spriteX + _sprite.Width * spriteY;
                std::size_t screenPixelIndex = (spriteX + x) + windowWidth * (spriteY + y);

                Colour& spritePixel = _sprite.Pixels[spritePixelIndex];
                const Colour& screenPixel = screenPixels[screenPixelIndex];

                Colour blend = { 0 };

                blend.Red = spritePixel.Red * alpha + screenPixel.Red * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);
                blend.Green = spritePixel.Green * alpha + screenPixel.Green * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);
                blend.Blue = spritePixel.Blue * alpha + screenPixel.Blue * alpha * (1 - alpha) / alpha + alpha * (1 - alpha);

                _graphics.DrawPixel(spriteX + x, spriteY + y, blend, false);
            };
        };
    };




    void DrawSpriteWithEffects(int x, int y, const Sprite& sprite, std::vector<ISpriteEffect*> effects)
    {

        for (size_t spriteX = 0; spriteX < sprite.Width; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < sprite.Height; spriteY++)
            {
                Colour spritePixel = sprite.GetPixel(spriteX, spriteY);


                for (ISpriteEffect* effect : effects)
                {
                    effect->ApplyEffect(spriteX + x, spriteY + y,
                                        spriteX, spriteY,
                                        spritePixel);
                };


                _graphics.DrawPixel(spriteX + x, spriteY + y, spritePixel, false);
            };
        };
    };



    void DrawSpriteWithEffects(int x, int y,
                               int x0, int y0,
                               int x1, int y1,
                               const Sprite& sprite,
                               const std::vector<ISpriteEffect*>& effects)
    {

        for (size_t spriteX = 0; spriteX < x1 - x0; spriteX++)
        {
            for (size_t spriteY = 0; spriteY < y1 - y0; spriteY++)
            {
                Colour spritePixel = sprite.GetPixel((spriteX + x0), (spriteY + y0));


                for (ISpriteEffect* effect : effects)
                {
                    effect->ApplyEffect(spriteX + x, spriteY + y,
                                        (spriteX + x0), (spriteY + y0),
                                        spritePixel);
                };


                _graphics.DrawPixel(spriteX + x, spriteY + y, spritePixel, false);
            };
        };
    };



    /// <summary>
    /// Draw a sprite entirely and scale
    /// </summary>
    /// <param name="x"> X position to start drawing from </param>
    /// <param name="y"> Y position to start drawing from </param>
    /// <param name="scale"> The scaling factor </param>
    void DrawSpriteWithEffect(int x, int y,
                              int x0, int y0,
                              int x1, int y1,
                              float horizontalScale,
                              float verticalScale,
                              const Sprite& sprite,
                              const std::vector<ISpriteEffect*>& effects)
    {
        // Scaling is hella expensive, don't draw if scale is tool small to be seen 
        if (horizontalScale < .0f ||
            verticalScale < .0f)
            return;

        if (horizontalScale > 1.f ||
            verticalScale > 1.f)
        {
            for (double spriteX = 0; spriteX < x1 - x0; spriteX++)
            {
                for (double spriteY = 0; spriteY < y1 - y0; spriteY++)
                {

                    for (double scaleX = 0; scaleX < horizontalScale; scaleX++)
                    {
                        for (double scaleY = 0; scaleY < verticalScale; scaleY++)
                        {
                            Colour spritePixel = sprite.GetPixel((spriteX + x0), (spriteY + y0));


                            for (ISpriteEffect* effect : effects)
                            {
                                effect->ApplyEffect(x + (spriteX * horizontalScale) + scaleX, y + (spriteY * verticalScale) + scaleY,
                                                    (spriteX + x0), (spriteY + y0),
                                                    spritePixel);
                            };


                            _graphics.DrawPixel(x + (spriteX * horizontalScale) + scaleX, y + (spriteY * verticalScale) + scaleY, spritePixel, false);
                        };
                    };

                };
            };
        }
        else
        {
            for (double spriteX = 0; spriteX < x1 - x0; spriteX++)
            {
                for (double spriteY = 0; spriteY < y1 - y0; spriteY++)
                {
                    Colour spritePixel = sprite.GetPixel((spriteX + x0), (spriteY + y0));

                    for (ISpriteEffect* effect : effects)
                    {
                        effect->ApplyEffect(x + (spriteX * horizontalScale), y + (spriteY * verticalScale),
                                            (spriteX + x0), (spriteY + y0),
                                            spritePixel);
                    };

                    _graphics.DrawPixel(x + (spriteX * horizontalScale), y + (spriteY * verticalScale), spritePixel, false);
                };
            };
        };
    };

    void DrawSpriteWithEffect(int x, int y,
                              int xOffset, int yOffset,
                              int width, int height,
                              float scale,
                              const Sprite& sprite,
                              const std::vector<ISpriteEffect*>& effects)
    {
        DrawSpriteWithEffect(x, y, xOffset, yOffset, width, height, scale, scale, sprite, effects);
    };


    virtual void UpdateScene(float deltaTime) override
    {
        Mouse mouse = _window.GetMouse();
        const Keyboard& keyboard = _window.GetKeyboard();

        if (mouse.LeftMouseButton == KeyState::Held)
        {
            _p0.X = mouse.X;
            _p0.Y = mouse.Y;
        };

        if (keyboard.GetKeyState(VK_UP) == KeyState::Held)
        {
            // _alpha += 0.5f * deltaTime; 
            // _textureY--;
            _verticalScale -= 1.0f * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            // _alpha -= 0.5f * deltaTime;
            // _textureY++;
            _verticalScale += 1.f * deltaTime;
        }

        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Held)
        {
            _horizontalScale -= 1.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            _horizontalScale += 1.0F * deltaTime;
        };


        if (keyboard.GetKeyState(VK_NUMPAD8) == KeyState::Held)
        {
            _alpha += 1.0f * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_NUMPAD2) == KeyState::Held)
        {
            _alpha -= 1.0f * deltaTime;
        };

    };



    virtual void DrawScene() override
    {
        Colour colour = { 0 };
        for (int x = 0; x < 255; x++)
        {
            for (int y = 0; y < 255; y++)
            {
                _graphics.DrawPixel(50 + x, 50 + y, colour);
                if (colour.Red < 254)
                {
                    colour.Red++;
                }
                else if (colour.Green < 254)
                {
                    colour.Red = 0;
                    colour.Green++;
                }
                else if (colour.Blue < 254)
                {
                    colour.Green = 0;
                    colour.Blue++;
                };

            };
        };


        /*
        DrawSpriteWithEffects(_p0.X, _p0.Y,
                              _sprite,
                              {
                                  &SpriteTransparencyEffect(_graphics, _alpha),
                                  &SpriteChromaKeyEffect({ 255, 0, 255}, _sprite, _graphics),
                              });
        */

        /*
        DrawSpriteWithEffects(_p0.X, _p0.Y,
                               (_textureWidth * _textureX), (_textureHeight * _textureY),
                               (_textureWidth * _textureX) + _textureWidth, (_textureHeight * _textureY) + _textureHeight,
                               _sprite,
                               {
                                   &SpriteTransparencyEffect(_graphics, _alpha),
                                   &SpriteChromaKeyEffect({ 0, 0, 0 }, _sprite, _graphics),
                               });
        */


        DrawSpriteWithEffect(_p0.X, _p0.Y,
                             (_textureWidth * _textureX), (_textureHeight * _textureY),
                             (_textureWidth * _textureX) + _textureWidth, (_textureHeight * _textureY) + _textureHeight,
                             //_horizontalScale,
                             _verticalScale,
                             _sprite,
                             {
                                 &SpriteTransparencyEffect(_graphics, _alpha),
                                 &SpriteChromaKeyEffect({ 0, 0, 0 }, _sprite, _graphics),
                             });

    };

};