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
        //DrawGraph();

        const Mouse& mouse = _window.GetMouse();

        VectorTransformer transformer(_window);

        if (mouse.LeftMouseButton == KeyState::Held)
            mouseVector = transformer.MouseToVector(mouse);


        // DrawLine(transformer.CartesianToScreenSpace(2, 0), transformer.CartesianToScreenSpace(6, 10));
        DrawLine(transformer.CartesianToScreenSpace(2, 0), mouseVector);

        //_font.DrawString(_charPos.X, _charPos.Y, "0\n12\n3\n456\n78\n9");
    };


private:

    void DrawGraph()
    {
        for (int x = 0; x < _window.GetWindowWidth(); x++)
        {
            _graphics.DrawPixel(x, _window.GetWindowHeight() / 2);
        };

        for (int y = 0; y < _window.GetWindowHeight(); y++)
        {
            _graphics.DrawPixel(_window.GetWindowWidth() / 2, y);
        };

    };

    /// <summary>
    /// Draw a line segment between 2 points
    /// </summary>
    /// <param name="p0"></param>
    /// <param name="p1"></param>
    /// <param name="colour"></param>
    void DrawLine(Vector2D p0, Vector2D p1, Colour colour = { 255,255,255 })
    {
        // Left "leaning" line
        if (p0.X >= p1.X)
        {
            float run = p0.X - p1.X;
            float rise = p0.Y - p1.Y;

            // If the rise starts getting larger than the run the line starts to break up, 
            // so we check when that happens and use "inverses" to draw the new line
            // Instead of y-Intercept we use x-Intercept, and replace x's with y's positions
            if (std::abs(rise) > std::abs(run))
            {
                float slope = run / rise;

                float xIntercept = p0.X - slope * p0.Y;

                if (p0.Y > p1.Y)
                    std::swap(p0, p1);
                
                float x = 0;

                for (float y = p0.Y; y < p1.Y; y++)
                {
                    x = slope * y + xIntercept;

                    _graphics.DrawPixel(x, y, colour, false);
                };
            }
            else
            {
                float slope = rise / run;

                float yIntercept = p0.Y - slope * p0.X;

                float y = 0;

                for (float x = p1.X; x < p0.X; x++)
                {
                    y = slope * x + yIntercept;

                    _graphics.DrawPixel(x, y, colour, false);
                };
            };
        }
        // Right "leaning" line
        else
        {
            float run = p1.X - p0.X;
            float rise = p1.Y - p0.Y;

            // If the rise starts getting larger than the run the line starts to break up, 
            // so we check when that happens and use "inverses" to draw the new line
            // Instead of y-Intercept we use x-Intercept, and replace x's with y's positions
            if (std::abs(rise) > std::abs(run))
            {
                float slope = run / rise;

                float xIntercept = p0.X - slope * p0.Y;

                if (p0.Y > p1.Y)
                    std::swap(p0, p1);

                float x = 0;

                for (float y = p0.Y; y < p1.Y; y++)
                {
                    x = slope * y + xIntercept;

                    _graphics.DrawPixel(x, y, colour, false);
                };
            }
            else
            {
                float slope = rise / run;

                float yIntercept = p0.Y - slope * p0.X;

                float y = 0;

                for (float x = p0.X; x < p1.X; x++)
                {
                    y = slope * x + yIntercept;

                    _graphics.DrawPixel(x, y, colour, false);
                };
            };
        };

    };

};