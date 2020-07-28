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

    float _playerFOV = 2.0f;//Maths::PI / 4;

    float _maxDepth = 10;

    bool* _map;


public:

    TestScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window)
    {
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


    virtual void UpdateScene(float deltaTime) override
    {
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
            _playerY -= 0.5 * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState('S') == KeyState::Held)
        {
            _playerY += 0.5 * deltaTime;
        };


        if (_window.GetKeyboard().GetKeyState('A') == KeyState::Held)
        {
            _playerX -= 0.5 * deltaTime;
        }
        else if (_window.GetKeyboard().GetKeyState('D') == KeyState::Held)
        {
            _playerX += 0.5 * deltaTime;
        };

    };


    virtual void DrawScene() override
    {
        for (int x = 0; x < _window.GetWindowWidth(); x++)
        {
            float rayAngle = (_playerLookAtAngle - _playerFOV / 2.0f) + (static_cast<float>(_playerX) / static_cast<float>(_window.GetWindowWidth())) * _playerFOV;


            float distanceToWall = 0.0f;

            bool hitWall = false;

            float playerEyeX = std::cos(_playerLookAtAngle);
            float playerEyeY = std::sin(_playerLookAtAngle);

            while ((hitWall == false) &&
                   distanceToWall < _maxDepth)
            {
                distanceToWall += 0.1f;

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


            int ceiling = static_cast<float>(_window.GetWindowHeight() / 2.0) - _window.GetWindowHeight() / distanceToWall;
            int floor = _window.GetWindowHeight() - ceiling;

            std::uint8_t shade = 0;

            if (distanceToWall <= _maxDepth / 4.0f)
                shade = 10;
            else if (distanceToWall < _maxDepth / 3.0f)
                shade = 22;
            else if (distanceToWall < _maxDepth / 2.0f)
                shade = 46;
            else if (distanceToWall < _maxDepth / 2.0f)
                shade = 94;
            else
                shade = 190;

            for (int y = 0; y < _window.GetWindowHeight(); y++)
            {
                if (y < ceiling)
                {
                    _graphics.DrawPixel(x, y, { 0, 255, 255 });
                }
                else if (y > ceiling && y < floor)
                {
                    Colour colour = Colours::White;

                    colour.Red -= shade;
                    colour.Green -= shade;
                    colour.Blue -= shade;

                    _graphics.DrawPixel(x, y, colour);
                }
                else
                {
                    _graphics.DrawPixel(x, y, Colours::Green);
                };

            };

        };

        DrawMiniMap();
    };

    void DrawMiniMap()
    {
        int miniMapXOffset = 0;
        int miniMapYOffset = 0;


        int miniMapWidthScale = 8;
        int miniMapHeightScale = 8;


        // Draw the map
        for (std::uint64_t x = 0; x < _mapWidth * miniMapWidthScale; x++)
        {
            for (std::uint64_t y = 0; y < _mapHeight * miniMapHeightScale; y++)
            {
                _graphics.DrawPixel(miniMapXOffset + x, miniMapYOffset + y);
            };
        };


        // Draw map blocks
        for (std::uint64_t x = 0; x < _mapWidth; x++)
        {
            for (std::uint64_t y = 0; y < _mapHeight; y++)
            {

                if (_map[x + _mapWidth * y] == true)
                {
                    for (std::uint64_t _x = 0; _x < miniMapWidthScale; _x++)
                    {
                        for (std::uint64_t _y = 0; _y < miniMapHeightScale; _y++)
                        {
                            _graphics.DrawPixel(_x + (x * miniMapWidthScale) + miniMapXOffset,
                                                _y + (y * miniMapHeightScale) + miniMapYOffset,
                                                Colours::Black, false);
                        };
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


        // Calculate x and y position of player view angle
        float angleX = std::cosf(_playerLookAtAngle);
        float angleY = std::sinf(_playerLookAtAngle);

        float lineLength = 10.0f;

        // Draw a line starting from the player's position to the view ray's angle
        Vector2D p0(miniMapXOffset + ((_playerX * miniMapWidthScale) + miniMapWidthScale / 2),
                    miniMapYOffset + ((_playerY * miniMapHeightScale) + miniMapWidthScale / 2));

        Vector2D p1 = p0;
        p1.X += (angleX * lineLength);
        p1.Y += (angleY * lineLength);

        _graphics.DrawLine(p0, p1, Colours::Black, false);
        _graphics.DrawLine({ p0.X, p0.Y + 1 }, { p1.X, p1.Y + 1 }, Colours::Black, false);

        _graphics.DrawLine({ p0.X + 1, p0.Y }, { p1.X + 1, p1.Y }, Colours::Black, false);
    };

};