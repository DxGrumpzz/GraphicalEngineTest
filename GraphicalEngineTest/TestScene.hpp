#pragma once

#include <cmath>
#include <random>

#include "IScene.hpp"
#include "Vector2D.hpp"
#include "VectorTransformer.hpp"
#include "Maths.hpp"


class TestScene : public IScene
{

private:

    Graphics& _graphics;

    Window& _window;

    float _playerX = 4.0f;
    float _playerY = 4.0f;

    float _playerLookAtAngle = 0.0f;

    int _mapWidth = 10;
    int _mapHeight = 10;

    float _playerFOV = Maths::DegreesToRadians(90);

    float _maxDepth = 10;

    bool* _map;

    std::function<void(int, int)> _rawMouseMovedHandler;

public:

    TestScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _rawMouseMovedHandler([this](int x, int y)
                              {
                                  MouseRawMoved(x, y);
                              })
    {

        _window.AddRawMouseMovedHandler(_rawMouseMovedHandler);

        _map = new bool[_mapWidth * _mapHeight]
        {
            1,1,1,1,1,1,1,1,1,1,
            1,0,0,0,0,0,0,0,0,1,
            1,0,1,1,0,0,0,0,0,1,
            1,0,1,1,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,1,
            1,0,0,1,0,0,0,0,0,1,
            1,0,0,1,1,0,0,0,0,1,
            1,0,0,0,0,0,0,0,0,1,
            1,1,1,1,1,1,1,1,1,1,
        };

    };

    ~TestScene()
    {
        delete[] _map;
        _map = nullptr;
    };

public:

    bool cursorConfined = false;
    float _deltaTime;

    void MouseRawMoved(int x, int y)
    {
        if (cursorConfined == true)
        {
            _playerLookAtAngle += (x * _deltaTime) * 0.05;
        };
    };


    virtual void UpdateScene(float deltaTime) override
    {
        _deltaTime = deltaTime;
        if ((_window.GetKeyboard().GetKeyState(VK_RETURN) == KeyState::Pressed) ||
            (_window.GetKeyboard().GetKeyState(VK_ESCAPE) == KeyState::Pressed))
        {
            cursorConfined = !cursorConfined;
            _window.ConfineMouse(cursorConfined);
            _window.HideMouse(cursorConfined);
        };



        if (_window.GetKeyboard().GetKeyState(VK_LEFT) == KeyState::Held)
        {
            _playerLookAtAngle -= 1.5 * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            _playerLookAtAngle += 1.5 * deltaTime;
        };


        if (_window.GetKeyboard().GetKeyState('W') == KeyState::Held)
        {
            _playerX += std::cosf(_playerLookAtAngle) * 2.0f * deltaTime;
            _playerY += std::sinf(_playerLookAtAngle) * 2.0f * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState('S') == KeyState::Held)
        {
            _playerX -= std::cosf(_playerLookAtAngle) * 2.0f * deltaTime;
            _playerY -= std::sinf(_playerLookAtAngle) * 2.0f * deltaTime;
        };

        if (_window.GetKeyboard().GetKeyState('A') == KeyState::Held)
        {
            _playerX += std::sinf(_playerLookAtAngle) * 2.0f * deltaTime;
            _playerY -= std::cosf(_playerLookAtAngle) * 2.0f * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState('D') == KeyState::Held)
        {
            _playerX -= std::sinf(_playerLookAtAngle) * 2.0f * deltaTime;
            _playerY += std::cosf(_playerLookAtAngle) * 2.0f * deltaTime;
        };
    };


