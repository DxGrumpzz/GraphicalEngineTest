#include <Windows.h>
#include <string>
#include <comdef.h>
#include <d3dcommon.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_3.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <D3DCompiler.h>

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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



/// <summary>
/// Simple pixel colour data structure
/// </summary>
struct Colour
{
    std::uint8_t Red;
    std::uint8_t Green;
    std::uint8_t Blue;

    std::uint8_t Alpha;
};

struct Vertex
{
    float X;
    float Y;
    float Z;

    float TextureX;
    float TextureY;
};




int windowWidth = 800;
int windowHeight = 500;


// create D3D device, responsible for creating resources
Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;

// Create D3D device context, the actual command dispatcher
Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDeviceContext;

// Create pointer to the swap chain
Microsoft::WRL::ComPtr<IDXGISwapChain> dxgiSwapChain;

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRendererTragetView;

Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d2DTexture;

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> d3dShaderResourceView;

Microsoft::WRL::ComPtr<ID3D11PixelShader> d3dPixelShader;

Microsoft::WRL::ComPtr<ID3D11VertexShader> d3dVertexShader;

Microsoft::WRL::ComPtr<ID3D11Buffer> d3dVertexBuffer;

Microsoft::WRL::ComPtr<ID3D11InputLayout> d3dInputLayout;

Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;

Microsoft::WRL::ComPtr<ID3D11SamplerState> d3dSamplerState;

D3D11_MAPPED_SUBRESOURCE d3dMappedSubResource = { 0 };

/// <summary>
/// The actual pixels that will be drawn on screen
/// </summary>
Colour* pixelData = new Colour[static_cast<std::size_t>(windowWidth) * static_cast<std::size_t>(windowHeight)];


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



HWND CreateMainWindow(HINSTANCE hInstance, const wchar_t* windowClassName, const wchar_t* windowTitle)
{
    // Create the main window
    WNDCLASSEXW windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_CLASSDC;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = windowClassName;
    windowClass.lpfnWndProc = WindowProcedure;
    windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    // Register the window
    RegisterClassExW(&windowClass);

    // Create the actuall window
    HWND windowHWND = CreateWindowExW(NULL,
                                      windowClassName,
                                      windowTitle,
                                      WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                                      0, 0,
                                      windowWidth, windowHeight,
                                      NULL,
                                      NULL,
                                      hInstance,
                                      NULL);

    return windowHWND;
};





void UpdateFrame()
{
};


void DrawFrame()
{

    for (size_t x = 0; x < 50; x++)
    {
        for (size_t y = 0; y < 50; y++)
        {
            pixelData[x + windowWidth * y] = { 255, 0, 0 };
        };
    };

};


void EndFrame()
{
    COM_CALL(d3dDeviceContext->Map(d3d2DTexture.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedSubResource));

    Colour* pDst = reinterpret_cast<Colour*>(d3dMappedSubResource.pData);

    std::size_t destinationPitch = d3dMappedSubResource.RowPitch / sizeof(Colour);
    std::size_t srcPitch = windowWidth;
    std::size_t rowBytes = srcPitch * sizeof(Colour);

    for (size_t y = 0u; y < windowHeight; y++)
    {
        memcpy(&pDst[y * destinationPitch], &pixelData[y * srcPitch], rowBytes);
    };


    d3dDeviceContext->Unmap(d3d2DTexture.Get(), 0);


    d3dDeviceContext->IASetInputLayout(d3dInputLayout.Get());
    d3dDeviceContext->VSSetShader(d3dVertexShader.Get(), nullptr, 0u);
    d3dDeviceContext->PSSetShader(d3dPixelShader.Get(), nullptr, 0u);
    d3dDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;

    d3dDeviceContext->IASetVertexBuffers(0u, 1u, d3dVertexBuffer.GetAddressOf(), &stride, &offset);
    d3dDeviceContext->PSSetShaderResources(0u, 1u, d3dShaderResourceView.GetAddressOf());
    d3dDeviceContext->PSSetSamplers(0u, 1u, d3dSamplerState.GetAddressOf());

    d3dDeviceContext->Draw(6u, 0u);


    COM_CALL(dxgiSwapChain->Present(1, NULL));
};




void CreateD3D2DTexture(D3D11_TEXTURE2D_DESC& d3d2DTextureDescriptor)
{
    d3d2DTextureDescriptor.Width = windowWidth;
    d3d2DTextureDescriptor.Height = windowHeight;

    d3d2DTextureDescriptor.MipLevels = 1;

    d3d2DTextureDescriptor.ArraySize = 1;

    d3d2DTextureDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

    d3d2DTextureDescriptor.SampleDesc.Count = 1;
    d3d2DTextureDescriptor.SampleDesc.Quality = 0;

    d3d2DTextureDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;

    d3d2DTextureDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

    d3d2DTextureDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

    d3d2DTextureDescriptor.MiscFlags = 0;


    COM_CALL(d3dDevice->CreateTexture2D(&d3d2DTextureDescriptor, nullptr, d3d2DTexture.GetAddressOf()));
};


void CreateShaderResourceView(const D3D11_TEXTURE2D_DESC& d3d2DTextureDescriptor)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDescriptor = { };

    d3dShaderResourceViewDescriptor.Format = d3d2DTextureDescriptor.Format;
    d3dShaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
    d3dShaderResourceViewDescriptor.Texture2D.MipLevels = 1;

    COM_CALL(d3dDevice->CreateShaderResourceView(d3d2DTexture.Get(), &d3dShaderResourceViewDescriptor, d3dShaderResourceView.GetAddressOf()));
};


