#pragma once
#include <fstream>
#include <math.h>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"
#include "Sprite.hpp"

class BitmapScene : public IScene
{
private:


private:

    Graphics& _graphics;
    Window& _window;

    Sprite _sprite;

public:

    BitmapScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _sprite(graphics)
    {
        std::wstring bitmapPath = L"Resources\\Vikings_smallenemies_004.bmp";
        _sprite.LoadFromFile(bitmapPath);

    };

public:


    Vector2D p0 = { 0, 0 };
    Colour p0Colour = { 255, 0, 255, 0 };

    float alpha = .2f;

    Vector2D p1 = { 100, 100 };

    int pSize = 50;

    float horizontalScale = 1.0f;
    float verticalScale = 1.0f;



    void DrawP(const Vector2D& position, const Colour& colour)
    {
        for (int x = 0; x < pSize; x++)
        {
            for (int y = 0; y < pSize; y++)
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

        for (int x = 0; x < pSize; x++)
        {
            for (int y = 0; y < pSize; y++)
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


    virtual void UpdateScene(float deltaTime) override
    {
        Mouse mouse = _window.GetMouse();
        const Keyboard& keyboard = _window.GetKeyboard();

        if (mouse.LeftMouseButton == KeyState::Held)
        {
            p0.X = mouse.X;
            p0.Y = mouse.Y;
        };

        if (keyboard.GetKeyState(VK_UP) == KeyState::Held)
        {
            alpha += 0.5f * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            alpha -= 0.5f * deltaTime;
        }

        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Held)
        {
            horizontalScale -= 2.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            horizontalScale += 2.0F * deltaTime;
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


        DrawPTransparent(p0, p0Colour);
    };

};