#pragma once
#include <cstddef>

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

};