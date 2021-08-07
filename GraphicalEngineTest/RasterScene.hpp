#include <Array>

#include "IScene.hpp"
#include "Window.hpp"
#include "Graphics.hpp"
#include "VectorTransformer.hpp"
#include "Vector2D.hpp"
#include <thread>


class RasterScene : public IScene
{
private:

    Graphics& _graphics;
    Window& _window;
    float _degrees = 0.0f;

public:

    RasterScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window)
    {
        _window.GetMouse().AddMouseWheelEventHandler([&](int delta)
        {
            if(delta > 0)
                _degrees += 1;
            if (delta < 0)
                _degrees -= 1;

        });

    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
        // _degrees += 100 * deltaTime;

        if (_degrees >= 360.f)
            _degrees = 0.0f;
        if (_degrees < 0.f)
            _degrees = 360.0f;

    };


    virtual void DrawScene() override
    {

        Colour colour { 255, 0, 0 };

        std::array<Vector2D, 3> points =
        {
            // Left
            Vector2D { -0.5f, 0.0f },
            // Top
            Vector2D { 0.0f, 0.5f },
            // Right
            Vector2D { 0.5f, 0.0f },
        };

        for (auto& point : points)
        point.RotateDeg(_degrees);



        VectorTransformer vectorTransformer = VectorTransformer(_window);


        for (std::size_t index = 0; index < points.size() - 1; index++)
        {
            _graphics.DrawLine(vectorTransformer.NDCToScreenSpace(points[index]), vectorTransformer.NDCToScreenSpace(points[index + 1]), colour, false);
        };

        _graphics.DrawLine(vectorTransformer.NDCToScreenSpace(*(points.end() - 1)), vectorTransformer.NDCToScreenSpace(*points.begin()), colour, false);



        float slope1 = (points[0].Y - points[1].Y) / ((points[0].X - points[1].X));
        float b1 = points[0].Y - (slope1 * points[0].X);



        float slope2 = (points[1].Y - points[2].Y) / ((points[1].X - points[2].X));
        float b2 = points[1].Y - (slope1 * points[1].X);



        for (float x2 = points[1].X; x2 < points[2].X; x2 += 0.001f)
        {
            float y2 = (slope2 * x2) + b2;

            auto screenCoords2 = vectorTransformer.NDCToScreenSpace({ x2, y2 });

            _graphics.DrawPixel(screenCoords2.X, screenCoords2.Y, Colours::Yellow, false);


            float xBeing = points[0].X;
            float yBegin = points[0].Y;

            float xEnd = x2;
            float yEnd = y2;

            _graphics.DrawLine(vectorTransformer.NDCToScreenSpace({ xBeing, yBegin }), vectorTransformer.NDCToScreenSpace({ xEnd, yEnd }), Colours::White, false);

        };



    };
};