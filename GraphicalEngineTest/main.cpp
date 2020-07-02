#include <Windows.h>
#include <string>
#include <comdef.h>
#include <d3dcommon.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <windows.graphics.directx.direct3d11.interop.h>

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib,"d3d11.lib")

// A macro used to debug COM functions.
// Creates a scoped variable of type HRESULT and stores the retured function value,
// using COM's FAILED macro, checks if the result is valid or not
#define COM_CALL(function) _COM_CALL(function)              

void _COM_CALL(HRESULT hResult)
{
    if (FAILED(hResult))
    {
        _com_error err(hResult);

        size_t charactersConverted = 0;

        size_t errorMessageLength = wcslen(err.ErrorMessage()) + 1;
        char* errorMessageNarrow = new char[errorMessageLength + 0] { 0 };

        wcstombs_s(&charactersConverted, errorMessageNarrow, errorMessageLength, err.ErrorMessage(), errorMessageLength);

        throw std::exception(errorMessageNarrow);

        delete[] errorMessageNarrow;
        errorMessageNarrow = nullptr;
    }
}


// Takes a DWORD error code and returns its string message 
std::wstring GetErrorStringW(DWORD error)
{
    // Stores the error message as a string in memory
    LPWSTR buffer = nullptr;

    // Format DWORD error ID to a string 
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   error,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&buffer, 0, NULL);

    // Create std string from buffer
    std::wstring message(buffer);

    return message;
};


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        };

        case WM_TIMER:
        {
            return 0;
        };
    };

    return DefWindowProcW(hwnd, msg, wParam, lParam);
};



HWND CreateMainWindow(HINSTANCE hInstance, const wchar_t* windowClassName, const wchar_t* windowTitle)
{
    // Create the main window
    WNDCLASSEXW windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = windowClassName;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);


    // Register the window
    ATOM registerClassResult = RegisterClassExW(&windowClass);

    if (registerClassResult == 0)
    {
        std::wstring error = GetErrorStringW(GetLastError());
        error.insert(0, L"An error occured while creating window.\n");

        MessageBoxW(NULL, error.c_str(), NULL, NULL);

        return NULL;
    };


    // Create the actuall window
    HWND windowHWND = CreateWindowExW(NULL,
                                      windowClassName,
                                      windowTitle,
                                      WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX,
                                      0, 0,
                                      800, 500,
                                      NULL,
                                      NULL,
                                      hInstance,
                                      NULL);

    return windowHWND;
};




void EndFrame(Microsoft::WRL::ComPtr<IDXGISwapChain> dxgiSwapChain)
{
    dxgiSwapChain->Present(1, NULL);
};


void ClearBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>    d3dDeviceContext,
                 Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRendererTraget,
                 float red, float green, float blue)
{
    float colour[] =
    {
        red,
        green,
        blue,
        1.0f,
    };

    d3dDeviceContext->ClearRenderTargetView(d3dRendererTraget.Get(), colour);
};
              

void ProcessFrame(Microsoft::WRL::ComPtr<ID3D11DeviceContext>    d3dDeviceContext,
                  Microsoft::WRL::ComPtr<IDXGISwapChain>         dxgiSwapChain,
                  Microsoft::WRL::ComPtr<ID3D11Device>           d3dDevice,
                  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRendererTraget)
{
    Microsoft::WRL::ComPtr<ID3D11Resource> d3dBackBufferResource;
    COM_CALL(dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&d3dBackBufferResource)));

    COM_CALL(d3dDevice->CreateRenderTargetView(d3dBackBufferResource.Get(), nullptr, d3dRendererTraget.GetAddressOf()));

    ClearBuffer(d3dDeviceContext, d3dRendererTraget, 1.0f, 1.0f, 1.0f);

    EndFrame(dxgiSwapChain);
};



int WindowLoop(HWND hwnd,
               Microsoft::WRL::ComPtr<ID3D11DeviceContext>    d3dDeviceContext,
               Microsoft::WRL::ComPtr<IDXGISwapChain>         dxgiSwapChain,
               Microsoft::WRL::ComPtr<ID3D11Device>           d3dDevice,
               Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRendererTraget)
{
    // Windows message loop
    MSG message;
    bool processMessages = true;

    // Continuously try and get message
    while (processMessages)
    {
        // Peek message returns 1 if there a message is available, 
        // If there are none it will return 0.
        // So we continually loop as long as there are messages in queue
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            // To exit the infinite loop check if the current message was a quit message
            if (message.message == WM_QUIT)
            {
                processMessages = false;
                break;
            };

            // If the message is a keystroke get the key's character value
            TranslateMessage(&message);

            // Send the message to the Window procedure function
            DispatchMessageW(&message);
        };

        ProcessFrame(d3dDeviceContext, dxgiSwapChain, d3dDevice, d3dRendererTraget);
    };

    DestroyWindow(hwnd);
    return (int)message.wParam;
};



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    const wchar_t* windowClassName = L"Window class";
    const wchar_t* windowTitle = L"Title";

    HWND hwnd = CreateMainWindow(hInstance, windowClassName, windowTitle);

    // Show the main window
    ShowWindow(hwnd, nShowCmd);

    // create D3D device, responsible for creating resources
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;

    // Create D3D device context, the actual command dispatcher
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDeviceContext;

    // Create pointer to the swap chain
    Microsoft::WRL::ComPtr<IDXGISwapChain> dxgiSwapChain;


    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRendererTraget;

    // Craete dxgi factory
    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
    COM_CALL(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));


    // Get the first graphics adapter, usually the GPU
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    dxgiFactory->EnumAdapters(0, adapter.GetAddressOf());


    // A data descriptor for the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDescriptor = { 0 };
    swapChainDescriptor.Windowed = TRUE;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescriptor.BufferCount = 1;

    swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.SampleDesc.Quality = 0;

    swapChainDescriptor.OutputWindow = hwnd;


    // Create D3D device and swap chain
    COM_CALL(D3D11CreateDeviceAndSwapChain(adapter.Get(),
             // Setting to unkwon type because we give it an IAdapter
             D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_UNKNOWN,
             // A handle to a software rasterizer, unnecessary in this case
             nullptr,
             // Use D3D debug features
             D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG,
             // Use default supported D3D versions from D3D 9.1 to D3D 11.0 
             nullptr,
             // Zero because we pass nullptr to supported versions
             0,
             // Set development version
             D3D11_SDK_VERSION,
             &swapChainDescriptor,
             dxgiSwapChain.GetAddressOf(),
             d3dDevice.GetAddressOf(),
             nullptr,
             d3dDeviceContext.GetAddressOf()));


    int exitCode = WindowLoop(hwnd, d3dDeviceContext, dxgiSwapChain, d3dDevice, d3dRendererTraget);

    return exitCode;
};