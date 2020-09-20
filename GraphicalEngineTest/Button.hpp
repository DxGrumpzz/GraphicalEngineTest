#include <functional>

#include <Graphics.hpp>
#include "Mouse.hpp"
#include "Colour.hpp"


/// <summary>
/// A button control that can be pressed
/// </summary>
class Button
{

private:

    /// <summary>
    /// The function that will exeucte when user presses the button
    /// </summary>
    std::function<void()> _onClickHandler;


    /// <summary>
    /// A text that will be displayed inside the button
    /// </summary>
    std::wstring _buttonText;


    /// <summary>
    /// The button's width
    /// </summary>
    float _buttonWidth;

    /// <summary>
    /// The button's height
    /// </summary>
    float _buttonHeight;

    /// <summary>
    /// The button's X position
    /// </summary>
    float _buttonX;

    /// <summary>
    /// The button's Y position
    /// </summary>
    float _buttonY;


    /// <summary>
    /// A pixel array that will be used to draw the button
    /// </summary>
    Colour* _buttonPixels;


    /// <summary>
    /// A boolean fla that indicates if the user's cursor is already inside the button
    /// </summary>
    bool _mouseInsideButton = false;

public:

    Button(std::wstring buttonText = L"",
           float width = 50.0f, float height = 25.0f,
           float x = 0.0f, float y = 0.0f) :
        _onClickHandler(nullptr),

        _buttonText(buttonText),

        _buttonWidth(width),
        _buttonHeight(height),

        _buttonX(x),
        _buttonY(y)
    {

        _buttonPixels = new Colour[_buttonWidth * _buttonHeight] { 0 };

        // Draw a white background
        for (int a = 0; a < _buttonWidth * _buttonHeight; a++)
        {
            _buttonPixels[a] = { 255, 255, 255 };
        };

        // Draw the black borders around the button
        for (int x = 0; x < _buttonWidth; x++)
        {
            const std::size_t index = Maths::Convert2DTo1D(x, 0, _buttonWidth);
            const std::size_t indexLast = Maths::Convert2DTo1D(x, _buttonHeight - 1, _buttonWidth);

            _buttonPixels[index] = Colours::Black;
            _buttonPixels[indexLast] = Colours::Black;
        };

        for (int y = 0; y < _buttonHeight; y++)
        {
            const std::uint64_t index = Maths::Convert2DTo1D(0, y, _buttonWidth);
            const std::uint64_t indexLast = Maths::Convert2DTo1D(_buttonWidth - 1, y, _buttonWidth);

            _buttonPixels[index] = Colours::Black;
            _buttonPixels[indexLast] = Colours::Black;
        };

    };


public:

    /// <summary>
    /// Binds a click event to a function
    /// </summary>
    /// <param name="onClickHandler"></param>
    void BindClickEvent(std::function<void()> onClickHandler)
    {
        _onClickHandler = onClickHandler;
    };


    /// <summary>
    /// Updates the button's interactivity 
    /// </summary>
    /// <param name="mouse"></param>
    void Update(const Mouse& mouse)
    {
        // If onClick isn't bound don't bother with further execution
        if (_onClickHandler == nullptr)
            return;

        // Check if mouse is inside the button
        if ((mouse.X > _buttonX && mouse.X < _buttonX + _buttonWidth) &&
            (mouse.Y > _buttonY && mouse.Y < _buttonY + _buttonHeight))
        {
            // If we're already inside the button...
            if (_mouseInsideButton == true)
                // Exit function so we don't overload on function calls
                return;

            _mouseInsideButton = true;

            // Set cursor to hand 
            SetCursor(LoadCursorW(NULL, IDC_HAND));


            // Check if the user pressed the button
            if (mouse.LeftMouseButton == KeyState::Pressed)
            {
                // Execute click handler
                _onClickHandler();
            };
        }
        else
        {
            // If we're already outside the button...
            if (_mouseInsideButton == false)
                // Exit function so we don't overload on function calls
                return;

            _mouseInsideButton = false;

            // Revert mouse cursor to default 
            SetCursor(LoadCursorW(NULL, IDC_ARROW));
        };

    };

    /// <summary>
    /// Draw the button
    /// </summary>
    /// <param name="graphics"></param>
    void Draw(Graphics& graphics)
    {
        // Copy button pixels onto the screen buffer
        for (int x = 0; x < _buttonWidth; x++)
        {
            for (int y = 0; y < _buttonHeight; y++)
            {
                const std::size_t index = Maths::Convert2DTo1D(x, y, _buttonWidth);

                graphics.DrawPixel(x + _buttonX, y + _buttonY, _buttonPixels[index]);
            };
        };

    };

};