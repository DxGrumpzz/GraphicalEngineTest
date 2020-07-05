#include <Windows.h>

#include "Window.hpp"
#include "Graphics.hpp"



int windowWidth = 800;
int windowHeight = 500;

Graphics* graphics = new Graphics(windowWidth, windowHeight);


void DrawFrame()
{

    for (size_t x = 0; x < 50; x++)
    {
        for (size_t y = 0; y < 50; y++)
        {
            graphics->DrawPixel(x, y, {255, 0,0});
        };
    };

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