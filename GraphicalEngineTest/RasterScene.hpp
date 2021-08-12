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
    float _degrees = 0.f;

public:

    RasterScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window)
    {
        _window.GetMouse().AddMouseWheelEventHandler([&](int delta)
        {
            if (delta > 0)
                _degrees += 1.0f;
            if (delta < 0)
                _degrees -= 1.0f;

        });

    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
        // _degrees += 100 * deltaTime;

        // if (_degrees >= 360.f)
        //     _degrees = 0.0f;
        // if (_degrees < 0.f)
        //     _degrees = 360.0f;



    };


    virtual void DrawScene() override
    {
        Colour colour { 255, 0, 0 };

        std::array<Vector2D, 3> points =
        {
            // Left
            Vector2D { -100, 0 },
            // Top
            Vector2D { 0, -150 },
            // Right
            Vector2D { 200, 0 },
        };

        for (auto& point : points)
            point.RotateDeg(_degrees);

        VectorTransformer vectorTransformer = VectorTransformer(_window);


        float height = points[1].Y - points[2].Y;

        if (points[0].Y == points[2].Y)
        {
            // Bottom facing triangle
            if (points[1].Y > 0)
            {
                float slope1 = (points[1].Y - points[0].Y) / (points[1].X - points[0].X);

                float b1 = points[1].Y - (slope1 * points[1].X);


                float slope2 = (points[1].Y - points[2].Y) / (points[1].X - points[2].X);

                float b2 = points[1].Y - (slope2 * points[1].X);


                auto f = [](float x, float slope, float b)
                {
                    return (slope * x) + b;
                };

                auto fInv = [](float y, float slope, float b)
                {
                    return (y - b) / slope;
                };


                for (int y = 0; y < height; y++)
                {
                    float beginX = fInv(y, slope1, b1);
                    float beginY = y;

                    float endX = fInv(y, slope2, b2);
                    float endY = y;


                    _graphics.DrawLine(vectorTransformer.CartesianVectorToScreenSpace({ beginX, beginY }), vectorTransformer.CartesianVectorToScreenSpace({ endX, endY }), Colours::White, false);
                };
            }
            // Top facing triangle
            else
            {
                float slope1 = (points[1].Y - points[0].Y) / (points[1].X - points[0].X);

                float b1 = points[1].Y - (slope1 * points[1].X);


                float slope2 = (points[1].Y - points[2].Y) / (points[1].X - points[2].X);

                float b2 = points[1].Y - (slope2 * points[1].X);


                auto f = [](float x, float slope, float b)
                {
                    return (slope * x) + b;
                };

                auto fInv = [](float y, float slope, float b)
                {
                    return (y - b) / slope;
                };


                for (int y = 0; y >= height; y--)
                {
                    float beginX = fInv(y, slope1, b1);
                    float beginY = y;

                    float endX = fInv(y, slope2, b2);
                    float endY = y;

                    _graphics.DrawLine(vectorTransformer.CartesianVectorToScreenSpace({ beginX, beginY }), vectorTransformer.CartesianVectorToScreenSpace({ endX, endY }), Colours::White, false);
                };
            }
        }


        // Outline 
        for (std::size_t index = 0; index < points.size() - 1; index++)
        {
            _graphics.DrawLine(vectorTransformer.CartesianVectorToScreenSpace(points[index]), vectorTransformer.CartesianVectorToScreenSpace(points[index + 1]), colour, false);
        };

        _graphics.DrawLine(vectorTransformer.CartesianVectorToScreenSpace(*(points.end() - 1)), vectorTransformer.CartesianVectorToScreenSpace(*points.begin()), colour, false);

    };
};