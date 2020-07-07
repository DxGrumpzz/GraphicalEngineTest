#pragma once
#include <fstream>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"
#include "Sprite.hpp"


class BitmapScene : public IScene
{
private:


private:

    Graphics& _graphics;
    Window& _window;

    Sprite _sprite;

public:

    BitmapScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _sprite(graphics)
    {

        std::wstring bitmapPath = L"Resources\\Vikings_smallenemies_004.bmp";

        std::ifstream file(bitmapPath,
                           // Read/open the file in binary mode (handles \n\r characters differently, and more)
                           std::ios::binary);

        if (file.bad() == true)
        {
            throw std::exception("Unable to open file");
        };

        // Get bitmap header
        BITMAPFILEHEADER bitmapFileHeader = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapFileHeader), sizeof(bitmapFileHeader));

        // Get bitmap info
        BITMAPINFOHEADER bitmapInfo = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapInfo), sizeof(bitmapInfo));


        // Bitmap validation, currently only non compressed, 32, and 24 bit, bitmaps are supported
        if (bitmapInfo.biCompression != BI_RGB)
            throw std::exception("Unsupported compression type");

        if (bitmapInfo.biBitCount != 32 &&
            (bitmapInfo.biBitCount != 24))
            throw std::exception("Unsupported bit-per-pixel count");


        // If height is negative the bitmap is read top to bottom
        // Otherwise bottom to top (default)
        bool isUpsideDown = bitmapInfo.biHeight < 0;

        int beginY = 0;
        int endY = 0;

        int deltaY = 0;

        if (isUpsideDown == true)
        {
            _sprite.Height = -bitmapInfo.biHeight;
            beginY = 0;
            endY = _sprite.Height;
            deltaY = 1;
        }
        else
        {
            _sprite.Height = bitmapInfo.biHeight;
            beginY = _sprite.Height - 1;
            endY = -1;
            deltaY = -1;
        };


        // Calculate bitmap row padding (If it's 24-bit bitmap )
        int bytesPerPixel = bitmapInfo.biBitCount / 8;
        int padding = (4 - (bitmapInfo.biWidth * bytesPerPixel) % 4) % 4;

        _sprite.Width = bitmapInfo.biWidth;

        // Create the sprite pixels 
        _sprite.PixelCount = _sprite.Height * _sprite.Width;
        _sprite.Pixels = new Colour[_sprite.PixelCount];

        memset(_sprite.Pixels, 0, _sprite.PixelCount * sizeof(Colour));


        // Read bitmap 
        for (long long y = beginY; y != endY; y += deltaY)
        {
            for (std::size_t x = 0; x < bitmapInfo.biWidth; x++)
            {
                // Find pixel position 
                std::size_t pixelPosition = x + bitmapInfo.biWidth * y;


                Colour pixel = { 0 };

                pixel.Blue = file.get();
                pixel.Green = file.get();
                pixel.Red = file.get();

                // If the bitmap is 32 bit
                if (bitmapInfo.biBitCount == 32)
                    // Read alpha
                    pixel.Alpha = file.get();

                // Add pixel to sprite
                _sprite.Pixels[pixelPosition] = pixel;
            };

            // If the bitmap isn't 32 bit there should usually be row padding,
            // so move reader-head to next the row
            if (bitmapInfo.biBitCount != 32)
                file.seekg(padding, std::ios::cur);
        };
    };

public:




    Vector2D p0 = { 0, 0 };

    float horizontalScale = 1.0f;
    float verticalScale = 1.0f;


    virtual void UpdateScene(float deltaTime) override
    {
        Mouse mouse = _window.GetMouse();
        const Keyboard& keyboard = _window.GetKeyboard();

        if (mouse.LeftMouseButton == KeyState::Held)
        {
            p0.X = mouse.X;
            p0.Y = mouse.Y;
        };

        if (keyboard.GetKeyState(VK_UP) == KeyState::Held)
        {
            verticalScale -= 2.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            verticalScale += 2.0F * deltaTime;
        }

        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Held)
        {
            horizontalScale -= 2.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            horizontalScale += 2.0F * deltaTime;
        };

    };


    virtual void DrawScene() override
    {
        //_sprite.DrawSpriteScale(p0.X, p0.Y, horizontalScale, verticalScale);
         _sprite.DrawSpriteScale(p0.X, p0.Y, verticalScale);
    };

};