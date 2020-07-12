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


    float Normalize(int val, int max, int min)
    {
        return (val - min) / (max - min);
    }


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


        _sprite.DrawSprite(_p0.X, _p0.Y,
                                     (_textureWidth * _textureX), (_textureHeight * _textureY),
                                     (_textureWidth * _textureX) + _textureWidth, (_textureHeight * _textureY) + _textureHeight,
                                     _horizontalScale,
                                     _verticalScale,
                                     {
                                         &SpriteTransparencyEffect(_graphics, _alpha),
                                         &SpriteChromaKeyEffect({ 0, 0, 0}, _sprite, _graphics),
                                     });

    };

};