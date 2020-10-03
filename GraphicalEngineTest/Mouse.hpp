#pragma once
#include <cstddef>
#include <functional>

#include "Event.hpp"
#include "KeyState.hpp"


/// <summary>
/// A class responsible for mouse interactions
/// </summary>
class Mouse
{
    // Allow Window class to set some private data that I don't want other classes to touch
    friend class Window;

private:

    /// <summary>
    /// A boolean flag that indicates if the mouse is inside the window
    /// </summary>
    bool _insideWindow;

    /// <summary>
    /// A list containing event handles for raw mouse moved event
    /// </summary>
    std::vector<std::function<void(int, int)>> _rawMouseMovedHandlers;

    /// <summary>
    /// A mouse wheel event. When execute gives a delta of the mouse wheel's spin
    /// </summary>
    Event<int> _mouseWheenEvent;

public:

    /// <summary>
    /// Mouse X position
    /// </summary>
    int X;

    /// <summary>
    /// Mouse Y position
    /// </summary>
    int Y;

    /// <summary>
    /// State of the left mouse button
    /// </summary>
    KeyState LeftMouseButton;

    /// <summary>
    /// State of the right mouse button
    /// </summary>
    KeyState RightMouseButton;


public:

    Mouse() :
        _insideWindow(false),
        X(0),
        Y(0),
        LeftMouseButton(KeyState::None),
        RightMouseButton(KeyState::None)
    {

    };


public:

    bool InsideWindow() const
    {
        return _insideWindow;
    };

    /// <summary>
    /// Adds a new raw mouse moved event handler 
    /// </summary>
    /// <param name="MouseRawMoved"></param>
    void AddRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        _rawMouseMovedHandlers.push_back(MouseRawMoved);
    };

    /// <summary>
    /// Remove an existing raw mouse moved event handler
    /// </summary>
    /// <param name="MouseRawMoved"></param>
    void RemoveRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        // Find the appropriate event handler
        std::vector<std::function<void(int, int)>>::iterator position =
            std::find_if(_rawMouseMovedHandlers.begin(), _rawMouseMovedHandlers.end(),
                         [MouseRawMoved](std::function<void(int, int)> func)
        {
            // Because std::function doesn't overload the '==' operator 
            // I have to compare the actual function *Pointers
            auto funcTarget = func.target<void(int, int)>();
            auto handlerTarget = func.target<void(int, int)>();

            if (funcTarget == handlerTarget)
                return true;

            return false;
        });

        // If handler exists
        if (position != _rawMouseMovedHandlers.end())
            // Remove it from the list
            _rawMouseMovedHandlers.erase(position);
    };


    void AddMouseWheenEventHandler(const std::function<void(int)>& callback)
    {
        _mouseWheenEvent.AddCallback(callback);
    };

    void RemoveMouseWheenEventHandler(const std::function<void(int)>& callback)
    {
        _mouseWheenEvent.RemoveCallback(callback);
    };


private:

    /// <summary>
    /// A function that will be called when the main window recevied a raw mouse input
    /// </summary>
    /// <param name="lastX"> </param>
    /// <param name="lastY"> </param>
    void OnMouseRawMoved(int lastX, int lastY)
    {
        for (const std::function<void(int, int)>& func : _rawMouseMovedHandlers)
        {
            func(lastX, lastY);
        };

    };

};