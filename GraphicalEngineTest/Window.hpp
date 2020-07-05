#pragma once
#include <Windows.h>
#include <string>


/// <summary>
/// A window class, encapsulates normal Windows window functionality into a single class
/// </summary>
class Window
{
    // Undefining CreateWindow macro so we can use a function with the same name
    #undef CreateWindow

private:

    /// <summary>
    /// A boolean flag that indicates if the window messages procedure is bound to the instanced function
    /// </summary>
    inline static bool winProcBound = false;

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

        return true;
    };

public:

    HWND GetHWND()
    {
        return _hwnd;
    };

private:


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
        // If window procedure isn't bound
        if (winProcBound == false)
        {
            // Check if window finished creation
            if (msg == WM_NCCREATE)
            {
                // Get a pointer to the Window
                const CREATESTRUCTW* windowDataPointer = reinterpret_cast<CREATESTRUCTW*>(lParam);
                Window* windowPointer = reinterpret_cast<Window*>(windowDataPointer->lpCreateParams);

                // Set GWLP_USERDATA to the window pointer so we can retrieve it later
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPointer));

                winProcBound = true;

                // Dispatch the message to the instanced window procedure
                return windowPointer->WindowProcedure(hwnd, msg, wParam, lParam);
            };

            // If any other message is received prior, handle it with DefWindowProc
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
        else
        {
            // Get Window ptr from bound user data and dispatch window message to the Window's WindowProcedure
            Window* windowPointer = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            return windowPointer->WindowProcedure(hwnd, msg, wParam, lParam);
        };
    };


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
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                return 0;
            };
        };

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    };


private:

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

        windowClass.lpfnWndProc = WinProcDispatch;

        windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

        // Register the window class
        RegisterClassExW(&windowClass);

        RECT windowRect;
        windowRect.top = 50;
        windowRect.left = 50;
        
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
                                this);
    };

};