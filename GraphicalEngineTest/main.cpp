#include <Windows.h>
#include <chrono>
#include <cmath>
#include <vector>
#include <string>
#include <cstring>

#include "Window.hpp"
#include "Graphics.hpp"

#include "IScene.hpp"

#include "BitmapScene.hpp"
#include "GraphScene.hpp"
#include "RayCasterScene.hpp"
#include "LightTestScene.hpp"
#include "RasterScene.hpp"

int windowWidth = 800;
int windowHeight = 600;

Window* window = nullptr;



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


void ShowFPS(float& elapsedFramesSeconds, int& elapsedFrames)
{
    // If enough time has elapsed...
    if (elapsedFramesSeconds > 0.7f)
    {
        // Get FPS by dividing the number of elapsed frames and elapsed time
        float fps = elapsedFrames / elapsedFramesSeconds;

        // Format the FPS into a string
        wchar_t str[127];
        std::swprintf(str, 127, L"FPS: %.2f", fps);

        // Show the fps
        SetWindowTextW(window->GetHWND(), str);

        elapsedFrames = 0;
        elapsedFramesSeconds = 0;
    };
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

    Graphics* graphics = new Graphics(windowWidth, windowHeight);

    // Setup graphics components
    graphics->SetupGraphics(window->GetHWND());

    scenes.push_back(new GraphScene(*graphics, *window));
    scenes.push_back(new BitmapScene(*graphics, *window));
    scenes.push_back(new LightTestScene(*graphics, *window));
    scenes.push_back(new RayCasterScene(*graphics, *window));
    scenes.push_back(new RasterScene(*graphics, *window));


    currentScene = scenes.end() - 1;


    // Show the window
    window->ShowWindow();

    using namespace WindowsUtilities;
    
    // Set default cursor, if we don't do this the default cursor will be IDC_APPSTARTING
    WINCALL(SetCursor(WINCALL(LoadCursorW(NULL, IDC_ARROW))));
    
    // Time points for start, and the end of the "game" loop
    std::chrono::system_clock::time_point beginning;
    std::chrono::system_clock::time_point end;

    // Elapsed time as a float
    std::chrono::duration<float> elapedTime;

    // Counter for elapsed frames
    int elapsedFrames = 0;

    // Counter for elapsed seconds, used to check if enough time has passed to show FPS
    float elapsedFramesSeconds = 0;

    while (window->ProcessMessageBuffer())
    {
        // Set up elpased time and frames
        elapedTime = end - beginning;
        elapsedFramesSeconds += elapedTime.count();
        elapsedFrames++;

        // Restart the clock
        beginning = std::chrono::system_clock::now();

        // Clear the frame before drawing again
        graphics->ClearFrame();

        // Display frames per second
        ShowFPS(elapsedFramesSeconds, elapsedFrames);

        // Draw frame function, should be only responsible for drawing, and simple branching logic
        DrawFrame(elapedTime.count());

        // Draw frame onto the screen and prepare for next frame
        graphics->EndFrame();

        // Get time that has passed since the beggining of the loop
        end = std::chrono::system_clock::now();
    };


    delete graphics;
    graphics = nullptr;

    delete window;
    window = nullptr;
};