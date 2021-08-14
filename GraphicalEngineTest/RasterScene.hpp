#include <Array>
#include <algorithm>

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
    float _degreesScalar = 1.f;
    VectorTransformer _vectorTransformer;

    FontSheet _fontSheet;


    std::array<Vector2D, 3> _points =
    {
        // Top   / bottom
        Vector2D { 0.0f, 300.0f },
        // Right / left 
        Vector2D { 400.0f, -300.0f },
        // Left  / right
        Vector2D { -400.0f, -300.0f },
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
                _degrees += 0.1f;
            if (delta < 0)
                _degrees -= 0.1f;
        });
    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
    };


    void DrawTopFacingTriangle(const Vector2D& top, const Vector2D& right, const Vector2D& left, const Colour& colour = Colours::White)
    {
        float height = top.Y - left.Y;

        // Run / Rise
        if ((top.X == left.X) ||
            (top.X == right.X))

        {
            float slope1 = (top.X - left.X) / (top.Y - left.Y);
            float b1 = left.X - (slope1 * left.Y);


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
                    _graphics.DrawPixel(_vectorTransformer.CartesianToScreenSpace(x, y), colour, false);
                };
            };
        }
        // Rise / Run
        else
        {
            float slope1 = (top.Y - left.Y) / (top.X - left.X);

            float b1 = top.Y - (slope1 * top.X);


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
                    _graphics.DrawPixel(_vectorTransformer.CartesianVectorToScreenSpace({ x, y }), colour, false);
                };
            };
        };
    };


    void DrawBottomFacingTriangle(const Vector2D& bottom, const Vector2D& left, const Vector2D& right, const Colour& colour = Colours::White)
    {
        float height = left.Y - bottom.Y;

        // Run / Rise
        if ((bottom.X == left.X) ||
            (bottom.X == right.X))
        {
            float slope1 = (bottom.X - left.X) / (bottom.Y - left.Y);
            float b1 = bottom.X - (slope1 * bottom.Y);


            float slope2 = (bottom.X - right.X) / (bottom.Y - right.Y);
            float b2 = bottom.X - (slope2 * bottom.Y);


            auto f = [](float y, float slope, float b)
            {
                return (y * slope) + b;
            };

            auto fInv = [](float x, float slope, float b)
            {
                return (x - b) / slope;
            };


            int deltaY = -1;
            int yBegin = left.Y;
            int yEnd = bottom.Y;

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
                    _graphics.DrawPixel(_vectorTransformer.CartesianToScreenSpace(x, y), colour, false);
                };
            };
        }
        // Rise / Run
        else
        {
            float slope1 = (bottom.Y - left.Y) / (bottom.X - left.X);

            float b1 = left.Y - (slope1 * left.X);


            float slope2 = (bottom.Y - right.Y) / (bottom.X - right.X);

            float b2 = bottom.Y - (slope2 * bottom.X);


            auto f = [](float x, float slope, float b)
            {
                return (slope * x) + b;
            };

            auto fInv = [](float y, float slope, float b)
            {
                return (y - b) / slope;
            };


            int deltaY = -1;
            int yBegin = left.Y;
            int yEnd = bottom.Y;

            int deltaX = 1;
            // if (top.X < right.X)
            //     deltaX = -1;
            // else
            //     deltaX = 1;

            for (int y = yBegin; y != yEnd; y += deltaY)
            {
                int beginX = fInv(y, slope1, b1);
                int endX = fInv(y, slope2, b2);

                if (beginX > endX)
                    deltaX = -1;
                else
                    deltaX = 1;

                for (int x = beginX; x != endX; x += deltaX)
                {
                    _graphics.DrawPixel(_vectorTransformer.CartesianVectorToScreenSpace({ x, y }), colour, false);
                };
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
            bool topFacing = _points[2].Y > _points[0].Y;

            // Top facing triangle
            if (topFacing == true)
            {
                DrawTopFacingTriangle(_points[2], _points[0], _points[1]);
            }
             // Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle(_points[2], _points[0], _points[1]);
            };
        }
        else if (std::roundf(_points[0].Y) == std::roundf(_points[2].Y))
        {
            bool topFacing = _points[1].Y > _points[0].Y;

            //  Top facing triangle
            if (topFacing == true)
            {
                DrawTopFacingTriangle(_points[1], _points[0], _points[2]);
            }
            //  Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle(_points[1], _points[0], _points[2]);
            };

        }
        else if (std::roundf(_points[1].Y) == std::roundf(_points[2].Y))
        {
            bool topFacing = _points[0].Y > _points[1].Y;

             // Top facing triangle
            if (topFacing == true)
            {
                DrawTopFacingTriangle(_points[0], _points[1], _points[2]);
            }
             // Bottom facing triangle
            else
            {
                DrawBottomFacingTriangle(_points[0], _points[1], _points[2]);
            };
        }
        else
        {
            const Vector2D& topPoint = *std::max_element(_points.begin(), _points.end(),
                                                         [](const Vector2D& point1, const Vector2D& point2)
            {
                return point2.Y > point1.Y;
            });

            const Vector2D& bottomPoint = *std::max_element(_points.begin(), _points.end(),
                                                            [](const Vector2D& point1, const Vector2D& point2)
            {
                return point1.Y > point2.Y;
            });

            Vector2D const* midPoint = nullptr;


            if ((topPoint == _points[0]) &&
                (bottomPoint == _points[1]))
            {
                midPoint = &_points[2];
            };

            if ((topPoint == _points[0]) &&
                (bottomPoint == _points[2]))
            {
                midPoint = &_points[1];
            };



            if ((topPoint == _points[1]) &&
                (bottomPoint == _points[0]))
            {
                midPoint = &_points[2];
            };

            if ((topPoint == _points[1]) &&
                (bottomPoint == _points[2]))
            {
                midPoint = &_points[0];
            };



            if ((topPoint == _points[2]) &&
                (bottomPoint == _points[0]))
            {
                midPoint = &_points[1];
            };

            if ((topPoint == _points[2]) &&
                (bottomPoint == _points[1]))
            {
                midPoint = &_points[0];
            };


            if (midPoint == nullptr)
                return;


            // Run / Rise
            if (bottomPoint.X == topPoint.X)
            {
                float s1 = (topPoint.X - bottomPoint.X) / (topPoint.Y - bottomPoint.Y);
                float b1 = topPoint.X - (s1 * topPoint.Y);

                int beginX = (s1 * midPoint->Y) + b1;
                int endX = midPoint->X;


                DrawTopFacingTriangle(topPoint, { static_cast<float>(beginX), midPoint->Y }, *midPoint);
                DrawBottomFacingTriangle(bottomPoint, { static_cast<float>(beginX), midPoint->Y }, *midPoint);


                _graphics.DrawLine(_vectorTransformer.CartesianVectorToScreenSpace({ static_cast<float>(beginX), midPoint->Y }), 
                                   _vectorTransformer.CartesianVectorToScreenSpace(*midPoint), 
                                   Colours::Red, false);
            }
            // Rise / Run
            else
            {
                float s1 = (topPoint.Y - bottomPoint.Y) / (topPoint.X - bottomPoint.X);
                float b1 = topPoint.Y - (s1 * topPoint.X);

                int beginX = midPoint->X;
                int endX = (midPoint->Y - b1) / s1;


                DrawTopFacingTriangle(topPoint, { static_cast<float>(endX), midPoint->Y }, *midPoint);
                DrawBottomFacingTriangle(bottomPoint, { static_cast<float>(endX), midPoint->Y }, *midPoint);



                _graphics.DrawLine(_vectorTransformer.CartesianVectorToScreenSpace({ static_cast<float>(endX), midPoint->Y }),
                                   _vectorTransformer.CartesianVectorToScreenSpace(*midPoint),
                                   Colours::Red, false);
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
                                              0.7f);
                    };
                };
            };


        };

    };
};