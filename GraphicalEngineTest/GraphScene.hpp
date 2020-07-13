#pragma once

#include <vector>

#include "IScene.hpp"
#include "Vector2D.hpp"
#include "Graphics.hpp"
#include "Sprite.hpp"
#include "Window.hpp"



class FontSheet
{

private:

    Graphics& _graphics;

    Sprite _sprite;

    int _glyphWidth;
    int _glyphHeight;

    int _numberOfColumns = 0;
    int _numberOfRows = 0;

public:

    FontSheet(Graphics& graphics,
              int glyphWidh,
              int glyphHeight) :
        _graphics(graphics),

        _sprite(graphics),

        _glyphWidth(glyphWidh),
        _glyphHeight(glyphHeight)
    {
    };


public:

    void LoadFromFile(const std::wstring& spriteFile)
    {
        _sprite.LoadFromFile(spriteFile);

        _numberOfColumns = _sprite.Width / _glyphWidth;
        _numberOfRows = _sprite.Height / _glyphHeight;

    };


    void DrawString(int x, int y, const std::string& text)
    {
        int startingX = x;
        int characterCounter = 0;

        for (size_t a = 0; a < text.size(); a++)
        {
            char currentChar = text[a];

            if (currentChar == '\n')
            {
                x = startingX;
                y += _glyphHeight;

                characterCounter = 0;
                continue;
            };

            DrawChar(x + (characterCounter * _glyphWidth), y,
                     currentChar);

            characterCounter++;
        };
    };


    void DrawChar(int x, int y, char character)
    {
        Vector2D characterPos = GetCharacterPos(character);

        _sprite.DrawSprite(x, y,
                           _glyphWidth * characterPos.X, _glyphHeight * characterPos.Y,
                           (_glyphWidth * characterPos.X) + _glyphWidth, (_glyphHeight * characterPos.Y) + _glyphHeight);
    };


    void DrawChar(int x, int y,
                  char character,
                  float horizontalScale, float verticalScale)
    {
        Vector2D characterPos = GetCharacterPos(character);

        _sprite.DrawSprite(x, y,
                           _glyphWidth * characterPos.X, _glyphHeight * characterPos.Y,
                           (_glyphWidth * characterPos.X) + _glyphWidth, (_glyphHeight * characterPos.Y) + _glyphHeight,
                           horizontalScale,
                           verticalScale);
    };

private:

    Vector2D GetCharacterPos(char character)
    {
        int glyphIndex = character - ' ';

        int glpyhX = glyphIndex % _numberOfColumns;
        int glpyhY = glyphIndex / _numberOfColumns;

        return { static_cast<float>(glpyhX), static_cast<float>(glpyhY) };
    };

};



class GraphScene : public IScene
{

private:
    std::vector<Vector2D> _points;

    Graphics& _graphics;
    Window& _window;


    FontSheet _font;


    float _horizontalScale = 1.0f;
    float _verticalScale = 1.0f;

    Vector2D _charPos = { 0, 0 };

public:

    GraphScene(Graphics& graphics, Window& window) :
        _graphics(graphics),
        _window(window),
        _font(graphics, 16, 28)
    {
        _font.LoadFromFile(L"Resources\\Fixedsys16x28.bmp");
    };


public:

    virtual void UpdateScene(float deltaTime) override
    {
        const Mouse& mouse = _window.GetMouse();
        const Keyboard& keyboard = _window.GetKeyboard();

        if (mouse.LeftMouseButton == KeyState::Held)
        {
            _charPos.X = mouse.X;
            _charPos.Y = mouse.Y;
        };

        if (keyboard.GetKeyState(VK_UP) == KeyState::Held)
        {
            _verticalScale -= 1.0f * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_DOWN) == KeyState::Held)
        {
            _verticalScale += 1.f * deltaTime;
        }

        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Held)
        {
            _horizontalScale -= 1.0F * deltaTime;
        }
        else if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Held)
        {
            _horizontalScale += 1.0F * deltaTime;
        };

    };


    virtual void DrawScene() override
    {
        _font.DrawString(_charPos.X,
                       _charPos.Y,
                       "asdf\nfdsa4\n12651");

        //_font.DrawString(_charPos.X, _charPos.Y, "0\n12\n3\n456\n78\n9");
    };


public:

    void DrawGraph()
    {


    };

};