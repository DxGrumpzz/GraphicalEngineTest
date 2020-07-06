#pragma once

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"

class BitmapScene : public IScene
{
private:

    Graphics& _graphics;
    Window& _window;

public:

    BitmapScene(Graphics& graphics, Window& window) :
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

    };

};