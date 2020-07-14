#pragma once

#include <vector>
#include <ctime>
#include <cstdlib>

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


    struct GraphPoint
    {
        int X;
        int Y;

        int Value;

    };


private:


    Graphics& _graphics;
    Window& _window;

    std::vector<GraphPoint> _graphPoints;

    FontSheet _font;

    Vector2D _graphPosition = { 20, _window.GetWindowHeight() - 20 };

    int _pointWidth = 4;
    int _pointHeight = 4;

    std::vector<Vector2D> _graphXAxisPoints;


    int _graphPointPadding = 20;


public:


    GraphScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),

        _font(graphics, 16, 28)
    {
        CreateGraphPoints();

        _graphXAxisPoints.resize(_graphPoints.size(), 0);

        _font.LoadFromFile(L"Resources\\Fixedsys16x28.bmp");
    };


public:


    virtual void UpdateScene(float deltaTime) override
    {
        if (_window.GetKeyboard().GetKeyState(VK_RETURN) == KeyState::Pressed)
        {
            _graphXAxisPoints.clear();
            _graphPoints.clear();

            CreateGraphPoints();

            _graphXAxisPoints.resize(_graphPoints.size(), 0);

        };

    };


    virtual void DrawScene() override
    {
        if (_window.GetMouse().LeftMouseButton == KeyState::Held)
            _graphPosition = VectorTransformer(_window).MouseToVector(_window.GetMouse());

        DrawLineAxes();

        DrawGraphLinePoints();

        DrawGraphPoints();

        DrawGraphPointLines();
    };


private:


    void DrawLineAxes()
    {
        for (int x = 0; x < _window.GetWindowWidth(); x++)
        {
            _graphics.DrawPixel(x, _graphPosition.Y);
            _graphics.DrawPixel(x, _graphPosition.Y + 1);
        };

        for (int y = 0; y < _window.GetWindowHeight(); y++)
        {
            _graphics.DrawPixel(_graphPosition.X, y);
            _graphics.DrawPixel(_graphPosition.X + 1, y);
        };

    };


    void DrawGraphLinePoints()
    {
        for (size_t a = 0; a < _graphXAxisPoints.size(); a++)
        {
            int xPoint = (((_pointWidth * a)) + (_graphPointPadding * a) + _graphPosition.X) + _graphPointPadding;
            int yPoint = (_pointHeight + _graphPosition.Y) - (_pointHeight + 1);

            _graphXAxisPoints[a] = Vector2D(xPoint, yPoint);

            for (int x = 0; x < _pointWidth; x++)
            {
                for (int y = 0; y < _pointHeight; y++)
                {
                    _graphics.DrawPixel(xPoint + x,
                                        yPoint + y,
                                        Colours::Red, false);
                };
            };

        };
    };


    void DrawGraphPoints()
    {
        for (size_t a = 0; a < _graphPoints.size(); a++)
        {
            GraphPoint& graphPoint = _graphPoints[a];
            Vector2D& graphXPoint = _graphXAxisPoints[a];

            graphPoint.X = graphXPoint.X;
            graphPoint.Y = graphXPoint.Y - graphPoint.Value;


            for (int x = 0; x < _pointWidth; x++)
            {
                for (int y = 0; y < graphPoint.Value; y++)
                {
                    _graphics.DrawPixel(graphPoint.X + x,
                                        graphPoint.Y + y,
                                        Colours::Green, false);
                };
            };
        };
    };


    void DrawGraphPointLines()
    {
        for (size_t a = 0; a < _graphPoints.size() - 1; a++)
        {
            const GraphPoint& currentPoint = _graphPoints[a];
            const GraphPoint& nextPoint = _graphPoints[a + 1];
           

            for (int b = 0; b < 4; b++)
            {
                DrawLine(Vector2D(currentPoint.X, currentPoint.Y) + b, Vector2D(nextPoint.X, nextPoint.Y) + b, Colours::Green, false);
            };
        };
    };


    void CreateGraphPoints()
    {
        std::srand(std::time(0));

        int numberOfGraphPoints = std::rand() % 20;

        _graphPoints.reserve(numberOfGraphPoints);


        for (size_t a = 0; a < numberOfGraphPoints; a++)
        {
            int value = std::rand() % 400 + 5;

            _graphPoints.push_back({ 0, 0, value });
        };
    };


    /// <summary>
    /// Draw a line segment between 2 points
    /// </summary>
    /// <param name="p0"></param>
    /// <param name="p1"></param>
    /// <param name="colour"></param>
    void DrawLine(Vector2D p0, Vector2D p1, Colour colour = { 255,255,255 }, bool checkBounds = true)
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

                    _graphics.DrawPixel(x, y, colour, checkBounds);
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

                    _graphics.DrawPixel(x, y, colour, checkBounds);
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

                    _graphics.DrawPixel(x, y, colour, checkBounds);
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

                    _graphics.DrawPixel(x, y, colour, checkBounds);
                };
            };
        };

    };


};