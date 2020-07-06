#pragma once

#include "Vector2D.hpp"
#include "Mouse.hpp"

class VectorTransformer
{
private:

    int _consoleWindowWidth;
    int _consoleWindowHeight;


public:

    VectorTransformer(int consoleWindowWidth, int consoleWindowHeight) :
        _consoleWindowWidth(consoleWindowWidth),
        _consoleWindowHeight(consoleWindowHeight)
    {
    };

    VectorTransformer(const Window& window) :
        _consoleWindowWidth(window.GetWindowWidth()),
        _consoleWindowHeight(window.GetWindowHeight())
    {
    };

public:

    Vector2D CartesianToScreenSpace(float x, float y)
    {
        float screenSpaceX = x + (_consoleWindowWidth / 2);
        float screenSpaceY = (-y) + (_consoleWindowHeight / 2);

        return { screenSpaceX, screenSpaceY };
    };

    Vector2D CartesianVectorToScreenSpace(const Vector2D& vector)
    {
        return CartesianToScreenSpace(vector.X, vector.Y);
    };

    Vector2D NDCToScreenSpace(const Vector2D& vector)
    {
        return { ((vector.X + 1.0f) * ((_consoleWindowWidth) / 2)),  ((-vector.Y + 1.0f) * ((_consoleWindowHeight) / 2)) };
    };


    Vector2D MouseToVector(short x, short y)
    {
        return { static_cast<float>(x), static_cast<float>(y) };
    };

    Vector2D MouseToVector(const Mouse& mouse)
    {
        return { static_cast<float>(mouse.X), static_cast<float>(mouse.Y) };
    };


    Vector2D MouseToCartesian(short x, short y)
    {
         // return { static_cast<float>(x) - _consoleWindowWidth / 2, -(static_cast<float>(y) - _consoleWindowHeight / 2) };
        return { static_cast<float>(x) - (_consoleWindowWidth / 2), (static_cast<float>(-y)) + (_consoleWindowHeight / 2) };
    };
    
    Vector2D MouseToCartesian(const Mouse& mouse)
    {
        // return { static_cast<float>(x) - _consoleWindowWidth / 2, -(static_cast<float>(y) - _consoleWindowHeight / 2) };
        return { static_cast<float>(mouse.X) - (_consoleWindowWidth / 2), (static_cast<float>(-mouse.Y)) + (_consoleWindowHeight / 2) };
    };

};