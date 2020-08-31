#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <bitset>
#include <hidusage.h>

#include "Mouse.hpp"
#include "Vector2D.hpp"
#include "WindowsUtilities.hpp"
#include "Keyboard.hpp"


/// <summary>
/// A window class, encapsulates normal Windows window functionality into a single class
/// </summary>
class Window
{
    // Undefining CreateWindow macro so we can use a function with the same name
    #undef CreateWindow

private:

    /// <summary>
    /// Handle to the window
    /// </summary>
    HWND _hwnd;

    /// <summary>
    /// Handle to process instnace
    /// </summary>
    HINSTANCE _hInstance;

    /// <summary>
    /// A registered window class name
    /// </summary>
    std::wstring _windowClassName;

    /// <summary>
    /// The tile of this window
    /// </summary>
    std::wstring _windowTitle;

    /// <summary>
    /// Window width
    /// </summary>
    int _windowWidth;

    /// <summary>
    /// Window height
    /// </summary>
    int _windowHeight;

    Mouse _mouse;

    Keyboard  _keyboard;

    /// <summary>
    /// A boolean flag that indicates if the mouse is currently being confined to the window
    /// </summary>
    bool _mouseConfined = false;

    /// <summary>
    /// A boolean flag that indicates if the cursor is currently hidden
    /// </summary>
    bool _mouseHidden = false;

public:

    Window(int windowWidth, int windowHeight,
           HINSTANCE hInstance,
           std::wstring windowClass, std::wstring windowTitle) :
        _hwnd(NULL),
        _hInstance(hInstance),
        _windowClassName(windowClass),
        _windowTitle(windowTitle),

        _windowWidth(windowWidth),
        _windowHeight(windowHeight)
    {

        CreateWindow();

    };

    ~Window()
    {
        UnregisterClassW(_windowClassName.c_str(), _hInstance);
    };


public:

    /// <summary>
    /// Shows the window after constructing it
    /// </summary>
    void ShowWindow()
    {
        ::ShowWindow(_hwnd, SW_SHOW);
    };


    /// <summary>
    /// Processes messages in window message buffer. Returns true after all messages were handled, 
    /// if WM_QUIT message was sent will return false
    /// </summary>
    /// <returns></returns>
    bool ProcessMessageBuffer()
    {
        // Windows message loop
        MSG message;

        // Process available messages
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            // To exit the infinite loop check if the current message was a quit message
            if (message.message == WM_QUIT)
            {
                return false;
            };

            // If the message is a keystroke get the key's character value
            TranslateMessage(&message);

            // Send the message to the Window procedure function
            DispatchMessageW(&message);
        };

        // Handle mouse and keybaord *key* events independently from window events
        HandleMouseEvents();
        HandleKeyboardEvents();


        // Confine the mouse is requested
        if (_mouseConfined == true)
        {
            // only confine the mouse if the window is focused
            if (WindowFocused() == true)
            {
                // A clipping area that the mouse will be confined to 
                RECT clipRect;

                // Get a RECT for the window's client area.
                // But there's a problem, GetClientRect returns a RECT relative to to screen
                GetClientRect(GetHWND(), &clipRect);
                // So we use this function to map the points inside clipRect to the actual window's client area
                MapWindowPoints(GetHWND(), nullptr, reinterpret_cast<POINT*>(&clipRect), 2);

                // Confine the cursor
                ClipCursor(&clipRect);
            };
        }
        else
        {
            // Release confinment
            ClipCursor(nullptr);
        };


        // Hide the cursor if requested
        if (_mouseHidden == true)
        {
            // Somewhere deep in windows cursor holds a counter for it's visibility state
            // When we call functions like 'ShowCursor' we need to decrement or increment 
            // until cursor is hidden or visible
            while (ShowCursor(false) >= 0);
        }
        else
        {
            while (ShowCursor(true) < 0);
        };

        return true;
    };


    /// <summary>
    /// Confines the cursor to the window's client area
    /// </summary>
    /// <param name="confine"></param>
    void ConfineMouse(bool confine)
    {
        _mouseConfined = confine;
    };

    /// <summary>
    /// Hides the cursor
    /// </summary>
    /// <param name="hide"></param>
    void HideMouse(bool hide)
    {
        _mouseHidden = hide;
    };


