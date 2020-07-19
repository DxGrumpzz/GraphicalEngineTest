#pragma once

#include <cmath>

#include "IScene.hpp"
#include "Vector2D.hpp"
#include "VectorTransformer.hpp"
#include "Maths.hpp"

class TestScene : public IScene
{

private:

    Graphics& _graphics;

    Window& _window;



public:

    TestScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window)
    {

    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
    };


    virtual void DrawScene() override
    {
        VectorTransformer transformer(_window);


    };
};