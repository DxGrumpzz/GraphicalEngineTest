#include <Array>

#include "IScene.hpp"
#include "Window.hpp"
#include "Graphics.hpp"
#include "VectorTransformer.hpp"
#include "Vector2D.hpp"
#include "FontSheet.hpp"


class RasterScene : public IScene
{
private:

    Graphics& _graphics;
    Window& _window;
    float _degrees = 0.f;
    float _degreesScalar = 50.f;
    VectorTransformer _vectorTransformer;

    FontSheet _fontSheet;


    std::array<Vector2D, 3> _points =
    {
        // Top
        Vector2D { -146.0f, 98.0f },
        // Right
        Vector2D { 100.0f, 0.0f },
        // Left
        Vector2D { -100.0f, 0.0f },
    };


public:

    RasterScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _vectorTransformer(VectorTransformer(_window)),
        _fontSheet(_graphics, 13, 24)
    {
        _fontSheet.LoadFromFile(L"Resources\\Consolas13x24.bmp");

        _window.GetMouse().AddMouseWheelEventHandler([&](int delta)
        {
            if (delta > 0)
                _degreesScalar += 1.0f;
            if (delta < 0)
                _degreesScalar -= 1.0f;

            if (_degreesScalar < 0.f)
                _degreesScalar = 0.0f;
        });
    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
        if (_window.GetKeyboard().GetKeyState(VK_LEFT) == KeyState::Held)
        {
            _degrees += _degreesScalar * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            _degrees -= _degreesScalar * deltaTime;
        };
    };


    void DrawTopFacingTriangle(const Vector2D& top, const Vector2D& right, const Vector2D& left)
    {
        float height = top.Y - left.Y;

        // Run / Rise
        if ((top.X == left.X) ||
            (top.X == right.X))
        {
            float slope1 = (top.X - left.X) / (top.Y - left.Y);

            float b1 = left.X - (slope1 * left.X);

            
            float slope2 = (top.X - right.X) / (top.Y - right.Y);

            float b2 = top.X - (slope2 * top.Y);


            auto f = [](float y, float slope, float b)
            {
                return (y * slope) + b;
            };

            auto fInv = [](float x, float slope, float b)
            {
                return (x - b) / slope;
            };


            int deltaY = -1;
            int yBegin = top.Y;
            int yEnd = left.Y;

            int deltaX = 1;

            for (int y = yBegin; y != yEnd; y += deltaY)
            {
                int xBegin = f(y, slope1, b1);
                int xEnd = f(y, slope2, b2);

                if (xBegin > xEnd)
                    deltaX = -1;
                else
                    deltaX = 1;

                for (int x = xBegin; x != xEnd; x += deltaX)
                {
                    _graphics.DrawPixel(_vectorTransformer.CartesianToScreenSpace(x, y), Colours::White, false);
                };
            };
        }
        // Rise / Run
        else
        {
            float slope1 = (top.Y - left.Y) / (top.X - left.X);

            float b1 = left.Y - (slope1 * left.X);


            float slope2 = (top.Y - right.Y) / (top.X - right.X);

            float b2 = top.Y - (slope2 * top.X);


            auto f = [](float x, float slope, float b)
            {
                return (slope * x) + b;
            };

            auto fInv = [](float y, float slope, float b)
            {
                return (y - b) / slope;
            };


            int deltaY = -1;
            int yBegin = top.Y;
            int yEnd = left.Y;

            int deltaX = -1;
            // if (top.X < right.X)
            //     deltaX = -1;
            // else
            //     deltaX = 1;

            for (int y = yBegin; y > yEnd; y += deltaY)
            {
                int beginX = fInv(y, slope1, b1);
                int endX = fInv(y, slope2, b2);

                if (beginX > endX)
                    deltaX = -1;
                else
                    deltaX = 1;

                for (int x = beginX; x != endX; x += deltaX)
                {
                    _graphics.DrawPixel(_vectorTransformer.CartesianVectorToScreenSpace({ x, y }), Colours::White, false);
                };
            };
        };
    };


    void DrawBottomFacingTriangle(const std::array<Vector2D, 3>& points)
    {
        float height = points[1].Y - points[2].Y;

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

            for (int x = beginX; x > endX; x--)
            {
                _graphics.DrawPixel(_vectorTransformer.CartesianVectorToScreenSpace({ x, y }), Colours::White, false);
            };
        };
    };


    virtual void DrawScene() override
    {
        Colour colour { 255, 0, 0 };

        for (auto& point : _points)
            point.RotateDeg(_degrees);


        if (std::roundf(_points[0].Y) == std::roundf(_points[1].Y))
        {
            // Top facing triangle
            if (_points[2].Y > 0)
            {
                DrawTopFacingTriangle(_points[0], _points[1], _points[2]);
            }
            // Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle({ _points[1], _points[2], _points[0] });
            };
        };

        if (std::roundf(_points[0].Y) == std::roundf(_points[2].Y))
        {
            // Top facing triangle
            if (_points[1].Y > 0)
            {
                // DrawTopFacingTriangle(_points);
                DrawTopFacingTriangle(_points[0], _points[1], _points[2]);
            }
            // Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle(_points);
            };
        };

        if (std::roundf(_points[1].Y) == std::roundf(_points[2].Y))
        {
            // Top facing triangle
            if (_points[0].Y > 0)
            {
                // DrawTopFacingTriangle(_points);
                DrawTopFacingTriangle(_points[0], _points[1], _points[2]);
            }
            // Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle(_points);
            };
        };



        // Outline 
        for (std::size_t index = 0; index < _points.size() - 1; index++)
        {
            _graphics.DrawLine(_vectorTransformer.CartesianVectorToScreenSpace(_points[index]), _vectorTransformer.CartesianVectorToScreenSpace(_points[index + 1]), colour, false);
        };

        _graphics.DrawLine(_vectorTransformer.CartesianVectorToScreenSpace(*(_points.end() - 1)), _vectorTransformer.CartesianVectorToScreenSpace(*_points.begin()), colour, false);


        _fontSheet.DrawString(_vectorTransformer.CartesianVectorToScreenSpace(_points[0]), "p0", 0.7f);
        _fontSheet.DrawString(_vectorTransformer.CartesianVectorToScreenSpace(_points[1]), "p1", 0.7f);
        _fontSheet.DrawString(_vectorTransformer.CartesianVectorToScreenSpace(_points[2]), "p2", 0.7f);



        int margin = 5;

        for (Vector2D& point : _points)
        {
            auto mousePositionCartesian = _vectorTransformer.MouseToCartesian(_window.GetMouse());


            for (int x = point.X - margin; x < point.X + margin; x++)
            {
                for (int y = point.Y - margin; y < point.Y + margin; y++)
                {

                    if ((mousePositionCartesian.X == x) &&
                        mousePositionCartesian.Y == y)
                    {
                        for (int _x = point.X - margin; _x < point.X + margin; _x++)
                            for (int _y = point.Y - margin; _y < point.Y + margin; _y++)
                                _graphics.DrawPixel(_vectorTransformer.CartesianToScreenSpace(_x, _y), Colours::Magenta, false);


                        if (_window.GetMouse().LeftMouseButton == KeyState::Held)
                            point = mousePositionCartesian;


                        _fontSheet.DrawString(_vectorTransformer.CartesianToScreenSpace(mousePositionCartesian.X, mousePositionCartesian.Y + margin),
                                              std::string("(")
                                              .append(std::to_string(static_cast<int>(point.X)))
                                              .append(",")
                                              .append(std::to_string(static_cast<int>(point.Y)))
                                              .append(")"),
                                              0.7);
                    };
                };
            };


        };

    };
};