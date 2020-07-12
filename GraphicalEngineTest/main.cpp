#include <Windows.h>

#include <cmath>
#include <chrono>
#include <vector>

#include "Window.hpp"
#include "Graphics.hpp"
#include "IScene.hpp"
#include "BitmapScene.hpp"



int windowWidth = 800;
int windowHeight = 500;

Graphics* graphics = new Graphics(windowWidth, windowHeight);
Window* window = nullptr;


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


std::vector<IScene*> scenes;
std::vector<IScene*>::iterator currentScene;



/// <summary>
/// Cycle between available scenes
/// </summary>
void CycleScences()
    {
    const Keyboard& keyboard = window->GetKeyboard();

    // If control is pressed 
    if (keyboard.GetKeyState(VK_CONTROL) == KeyState::Held)
    {
        // And user pressed right key 
        if (keyboard.GetKeyState(VK_RIGHT) == KeyState::Pressed)
        {
            // Cycle to the next scene
            currentScene++;

            // If it's the last scene
            if (currentScene == scenes.end())
            {
                // Go back to the beginning
                currentScene = scenes.begin();
            };
        };

        // If user pressed the left arrow
        if (keyboard.GetKeyState(VK_LEFT) == KeyState::Pressed)
        {
            // Check if we're at the beginning
            if (currentScene == scenes.begin())
        {
                // Go to the end scene
                currentScene = scenes.end() - 1;
            }
            else
                // Cycle to previous scene
                currentScene--;
        };
    };
};



void DrawFrame(float deltaTime)
    {
    CycleScences();

    (*currentScene)->UpdateScene(deltaTime);
    (*currentScene)->DrawScene();

};



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    // Registered name of this window
    const wchar_t* windowClassName = L"DirectXWindow";

    // Title of this window
    const wchar_t* windowTitle = L"DirectX Window";


    window = new Window(windowWidth, windowHeight,
                                hInstance,
                                windowClassName,
                                windowTitle);

    // Setup graphics components
    graphics->SetupGraphics(window->GetHWND());

    scenes.push_back(new BitmapScene(*graphics, *window));

    currentScene = scenes.end() - 1;

    // Show the window
    window->ShowWindow();


    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;

    std::chrono::duration<float > elapsed;

    float secondsAccumulator = 0.0f;


    int framesElapsed = 0;

    while (window->ProcessMessageBuffer())
    {
        elapsed = end - start;

        start = std::chrono::steady_clock::now();

        // Clear the frame before drawing again
        graphics->ClearFrame();

        secondsAccumulator += elapsed.count();


        if (secondsAccumulator > 0.5f)
        {
            float fps = framesElapsed / secondsAccumulator;

            SetWindowTextW(window->GetHWND(), std::to_wstring(fps).c_str());

            framesElapsed = 0;
            secondsAccumulator = 0;
        };


        // Draw frame function, should be only responsible for drawing, and simple branching logic
        DrawFrame(elapsed.count());

        // Draw frame onto the screen and prepare for next frame
        graphics->EndFrame();

        framesElapsed++;
        end = std::chrono::steady_clock::now();
    };


    delete graphics;
    graphics = nullptr;

    delete window;
    window = nullptr;
};