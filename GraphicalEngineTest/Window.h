#pragma once
#include <Windows.h>
#include <string>

class Window
{
private:

    HWND _hwnd;

    HINSTANCE _hInstance;

    std::wstring _windowClassName;
    std::wstring _windowTitle;

    int _windowWidth;
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
        CreateWindowHandle();
    };

    ~Window()
    {
        UnregisterClassW(_windowClassName.c_str(), _hInstance);
    };

public:


    void ShowWindow()
    {
        ::ShowWindow(_hwnd, SW_SHOW);
    };


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

    static LRESULT CALLBACK WinProcDispatch(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Window* windowPointer = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        return windowPointer->WindowProcedure(hwnd, msg, wParam, lParam);
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

    void CreateWindowHandle()
    {
        auto tempWindowProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (msg == WM_NCCREATE)
            {
                const CREATESTRUCTW* windowDataPointer = reinterpret_cast<CREATESTRUCTW*>(lParam);
                Window* windowPointer = reinterpret_cast<Window*>(windowDataPointer->lpCreateParams);
                
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowPointer));

                SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Window::WinProcDispatch));

                return windowPointer->WindowProcedure(hwnd, msg, wParam, lParam);
            };

            return DefWindowProcW(hwnd, msg, wParam, lParam);
        };


        // Create the main window
        WNDCLASSEXW windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEXW);
        windowClass.style = CS_CLASSDC;
        windowClass.hInstance = _hInstance;
        windowClass.lpszClassName = _windowClassName.c_str();

        windowClass.lpfnWndProc = tempWindowProc;

        windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

        // Register the window class
        RegisterClassExW(&windowClass);

        // Create the actual window
        _hwnd = CreateWindowExW(0,
                                _windowClassName.c_str(),
                                _windowTitle.c_str(),
                                WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                                0, 0,
                                _windowWidth, _windowHeight,
                                nullptr,
                                nullptr,
                                _hInstance,
                                this);
    };

};