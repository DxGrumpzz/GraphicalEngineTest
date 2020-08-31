#pragma once
#include <cstddef>
#include <functional>

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

    std::vector<std::function<void(int, int)>> _rawMouseMovedHandlers;

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


    bool InsideWindow()
    {
        return _insideWindow;
    };


    void AddRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        _rawMouseMovedHandlers.push_back(MouseRawMoved);
    };

    void RemoveRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        std::vector<std::function<void(int, int)>>::iterator position =
            std::find_if(_rawMouseMovedHandlers.begin(), _rawMouseMovedHandlers.end(),
                         [MouseRawMoved](std::function<void(int, int)> func)
                         {
                             auto funcTarget = func.target<void(int, int)>();
                             auto handlerTarget = func.target<void(int, int)>();

                             if (funcTarget == handlerTarget)
                                 return true;

                             return false;
                         });

        if (position != _rawMouseMovedHandlers.end())
            _rawMouseMovedHandlers.erase(position);
    };


private:

    void OnMouseRawMoved(int x, int y)
    {
        for (const std::function<void(int, int)>& func : _rawMouseMovedHandlers)
        {
            func(x, y);
        };

    };

};