    virtual void DrawScene() override
    {
        for (int x = 0; x < _window.GetWindowWidth(); x++)
        {
            float rayAngle = (_playerLookAtAngle - _playerFOV / 2.0f) + (static_cast<float>(x) / static_cast<float>(_window.GetWindowWidth())) * _playerFOV;


            float distanceToWall = 0.0f;

            const float steps = 0.01;

            bool hitWall = false;


            float playerEyeX = std::cosf(rayAngle);
            float playerEyeY = std::sinf(rayAngle);

            // Find distance to wall
            while ((hitWall == false) &&
                   distanceToWall < _maxDepth)
            {
                distanceToWall += steps;


                int testX = static_cast<int>(_playerX + playerEyeX * distanceToWall);
                int testY = static_cast<int>(_playerY + playerEyeY * distanceToWall);

                // Test if ray is out of bounds
                if (testX < 0 || testX >= _mapWidth ||
                    testY < 0 || testY >= _mapHeight)
                {
                    hitWall = true;
                    distanceToWall = _maxDepth;
                }
                else
                {
                    if (_map[testX + _mapWidth * testY] == true)
                    {
                        hitWall = true;
                    };
                };
            };


            // No idea why or how but this fixes the "fisheye" effect
            float projectedAngle = (_playerLookAtAngle - rayAngle) * (_playerFOV / 2.0f);
            distanceToWall *= std::cosf(projectedAngle);


            int ceiling = static_cast<float>(_window.GetWindowHeight() / 2.0) - _window.GetWindowHeight() / distanceToWall;
            int floor = _window.GetWindowHeight() - ceiling;

            // Calculate wall shading based on distance
            std::uint8_t shade = 255;
            if (distanceToWall < _maxDepth)
                shade = _maxDepth * distanceToWall;

            // Draw the frame column-by-column
            for (int y = 0; y < _window.GetWindowHeight(); y++)
            {
                // Draw ceiling
                if (y < ceiling)
                {
                    _graphics.DrawPixel(x, y, { 0, 255, 255 });
                }
                // Draw walls
                else if (y > ceiling && y < floor)
                {
                    Colour colour = Colours::White;

                    colour.Red -= shade;
                    colour.Green -= shade;
                    colour.Blue -= shade;

                    _graphics.DrawPixel(x, y, colour);
                }
                // Draw floor
                else
                {
                    _graphics.DrawPixel(x, y, Colours::Green);
                };

            };

        };

        // Draw a little minimap showing where the player is located at
        DrawMiniMap();
    };



    void DrawMiniMap()
    {
        int miniMapXOffset = 0;
        int miniMapYOffset = 0;

        int miniMapWidthScale = 10;
        int miniMapHeightScale = 10;

        float lineLength = ((miniMapWidthScale + miniMapHeightScale) / 2);


        // Draw the _map
        for (std::uint64_t x = 0; x < _mapWidth * miniMapWidthScale; x++)
        {
            for (std::uint64_t y = 0; y < _mapHeight * miniMapHeightScale; y++)
            {
                _graphics.DrawPixel(miniMapXOffset + x, miniMapYOffset + y);
            };
        };


        // Draw _map blocks
        for (std::uint64_t a = 0; a < _mapHeight * _mapWidth; a++)
        {
            if (_map[a] == true)
            {
                // Find the block's X and Y
                int mapBlockX = a % _mapWidth;
                int mapBlockY = a / _mapWidth;


                for (std::uint64_t y = 0; y < miniMapHeightScale; y++)
                {
                    for (std::uint64_t x = 0; x < miniMapWidthScale; x++)
                    {
                        _graphics.DrawPixel(x + (mapBlockX * miniMapWidthScale) + miniMapXOffset,
                                            y + (mapBlockY * miniMapHeightScale) + miniMapYOffset,
                                            Colours::Black, false);

                    };
                };
            };
        };


        // Draw player
        for (std::uint64_t x = 0; x < miniMapWidthScale; x++)
        {
            for (std::uint64_t y = 0; y < miniMapHeightScale; y++)
            {
                _graphics.DrawPixel(miniMapXOffset + (_playerX * miniMapWidthScale) + x,
                                    miniMapYOffset + (_playerY * miniMapHeightScale) + y,
                                    Colours::Red);
            };
        };


        // Draw a cone of vision to the camera's angle
        float pointToPointDistanceSpacing = 0.50f;
        float pointsToPlayerDistanceSpacing = 30.f;

        // Draw a line starting from the player's position to the view ray's angle
        Vector2D p0(miniMapXOffset + ((_playerX * miniMapWidthScale) + miniMapWidthScale / 2),
                    miniMapYOffset + ((_playerY * miniMapHeightScale) + miniMapWidthScale / 2));

        Vector2D p1 = p0;
        p1.X += (std::cosf(_playerLookAtAngle + pointToPointDistanceSpacing) * pointsToPlayerDistanceSpacing);
        p1.Y += (std::sinf(_playerLookAtAngle + pointToPointDistanceSpacing) * pointsToPlayerDistanceSpacing);

        Vector2D p2 = p0;
        p2.X += (std::cosf(_playerLookAtAngle - pointToPointDistanceSpacing) * pointsToPlayerDistanceSpacing);
        p2.Y += (std::sinf(_playerLookAtAngle - pointToPointDistanceSpacing) * pointsToPlayerDistanceSpacing);

        _graphics.DrawLine(p0, p1, Colours::Red, false);
        _graphics.DrawLine(p0, p2, Colours::Red, false);
        _graphics.DrawLine(p1, p2, Colours::Red, false);

    };

};