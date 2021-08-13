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

    /// <summary>
    /// A Graph point containing point position in x-y and a value
    /// </summary>
    struct GraphPoint
    {
        int X;
        int Y;

        int Value;
    };


private:


    Graphics& _graphics;
    Window& _window;

    /// <summary>
    /// A list of points visible on the graph
    /// </summary>
    std::vector<GraphPoint> _graphPoints;

    /// <summary>
    /// A list of points that lay on the graph's X axis
    /// </summary>
    std::vector<Vector2D> _graphXAxisPoints;

    FontSheet _font;

    Vector2D _graphPosition = { 20, (_window.GetWindowHeight() - 20) };

    int _pointWidth = 4;
    int _pointHeight = 4;

    /// <summary>
    /// Space between each graph point in X
    /// </summary>
    int _graphPointPadding = 20;


public:


    GraphScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),

        _font(graphics, 16, 28)
    {
        // Generate random number of points
        std::srand(static_cast<int>(std::time(0)));


        // Generate a graph point list
        CreateRandomGraphPoints();

        _graphXAxisPoints.resize(_graphPoints.size(), 0);

        // _font.LoadFromFile(L"Resources\\Fixedsys16x28.bmp");
    };


public:


    virtual void UpdateScene(float deltaTime) override
    {
        // Clear and generate new graph when user presses enter
        if (_window.GetKeyboard().GetKeyState(VK_RETURN) == KeyState::Pressed)
        {
            _graphXAxisPoints.clear();
            _graphPoints.clear();

            CreateRandomGraphPoints();

            _graphXAxisPoints.resize(_graphPoints.size(), 0);
        };


        if (_window.GetMouse().LeftMouseButton == KeyState::Held)
            _graphPosition = VectorTransformer(_window).MouseToVector(_window.GetMouse());
    };


    virtual void DrawScene() override
    {
        DrawLineAxes();

        DrawGraphLinePoints();

        DrawGraphPoints();

        DrawGraphPointLines();
    };


private:


    void DrawLineAxes()
    {
        // Draw a graph line in the X axis
        for (int x = 0; x < _window.GetWindowWidth(); x++)
        {
            _graphics.DrawPixel(x, static_cast<int>(_graphPosition.Y), Colours::White, false);
            _graphics.DrawPixel(x, static_cast<int>(_graphPosition.Y + 1), Colours::White, false);
        };

        // Draw a graph line in the Y axis
        for (int y = 0; y < _window.GetWindowHeight(); y++)
        {
            _graphics.DrawPixel(static_cast<int>(_graphPosition.X), y, Colours::White, false);
            _graphics.DrawPixel(static_cast<int>(_graphPosition.X + 1), y, Colours::White, false);
        };

    };


    /// <summary>
    /// Draw points on the graph showing the number of values
    /// </summary>
    void DrawGraphLinePoints()
    {
        // For every graph point 
        for (size_t a = 0; a < _graphXAxisPoints.size(); a++)
        {
            // Calculate x and y point relative to the graph's center
            int xPoint = (((_pointWidth * static_cast<int>(a))) + (_graphPointPadding * static_cast<int>(a)) + static_cast<int>(_graphPosition.X)) + _graphPointPadding;
            int yPoint = (_pointHeight + static_cast<int>(_graphPosition.Y)) - (_pointHeight + 1);

            _graphXAxisPoints[a] = Vector2D(static_cast<float>(xPoint), static_cast<float>(yPoint));

            // Draw the point
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


    /// <summary>
    /// Draw the actual points on the graph
    /// </summary>
    void DrawGraphPoints()
    {
        // For every point on the graph
        for (size_t a = 0; a < _graphPoints.size(); a++)
        {
            GraphPoint& graphPoint = _graphPoints[a];
            Vector2D& graphXLinePoint = _graphXAxisPoints[a];

            // Assign graph point values
            graphPoint.X = static_cast<int>(graphXLinePoint.X);

            // To get the correct position of the graph's Y value we need to translate back by the points value
            graphPoint.Y = static_cast<int>(graphXLinePoint.Y) - graphPoint.Value;


            // Draw the point as a solid rectangle going from the point's x-y to the bottom (y=0) of the graph
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


    /// <summary>
    /// Draw lines between every graph point
    /// </summary>
    void DrawGraphPointLines()
    {
        // For every graph point...
        for (size_t a = 0; a < _graphPoints.size() - 1; a++)
        {
            const GraphPoint& currentPoint = _graphPoints[a];
            const GraphPoint& nextPoint = _graphPoints[a + 1];

            // Draw a thicc line 
            for (int b = 0; b < (_pointWidth + _pointHeight) / 2; b++)
            {
                _graphics.DrawLine(Vector2D(static_cast<float>(currentPoint.X), static_cast<float>(currentPoint.Y + b)), Vector2D(static_cast<float>(nextPoint.X), static_cast<float>(nextPoint.Y + b)), Colours::Green, false);
            };
        };
    };


    /// <summary>
    /// Populates graph points list
    /// </summary>
    void CreateRandomGraphPoints()
    {
        int maxGraphPoints = 20;
        int minGraphPoints = 5;

        int numberOfGraphPoints = std::rand() % (maxGraphPoints - minGraphPoints + 1) + minGraphPoints;

        _graphPoints.reserve(numberOfGraphPoints);


        // Create the populate the graph points list
        for (size_t a = 0; a < numberOfGraphPoints; a++)
        {
            int value = std::rand() % 400 + 5;

            _graphPoints.push_back({ 0, 0, value });
        };
    };


};