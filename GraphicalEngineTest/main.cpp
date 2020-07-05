#include <Windows.h>

#include <cmath>

#include "Window.hpp"
#include "Graphics.hpp"



int windowWidth = 800;
int windowHeight = 500;

Graphics* graphics = new Graphics(windowWidth, windowHeight);


int SignNum(float number)
{
    if (number > 0)
        return 1;

    if (number < 0)
        return -1;

    return 0;
};

HWND hwnd;

void DrawLine(int x0, int y0, int x1, int y1)
{
    float deltaX = x1 - x0;
    float deltaY = y1 - y0;

    float deltaError = std::fabs(deltaY / deltaX);

    float error = 0.0f;
    int y = y0;

    for (int x = x0; x < x1; x++)
    {
        graphics->DrawPixel(x, y, { 255, 255, 255 }, false);

        error += deltaError;

        if (error >= 0.5f)
        {
            y += SignNum(deltaY) * 1;
            error -= 1.0f;
        };
    };
};

int x2 = 0;
int y2 = 0;

void DrawFrame()
{
    if (GetAsyncKeyState(VK_LBUTTON))
    {
        POINT p = { 0 };
        GetCursorPos(&p);
        ScreenToClient(hwnd, &p);

        x2 = p.x;
        y2 = p.y;
    };


    DrawLine(windowWidth /2, windowHeight /2 , x2, y2);
};



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    // Registered name of this window
    const wchar_t* windowClassName = L"DirectXWindow";

    // Title of this window
    const wchar_t* windowTitle = L"DirectX Window";

    Window* window = new Window(windowWidth, windowHeight,
                                hInstance,
                                windowClassName,
                                windowTitle);
    hwnd = window->GetHWND();

    // Setup graphics components
    graphics->SetupGraphics(window->GetHWND());

    // Show the window
    window->ShowWindow();


    while (window->ProcessMessageBuffer())
    {
        // Clear the frame before drawing again
        graphics->ClearFrame();

        // Draw frame function, should be only responsible for drawing, and simple branching logic
        DrawFrame();

        // Draw frame onto the screen and prepare for next frame
        graphics->EndFrame();
    };


    delete graphics;
    graphics = nullptr;

    delete window;
    window = nullptr;
};