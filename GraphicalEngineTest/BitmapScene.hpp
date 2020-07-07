#pragma once
#include <fstream>

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

    float horizontalScale = 1.0f;
    float verticalScale = 1.0f;


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
            verticalScale -= 2.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            verticalScale += 2.0F * deltaTime;
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
        _sprite.DrawSpriteScale(p0.X, p0.Y, horizontalScale, verticalScale);
         //_sprite.DrawSpriteScale(p0.X, p0.Y, verticalScale);
    };

};