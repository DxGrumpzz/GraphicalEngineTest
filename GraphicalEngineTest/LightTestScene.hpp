#pragma once

#include <cstdint>
#include <random>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"


class LightTestScene : public IScene
{
private:

    Graphics& _graphics;

    Window& _window;

    std::uint64_t _mapWidth = 10;
    std::uint64_t _mapHeight = 10;

    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<int> dist;

    int _cellSize = 20;

    bool* _map;

    float* _mapF;



public:

    LightTestScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),

        mt(rd()),
        dist(0, _mapWidth * _mapHeight)
    {

        _map = new bool[_mapWidth * _mapHeight] // { 0 };
        {
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,1,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,
        };
        /*
        {
            1,0,1,0,1,1,0,0,0,0,
            0,0,1,1,1,1,0,0,0,0,
            0,0,0,0,0,1,0,0,1,0,
            0,0,0,0,1,0,0,0,0,0,
            1,0,0,1,1,0,0,0,0,0,
            0,0,0,0,0,0,1,0,0,0,
            0,0,0,1,0,0,0,0,0,0,
            0,0,0,0,1,0,1,0,1,0,
            0,0,0,0,0,0,0,0,0,0,
            0,0,1,0,0,0,0,0,0,0,
        };
        */

        _mapF = new float[_mapWidth * _mapHeight] { 0 };


        //GenerateRandomMapPoints();
    };


    ~LightTestScene()
    {
        delete[] _mapF;
        _mapF = nullptr;

        delete[] _map;
        _map = nullptr;
    };


public:


    virtual void UpdateScene(float deltaTime) override
    {
        if (_window.GetKeyboard().GetKeyState(VK_RETURN) == KeyState::Pressed)
        {

            return;

            ClearMaps();
            GenerateRandomMapPoints();
        };

    };


    virtual void DrawScene() override
    {
        std::uint64_t previousIndex = 0;

        float falloffRate = 0.1f;
        float upliftRate = 0.1f;



        for (std::uint64_t mapIndex = 0; mapIndex < _mapWidth * _mapHeight; mapIndex++)
        {
            if (_map[mapIndex] == true)
            {
                _mapF[mapIndex] = 1.0f;

                float falloffTemp = 0.0f;

                int s = 3;
                int ss = 1;

                while (falloffTemp < 1.0f)
                {
                    int x = mapIndex % _mapWidth;
                    int y = mapIndex / _mapWidth;

                    x -= ss;
                    y -= ss;

                    for (std::uint64_t column = 0; column < s; column++)
                    {
                        if (!(column == 0 || 
                            column == s - 1))
                            continue;

                        for (std::uint64_t row = 0; row < s; row++)
                        {
                            //if (!(row == 0 ||
                                //row == s - 1))
                                //continue;

                            int position = Maths::Convert2DTo1D(x + column,
                                                                y + row,
                                                                _mapWidth);

                            if (position < 0 ||
                                position > _mapWidth * _mapHeight)
                                continue;

                            if (position == mapIndex)
                                continue;

                            _mapF[position] = falloffTemp;

                        };
                    };

                    falloffTemp += falloffRate;
                    s += 2;
                    ss++;
                };

            }
            else
            {
            };

        };



        for (std::uint64_t mapIndex = 0; mapIndex < _mapWidth * _mapHeight; mapIndex++)
        {
            const float& colourF = _mapF[mapIndex];

            const uint8_t normalized = DenormalizeToByte(colourF);

            const Colour colour = { normalized, normalized, normalized };


            const std::uint64_t mapX = mapIndex % _mapWidth;
            const std::uint64_t mapY = mapIndex / _mapWidth;


            for (std::uint64_t column = 0; column < _cellSize; column++)
            {
                for (std::uint64_t row = 0; row < _cellSize; row++)
                {
                    const std::uint64_t xPos = column + (mapX * _cellSize);
                    const std::uint64_t yPos = row + (mapY * _cellSize);

                    _graphics.DrawPixel(xPos,
                                        yPos,
                                        colour);


                    if ((column % _cellSize) == 0)
                    {
                        _graphics.DrawPixel(xPos, yPos, Colours::Black);
                    };

                    if ((row % _cellSize) == 0)
                    {
                        _graphics.DrawPixel(xPos, yPos, Colours::Black);
                    };

                };
            };

        };

    };


    void DrawCell(int x, int y, const Colour& cellColour)
    {

        for (std::uint64_t column = 0; column < _cellSize; column++)
        {
            for (std::uint64_t row = 0; row < _cellSize; row++)
            {
                const std::uint64_t xPos = column + (x * _cellSize);
                const std::uint64_t yPos = row + (y * _cellSize);

                _graphics.DrawPixel(xPos,
                                    yPos,
                                    cellColour);
            };
        };

    };


private:

    std::uint8_t DenormalizeToByte(float normalizedValue)
    {
        std::uint8_t denormalized = Maths::DenormalizeNumber<std::uint8_t, float>(normalizedValue, 0, 255);

        return denormalized;
    };


    void ClearMaps()
    {
        for (std::uint64_t a = 0; a < _mapWidth * _mapHeight; a++)
        {
            _map[a] = 0;
            _mapF[a] = 0.0f;
        };

    };


    void GenerateRandomMapPoints()
    {
        int points = 50;

        for (std::uint64_t a = 0; a < points; a++)
        {
            int position = dist(mt);

            _map[position] = 1;
        };
    };

};