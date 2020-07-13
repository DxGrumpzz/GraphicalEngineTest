#pragma once

#include <vector>

#include "IScene.hpp"
#include "Vector2D.hpp"
#include "Graphics.hpp"
#include "Sprite.hpp"
#include "Window.hpp"
#include "FontSheet.hpp"
#include "VectorTransformer.hpp"


class GraphScene : public IScene
{

private:
    std::vector<Vector2D> _points;

    Graphics& _graphics;
    Window& _window;


    FontSheet _font;


    float _horizontalScale = 1.0f;
    float _verticalScale = 1.0f;

    Vector2D _charPos = { 0, 0 };

public:

    GraphScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _font(graphics, 16, 28)
    {
        _font.LoadFromFile(L"Resources\\Fixedsys16x28.bmp");
    };


public:


    virtual void UpdateScene(float deltaTime) override
    {
        const Mouse& mouse = _window.GetMouse();
        const Keyboard& keyboard = _window.GetKeyboard();


        if (mouse.LeftMouseButton == KeyState::Held)
        {
            _charPos.X = mouse.X;
            _charPos.Y = mouse.Y;
        };


        if (keyboard.GetKeyState(VK_UP) == KeyState::Held)
        {
            _verticalScale -= 1.0f * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            _verticalScale += 1.f * deltaTime;
        };


        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Held)
        {
            _horizontalScale -= 1.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            _horizontalScale += 1.0F * deltaTime;
        };

    };

    Vector2D mouseVector = { 5, 5 };

    virtual void DrawScene() override
    {
        const Mouse& mouse = _window.GetMouse();

        VectorTransformer transformer(_window);

        if (mouse.LeftMouseButton == KeyState::Held)
            mouseVector = transformer.MouseToVector(mouse);


        DrawLine(transformer.CartesianToScreenSpace(1, 0), mouseVector);

        //_font.DrawString(_charPos.X, _charPos.Y, "0\n12\n3\n456\n78\n9");
    };


private:

    void DrawGraph()
    {


    };


    void DrawLine(const Vector2D p0, const Vector2D p1)
    {
        if (p0.X > p1.X)
        {
            float run = p0.X - p1.X;
            float rise = p0.Y - p1.Y;

            float slope = rise / run;

            float yIntercept = p0.Y - slope * p0.X;

            float y = 0;

            for (float x = p1.X; x < p0.X; x++)
            {
                y = slope * x + yIntercept;

                _graphics.DrawPixel(x, y, { 255,255,255 }, false);
            };
        }
        else
        {
            float run = p1.X - p0.X;
            float rise = p1.Y - p0.Y;

            float slope = rise / run;

            float yIntercept = p0.Y - slope * p0.X;

            float y = 0;

            for (float x = p0.X; x < p1.X; x++)
            {
                y = slope * x + yIntercept;

                _graphics.DrawPixel(x, y, { 255,255,255 }, false);
            };
        };

    };
};