public:

    HWND GetHWND()
    {
        return _hwnd;
    };

    Vector2D GetMousePosition()
    {
        return { static_cast<float>(_mouse.X), static_cast<float>(_mouse.Y) };
    };


    int GetWindowWidth() const
    {
        return _windowWidth;
    };

    int GetWindowHeight() const
    {
        return _windowHeight;
    };


    const Mouse& GetMouse()
    {
        return _mouse;
    };

    const Keyboard& GetKeyboard()
    {
        return _keyboard;
    };

    /// <summary>
    /// Returns true if the window is currently focused
    /// </summary>
    /// <returns></returns>
    bool WindowFocused()
    {
        return (GetFocus() == GetHWND());
    };


    void AddRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        _mouse.AddRawMouseMovedHandler(MouseRawMoved);
    };

    void RemoveRawMouseMovedHandler(std::function<void(int, int)> MouseRawMoved)
    {
        _mouse.AddRawMouseMovedHandler(MouseRawMoved);
    };

private:


    /// <summary>
    /// A window message handler for the main window
    /// </summary>
    /// <param name="hwnd"> Calling(sender) window </param>
    /// <param name="msg"> The recevied Message </param>
    /// <param name="wParam"> Parameter related to received message </param>
    /// <param name="lParam"> Parameter related to received message </param>
    /// <returns></returns>
    LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {

            case WM_MOUSEMOVE:
            {
                // Get mouse X and X positions
                _mouse.X = GET_X_LPARAM(lParam);
                _mouse.Y = GET_Y_LPARAM(lParam);


                _mouse._insideWindow = true;


                // To Receive WM_MOUSELEAVE message we need to notify Windows to track mouse events
                TRACKMOUSEEVENT mouseTrackEvent;
                mouseTrackEvent.cbSize = sizeof(mouseTrackEvent);
                mouseTrackEvent.hwndTrack = _hwnd;
                mouseTrackEvent.dwFlags = TME_LEAVE;

                TrackMouseEvent(&mouseTrackEvent);

                return 0;
            };

            case WM_INPUT:
            {
                std::uint32_t size = 0;
                GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

                std::uint8_t* bytes = new uint8_t[size];

                GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, bytes, &size, sizeof(RAWINPUTHEADER));

                RAWINPUT& rawInput = *reinterpret_cast<RAWINPUT*>(bytes);


                if (rawInput.header.dwType == RIM_TYPEMOUSE)
                {
                    RAWMOUSE mouse = rawInput.data.mouse;
                  
                    _mouse.OnMouseRawMoved(mouse.lLastX, mouse.lLastY);
                };


                delete[] bytes;
                bytes = nullptr;

                return 0;
            };

            case WM_MOUSELEAVE:
            {
                _mouse._insideWindow = false;
                return 0;
            };


            // Even though we handle keyboard inputs in HandleKeyboardEvents function 
            // I'm handling WM_KEYDOWN just for the Text auto repeat
            case WM_KEYDOWN:
            {
                // Check if key is requesting auto repeat, by getting the value of the 30th bit
                bool isKeyHeld = std::bitset<sizeof(LPARAM) * 8>(lParam).test(30);

                int keycode = static_cast<int>(wParam);

                if (isKeyHeld == true)
                    _keyboard._keys[keycode].TextAutoRepeat = true;

                return 0;
            };


            case WM_KEYUP:
            {
                int keycode = static_cast<int>(wParam);

                _keyboard._keys[keycode].TextAutoRepeat = false;

                return 0;
            };


            case WM_CLOSE:
            {
                PostQuitMessage(0);
                return 0;
            };

            case WM_DESTROY:
            {
                // Free cursor confinement, if necessary
                ClipCursor(nullptr);
            };
        };

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    };


    /// <summary>
    /// Creates the window handle
    /// </summary>
    void CreateWindow()
    {
        // Create the main window
        WNDCLASSEXW windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEXW);
        windowClass.style = CS_CLASSDC;
        windowClass.hInstance = _hInstance;
        windowClass.lpszClassName = _windowClassName.c_str();

        // Start with the default window procedure
        windowClass.lpfnWndProc = DefWindowProcW;

        windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

        // Register the window class
        RegisterClassExW(&windowClass);

        RECT windowRect;
        windowRect.top = 100;
        windowRect.left = 75;

        windowRect.bottom = _windowHeight + windowRect.top;
        windowRect.right = _windowWidth + windowRect.left;


        DWORD windowStyles = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

        // Apparently this function is VERY important if you don't want your draw calls to skip pixel lines(???)
        AdjustWindowRect(&windowRect, windowStyles, false);

        // Create the actual window
        _hwnd = CreateWindowExW(0,
                                _windowClassName.c_str(),
                                _windowTitle.c_str(),
                                windowStyles,
                                windowRect.left, windowRect.top,
                                // To not skip vertical/horizontal lines when drawing we must specify window width and height 
                                // based on the differences between right - left and bottom - top
                                windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                                nullptr,
                                nullptr,
                                _hInstance,
                                nullptr);

        RAWINPUTDEVICE rawInputDevice { 0 };
        rawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
        rawInputDevice.usUsage = HID_USAGE_GENERIC_MOUSE;
        rawInputDevice.hwndTarget = _hwnd;

        if (!RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice)))
        {
            std::wstring error = WindowsUtilities::GetLastErrorAsStringW();
            DebugBreak();
        };

        // Set window user data to point to this class instance
        SetWindowLongPtrW(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        // Set window procedure to use the dipatching window procedure
        SetWindowLongPtrW(_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WinProcDispatch));
    };


    /// <summary>
    /// Handles user mouse input
    /// </summary>
    void HandleMouseEvents()
    {
        // Handle mouse clicks only if mouse is in bounds of the console
        if (_mouse.InsideWindow() == true)
        {
            _mouse.LeftMouseButton = GetNewKeyState(VK_LBUTTON, _mouse.LeftMouseButton);

            _mouse.RightMouseButton = GetNewKeyState(VK_RBUTTON, _mouse.RightMouseButton);
        };
    };


    /// <summary>
    /// Handles user keyboard input 
    /// </summary>
    void HandleKeyboardEvents()
    {
        // Check if window is in focus
        bool windowInFocus = GetFocus() == _hwnd;

        // Don't update keys if window isn't currently focused
        if (windowInFocus == false)
        {
            for (int a = 0; a < Keyboard::NUMBER_OF_KEYS; a++)
                _keyboard._keys[a].KeyState = KeyState::None;

            return;
        };


        // Update keys 
        for (int a = 0; a < Keyboard::NUMBER_OF_KEYS; a++)
        {
            Key& key = _keyboard._keys[a];

            key.KeyState = GetNewKeyState(key.Key, key.KeyState);
        };

    };


    /// <summary>
    /// Returns a MouseKeyState based on a current mouse key state
    /// </summary>
    /// <param name="mouseKeyCode"> The mouse button key code </param>
    /// <param name="previousMouseKey"> The previous mouse key state </param>
    /// <returns></returns>
    KeyState GetNewKeyState(int KeyCode, KeyState previousKeyState)
    {
        // Check if mouse button is pressed
        if (GetAsyncKeyState(KeyCode))
        {
            // If key was pressed or held before
            if (previousKeyState == KeyState::Held ||
                previousKeyState == KeyState::Pressed)
            {
                // Set key state to held from a press
                return KeyState::Held;
            }
            // If key state was None, or released 
            else
            {
                // Return pressed key state
                return KeyState::Pressed;
            };
        }
        // If key state hasn't changed between frames
        else
        {
            // If mouse key was previously held or pressed
            if (previousKeyState == KeyState::Held ||
                previousKeyState == KeyState::Pressed)
            {
                // Set it to released
                return KeyState::Released;
            }
            // If key still wasn't pressed 
            else
            {
                // Set it's state to none because nothing changed
                return KeyState::None;
            };
        };
    };


    /// <summary>
    /// A window message dispatcher, the only purpose of this function is to dispatch window messages to the instanced WinProc in this class
    /// </summary>
    /// <param name="hwnd"></param>
    /// <param name="msg"></param>
    /// <param name="wParam"></param>
    /// <param name="lParam"></param>
    /// <returns></returns>
    static LRESULT CALLBACK WinProcDispatch(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        // Get Window ptr from bound user data and dispatch window message to the Window's WindowProcedure
        Window* windowPointer = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        return windowPointer->WindowProcedure(hwnd, msg, wParam, lParam);
    };

};