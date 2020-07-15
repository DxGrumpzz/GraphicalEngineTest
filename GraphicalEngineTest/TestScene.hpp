#pragma once

#include <cmath>

#include "IScene.hpp"
#include "Vector2D.hpp"
#include "VectorTransformer.hpp"


class TestScene : public IScene
{
private:

    const float PI = 22 / 7;


private:

    Graphics& _graphics;

    Window& _window;


    Vector2D _p0 = { 50, 50 };

    Vector2D _p1 = { 200, 50 };

    Vector2D _p3 = _p0;

public:

    TestScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window)
    {

    };


public:

    bool xDescending = false;
    bool yDescending = false;

    float xAccumualtor = 0.0f;

    float xIncrementSpeed = 24.0f;

    float yMax = 12.0f;


    virtual void UpdateScene(float deltaTime) override
    {
        float distance = _p1.X - _p0.X; 

        float p2x = xAccumualtor + _p0.X;
        float p2y = std::sin(((p2x - _p0.X) * PI) / distance) * 50 + _p0.Y;

        if (xAccumualtor < distance && xDescending == false)
        {
            xAccumualtor += xIncrementSpeed * deltaTime;
        }
        else
        {
            xAccumualtor -= xIncrementSpeed * deltaTime;

            if (xAccumualtor < 0)
                xDescending = false;
            else
                xDescending = true;
        };


        _p3.X = p2x;
        _p3.Y = p2y;
    };

    virtual void DrawScene() override
    {
        VectorTransformer transformer(_window);

        DrawPoint(transformer.CartesianVectorToScreenSpace(_p0));
        DrawPoint(transformer.CartesianVectorToScreenSpace(_p1));

        if (_window.GetMouse().LeftMouseButton == KeyState::Held)
            _p0 = transformer.MouseToCartesian(_window.GetMouse());

        DrawPoint(transformer.CartesianVectorToScreenSpace(_p3));
        //DrawPoint(transformer.CartesianVectorToScreenSpace(Vector2D(_p3.X + std::sin(accumualtor), _p3.Y)));
    };


    void DrawPoint(const Vector2D& point)
    {
        int pointWidth = 4;
        int pointHeight = 4;

        for (size_t x = 0; x < pointWidth; x++)
        {
            for (size_t y = 0; y < pointHeight; y++)
            {
                _graphics.DrawPixel(point.X + x, point.Y + y);
            };
        };

    }

};