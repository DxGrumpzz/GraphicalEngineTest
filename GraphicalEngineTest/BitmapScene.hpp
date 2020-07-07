#pragma once
#include <fstream>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"

class BitmapScene : public IScene
{
private:


    class Sprite
    {

    private:
        Graphics& _graphics;

    public:
        unsigned int Width = 0;
        unsigned int Height = 0;

        Colour* Pixels = nullptr;
        size_t PixelCount = 0;

    public:

        Sprite(Graphics& graphics) :
            _graphics(graphics)
        {
        };


    public:

        void DrawSprite(int x, int y) const
        {
            for (size_t spriteX = 0; spriteX < Width; spriteX++)
            {
                for (size_t spriteY = 0; spriteY < Height; spriteY++)
                {
                    size_t pixelDataIndexer = spriteX + Width * spriteY;

                    Colour& colour = Pixels[pixelDataIndexer];

                    _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
                };
            };
        };


        void DrawSprite(int x, int y, int xOffset, int yOffset, int width, int height) const
        {

            for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
            {
                for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
                {

                    size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                    Colour& colour = Pixels[pixelDataIndexer];

                    _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
                };
            };

        };


        void DrawSpriteChromaKey(int x, int y, int xOffset, int yOffset, int width, int height, const Colour& chromaColour) const
        {

            for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
            {
                for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
                {
                    size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                    Colour& colour = Pixels[pixelDataIndexer];

                    if (colour == chromaColour)
                        continue;

                    _graphics.DrawPixel(spriteX + x, spriteY + y, colour, false);
                };
            };

        };


        void DrawSpriteColourChromaKey(int x, int y, int xOffset, int yOffset, int width, int height, const Colour& spriteolour, const Colour& chromaColour) const
        {

            for (size_t spriteX = 0; spriteX < width - xOffset; spriteX++)
            {
                for (size_t spriteY = 0; spriteY < height - yOffset; spriteY++)
                {
                    size_t pixelDataIndexer = (spriteX + xOffset) + Width * (spriteY + yOffset);

                    Colour& colour = Pixels[pixelDataIndexer];

                    if (colour == chromaColour)
                        continue;

                    _graphics.DrawPixel(spriteX + x, spriteY + y, spriteolour, false);
                };
            };

        };


    };


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

        //std::wstring bitmapPath = L"Resources\\0001.bmp";
        // std::wstring bitmapPath = L"Resources\\Fixedsys16x28.bmp";
        std::wstring bitmapPath = L"Resources\\a32.bmp";

        //std::ifstream file("Resources\\0001.bmp",
        std::ifstream file(bitmapPath,
                           // Read/open the file in binary mode (handles \n\r characters differently, and more)
                           std::ios::binary);

        if (file.bad() == true)
        {
            throw std::exception("Unable to open file");
        };



        BITMAPFILEHEADER bitmapFileHeader = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapFileHeader), sizeof(bitmapFileHeader));

        BITMAPINFOHEADER bitmapInfo = { 0 };
        file.read(reinterpret_cast<char*>(&bitmapInfo), sizeof(bitmapInfo));


        if (bitmapInfo.biCompression != BI_RGB)
            throw std::exception("Unsupported compression type");

        if (bitmapInfo.biBitCount != 32 &&
            (bitmapInfo.biBitCount != 24) &&
            (bitmapInfo.biBitCount != 4))
            throw std::exception("Unsupported bit-per-pixel count");


        bool isUpsideDown = bitmapInfo.biHeight < 0;

        int beginY = 0;
        int endY = 0;

        int deltaY = 0;


        if (isUpsideDown == true)
        {
            _sprite.Height = -bitmapInfo.biHeight;
            beginY = -1;
            endY = _sprite.Height - 1;
            deltaY = 1;
        }
        else
        {
            _sprite.Height = bitmapInfo.biHeight;
            beginY = _sprite.Height - 1;
            endY = -1;
            deltaY = -1;
        };


        int bytesPerPixel = bitmapInfo.biBitCount / 8;

        int padding = (4 - (bitmapInfo.biWidth * bytesPerPixel) % 4) % 4;

        _sprite.Width = bitmapInfo.biWidth;

        _sprite.PixelCount = _sprite.Height * _sprite.Width;
        _sprite.Pixels = new Colour[_sprite.PixelCount];

        memset(_sprite.Pixels, 0, _sprite.PixelCount * sizeof(Colour));


        for (long long y = beginY; y != endY; y += deltaY)
        {
            for (std::size_t x = 0; x < bitmapInfo.biWidth; x++)
            {
                std::size_t pixelPosition = x + bitmapInfo.biWidth * y;

                Colour pixel = { 0 };

                pixel.Red = file.get();
                pixel.Blue = file.get();
                pixel.Green = file.get();


                if (bitmapInfo.biBitCount == 32)
                    pixel.Alpha = file.get();

                _sprite.Pixels[pixelPosition] = pixel;
            };

            if (bitmapInfo.biBitCount != 32)
                file.seekg(padding, std::ios::cur);
        };
    };

public:

    void DrawSprite(int x, int y, float scale, const Sprite& sprite)
    {
        if (scale < -.0f)
            return;

        if (scale > 1.0f)
        {
            for (double spriteX = 0; spriteX < sprite.Width; spriteX++)
            {
                for (double spriteY = 0; spriteY < sprite.Height; spriteY++)
                {

                    for (size_t scaleX = 0; scaleX < scale; scaleX++)
                    {
                        for (size_t scaleY = 0; scaleY < scale; scaleY++)
                        {

                            size_t pixelDataIndexer = spriteX + sprite.Width * spriteY;

                            Colour& colour = sprite.Pixels[pixelDataIndexer];

                            _graphics.DrawPixel(x + (spriteX * scale) + scaleX, y + (spriteY * scale) + scaleY, colour, false);

                        };
                    };

                };
            };
        }
        else
        {
            for (double spriteX = 0; spriteX < sprite.Width; spriteX++)
            {
                for (double spriteY = 0; spriteY < sprite.Height; spriteY++)
                {
                    size_t pixelDataIndexer = spriteX + sprite.Width * spriteY;

                    Colour& colour = sprite.Pixels[pixelDataIndexer];

                    _graphics.DrawPixel(x + (spriteX * scale), y + (spriteY * scale), colour, false);
                };
            };
        };
    };


    virtual void UpdateScene(float deltaTime) override
    {

    };

    Vector2D p0 = { 0, 0 };

    float scale = 1.0f;


    virtual void DrawScene() override
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
            scale += 0.01F;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            scale -= 0.01F;
        };

        DrawSprite(p0.X, p0.Y, scale, _sprite);
    };

};