void CreatePixelShader()
{
    const char* pixelShaderTargetProfile = "ps_5_0";
    const char* pixelShaderEntryPoint = "main";
    const wchar_t* pixelShaderFileName = L"DefaultPixelShader.hlsl";

    int pixelShaderFlags = D3DCOMPILE_DEBUG;

    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    COM_CALL(D3DCompileFromFile(pixelShaderFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pixelShaderEntryPoint, pixelShaderTargetProfile, pixelShaderFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));

    COM_CALL(d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, d3dPixelShader.GetAddressOf()));
};



void CreateVertexShader()
{
    const char* vertexShaderTargetProfile = "vs_5_0";
    const char* vertexShaderEntryPoint = "main";
    const wchar_t* vertexShaderFileName = L"DefaultVertexShader.hlsl";

    int pixelShaderFlags = D3DCOMPILE_DEBUG;

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    COM_CALL(D3DCompileFromFile(vertexShaderFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, vertexShaderEntryPoint, vertexShaderTargetProfile, pixelShaderFlags, 0, vertexShaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));

    COM_CALL(d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, d3dVertexShader.GetAddressOf()));
}



void CreateBuffer()
{
    const Vertex vertices[] =
    {
        { -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
        {  1.0f,  1.0f, 0.5f, 1.0f, 0.0f },
        {  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
        { -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
        {  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
    };

    unsigned int numberOfVertices = ARRAYSIZE(vertices);

    D3D11_BUFFER_DESC d3dBufferDescriptor = { 0 };
    d3dBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
    d3dBufferDescriptor.ByteWidth = sizeof(Vertex) * numberOfVertices;
    d3dBufferDescriptor.CPUAccessFlags = 0;
    d3dBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA d3dSubResourceData = { 0 };
    d3dSubResourceData.pSysMem = vertices;

    COM_CALL(d3dDevice->CreateBuffer(&d3dBufferDescriptor, &d3dSubResourceData, d3dVertexBuffer.GetAddressOf()));
};



void CreateInputLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    COM_CALL(d3dDevice->CreateInputLayout(inputElementDescriptor,
             // Passing 2 because the Vertex shader expects 2 arguments
             2,
             vertexShaderBlob->GetBufferPointer(),
             vertexShaderBlob->GetBufferSize(),
             d3dInputLayout.GetAddressOf()));
};


void CreateSamplerState()
{
    D3D11_SAMPLER_DESC smaplerDescriptor;
    smaplerDescriptor.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    smaplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    smaplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    smaplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    smaplerDescriptor.MipLODBias = 0;
    smaplerDescriptor.MaxAnisotropy = 0;

    smaplerDescriptor.ComparisonFunc = D3D11_COMPARISON_NEVER;

    smaplerDescriptor.MinLOD = 0;
    smaplerDescriptor.MaxLOD = D3D11_FLOAT32_MAX;

    COM_CALL(d3dDevice->CreateSamplerState(&smaplerDescriptor, d3dSamplerState.GetAddressOf()));
};


void CreateDeviceAndSwapChain(HWND hwnd)
{

    // Craete dxgi factory
    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
    COM_CALL(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));


    // Get the first graphics adapter, usually the GPU
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    COM_CALL(dxgiFactory->EnumAdapters(0, adapter.GetAddressOf()));



    // A data descriptor for the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDescriptor = { 0 };
    swapChainDescriptor.Windowed = TRUE;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescriptor.BufferCount = 1;

    swapChainDescriptor.BufferDesc.Width = windowWidth;
    swapChainDescriptor.BufferDesc.Height = windowHeight;
    swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 1;
    swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 60;

    swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;


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

};


void SetupDirectX(HWND hwnd)
{
    CreateDeviceAndSwapChain(hwnd);


    Microsoft::WRL::ComPtr<ID3D11Resource> d3dBackBufferResource;
    COM_CALL(dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(d3dBackBufferResource.GetAddressOf())));


    COM_CALL(d3dDevice->CreateRenderTargetView(d3dBackBufferResource.Get(), nullptr, d3dRendererTragetView.GetAddressOf()));

    d3dDeviceContext->OMSetRenderTargets(1, d3dRendererTragetView.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport = { 0 };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    viewport.Width = static_cast<float>(windowWidth);
    viewport.Height = static_cast<float>(windowHeight);

    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;

    d3dDeviceContext->RSSetViewports(1, &viewport);


    D3D11_TEXTURE2D_DESC d3d2DTextureDescriptor = { 0 };
    CreateD3D2DTexture(d3d2DTextureDescriptor);

    CreateShaderResourceView(d3d2DTextureDescriptor);

    CreatePixelShader();

    CreateVertexShader();

    CreateBuffer();

    CreateInputLayout();

    CreateSamplerState();

    pixelData = new Colour[static_cast<std::size_t>(windowWidth) * static_cast<std::size_t>(windowHeight)];
};



int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    // Registered name of this window
    const wchar_t* windowClassName = L"DirectXWindow";

    // Title of this window
    const wchar_t* windowTitle = L"DirectX Window";

    // Create the main window
    HWND hwnd = CreateMainWindow(hInstance, windowClassName, windowTitle);

    // Show the main window
    ShowWindow(hwnd, nShowCmd);

    // Setup directX components
    SetupDirectX(hwnd);


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

        // Clear pixel buffer
        memset(pixelData, 0, (static_cast<std::size_t>(windowWidth) * static_cast<std::size_t>(windowHeight)) * sizeof(Colour));

        // Update frame function, responsible for input, collision hanlding and such
        UpdateFrame();
        // Draw frame function, should be only responsible for drawing, and simple branching logic
        DrawFrame();

        // End frame and prepare for next one
        EndFrame();
    };


    UnregisterClassW(windowClassName, hInstance);

    return (int)message.wParam;
};