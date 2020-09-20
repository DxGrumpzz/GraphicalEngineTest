#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

#include "WindowsUtilities.hpp"
#include "Colour.hpp"
#include "Vertex.hpp"

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")


/// <summary>
/// A class responsible for drawing frames on screen
/// </summary>
class Graphics
{
private:


    // create D3D device, responsible for creating resources
    Microsoft::WRL::ComPtr<ID3D11Device> _d3dDevice;

    // Create D3D device context, the actual command dispatcher
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> _d3dDeviceContext;

    // Create pointer to the swap chain
    Microsoft::WRL::ComPtr<IDXGISwapChain> _dxgiSwapChain;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _d3dRendererTragetView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> _d3d2DTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _d3dShaderResourceView;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> _d3dPixelShader;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> _d3dVertexShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> _d3dVertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> _d3dInputLayout;

    Microsoft::WRL::ComPtr<ID3DBlob> _vertexShaderBlob;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> _d3dSamplerState;

    D3D11_MAPPED_SUBRESOURCE _d3dMappedSubResource = { 0 };

    /// <summary>
    /// The actual pixels that will be drawn on screen
    /// </summary>
    Colour* _pixelData;

    int _windowWidth;
    int _windowHeight;

public:

    Graphics(int windowWidth, int windowHeight) :
        _windowWidth(windowWidth),
        _windowHeight(windowHeight)
    {

    };


public:

    void SetupGraphics(HWND hwnd)
    {
        CreateDeviceAndSwapChain(hwnd);


        Microsoft::WRL::ComPtr<ID3D11Resource> d3dBackBufferResource;
        WindowsUtilities::COM_CALL(_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(d3dBackBufferResource.GetAddressOf())));


        WindowsUtilities::COM_CALL(_d3dDevice->CreateRenderTargetView(d3dBackBufferResource.Get(), nullptr, _d3dRendererTragetView.GetAddressOf()));

        
        _d3dDeviceContext->OMSetRenderTargets(1, _d3dRendererTragetView.GetAddressOf(), nullptr);

        D3D11_VIEWPORT viewport = { 0 };
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        viewport.Width = static_cast<float>(_windowWidth);
        viewport.Height = static_cast<float>(_windowHeight);

        viewport.MinDepth = 0;
        viewport.MaxDepth = 1;

        _d3dDeviceContext->RSSetViewports(1, &viewport);


        D3D11_TEXTURE2D_DESC d3d2DTextureDescriptor = { 0 };
        CreateD3D2DTexture(d3d2DTextureDescriptor);

        CreateShaderResourceView(d3d2DTextureDescriptor);

        CreatePixelShader();

        CreateVertexShader();

        CreateBuffer();

        CreateInputLayout();

        CreateSamplerState();

        _pixelData = new Colour[static_cast<std::size_t>(_windowWidth) * static_cast<std::size_t>(_windowHeight)];
    };


    void ClearFrame()
    {
        // Clear pixel buffer
        memset(_pixelData, 0, (static_cast<std::size_t>(_windowWidth) * static_cast<std::size_t>(_windowHeight)) * sizeof(Colour));
    };


    void EndFrame()
    {
        WindowsUtilities::COM_CALL(_d3dDeviceContext->Map(_d3d2DTexture.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_d3dMappedSubResource));

        Colour* pDst = reinterpret_cast<Colour*>(_d3dMappedSubResource.pData);

        std::size_t destinationPitch = _d3dMappedSubResource.RowPitch / sizeof(Colour);
        std::size_t srcPitch = _windowWidth;
        std::size_t rowBytes = srcPitch * sizeof(Colour);

        for (size_t y = 0u; y < _windowHeight; y++)
        {
            memcpy(&pDst[y * destinationPitch], &_pixelData[y * srcPitch], rowBytes);
        };


        _d3dDeviceContext->Unmap(_d3d2DTexture.Get(), 0);


        _d3dDeviceContext->IASetInputLayout(_d3dInputLayout.Get());
        _d3dDeviceContext->VSSetShader(_d3dVertexShader.Get(), nullptr, 0u);
        _d3dDeviceContext->PSSetShader(_d3dPixelShader.Get(), nullptr, 0u);
        _d3dDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        const UINT stride = sizeof(Vertex);
        const UINT offset = 0u;

        _d3dDeviceContext->IASetVertexBuffers(0u, 1u, _d3dVertexBuffer.GetAddressOf(), &stride, &offset);
        _d3dDeviceContext->PSSetShaderResources(0u, 1u, _d3dShaderResourceView.GetAddressOf());
        _d3dDeviceContext->PSSetSamplers(0u, 1u, _d3dSamplerState.GetAddressOf());

        _d3dDeviceContext->Draw(6u, 0u);


        // Present the frame
        WindowsUtilities::COM_CALL(_dxgiSwapChain->Present(
            // Don't sync frame to any frame interval, run at fastest speed possible
            0, 
            // Present a frame from each buffer available
            0));
    };


    void DrawPixel(int x, int y, const Colour& pixelColour = { 255, 255, 255, 1 }, bool checkBounds = true)
    {
        if (x < 0 || x >= _windowWidth)
        {
            if (checkBounds == true)
               throw std::exception("X is out of bounds");
            else
                return;
        }
        else if (y < 0 || y >= _windowHeight)
        {
            if (checkBounds == true)
                throw std::exception("Y is out of bounds");
            else
                return;
        };

        Colour& pixel = GetPixel(x, y);
        pixel = pixelColour;
    };


    /// <summary>
    /// Draw a line segment between 2 points
    /// </summary>
    /// <param name="p0"></param>
    /// <param name="p1"></param>
    /// <param name="colour"></param>
    void DrawLine(Vector2D p0, Vector2D p1, Colour colour = { 255,255,255 }, bool checkBounds = true)
    {
        // Left "leaning" line
        if (p0.X >= p1.X)
        {
            float run = p0.X - p1.X;
            float rise = p0.Y - p1.Y;

            // If the rise starts getting larger than the run the line starts to break up, 
            // so we check when that happens and use "inverses" to draw the new line
            // Instead of y-Intercept we use x-Intercept, and replace x's with y's positions
            if (std::abs(rise) > std::abs(run))
            {
                float slope = run / rise;

                float xIntercept = p0.X - slope * p0.Y;

                if (p0.Y > p1.Y)
                    std::swap(p0, p1);

                float x = 0;

                for (int y = static_cast<int>(p0.Y); y < p1.Y; y++)
                {
                    x = slope * y + xIntercept;

                    DrawPixel(static_cast<int>(x), static_cast<int>(y), colour, checkBounds);
                };
            }
            else
            {
                float slope = rise / run;

                float yIntercept = p0.Y - slope * p0.X;

                float y = 0;

                for (int x = static_cast<int>(p1.X); x < p0.X; x++)
                {
                    y = slope * x + yIntercept;

                    DrawPixel(static_cast<int>(x), static_cast<int>(y), colour, checkBounds);
                };
            };
        }
        // Right "leaning" line
        else
        {
            float run = p1.X - p0.X;
            float rise = p1.Y - p0.Y;

            // If the rise starts getting larger than the run the line starts to break up, 
            // so we check when that happens and use "inverses" to draw the new line
            // Instead of y-Intercept we use x-Intercept, and replace x's with y's positions
            if (std::abs(rise) > std::abs(run))
            {
                float slope = run / rise;

                float xIntercept = p0.X - slope * p0.Y;

                if (p0.Y > p1.Y)
                    std::swap(p0, p1);

                float x = 0;

                for (int y = static_cast<int>(p0.Y); y < p1.Y; y++)
                {
                    x = slope * y + xIntercept;

                    DrawPixel(static_cast<int>(x), static_cast<int>(y), colour, checkBounds);
                };
            }
            else
            {
                float slope = rise / run;

                float yIntercept = p0.Y - slope * p0.X;

                float y = 0;

                for (int x = static_cast<int>(p0.X); x < p1.X; x++)
                {
                    y = slope * x + yIntercept;

                    DrawPixel(static_cast<int>(x), static_cast<int>(y), colour, checkBounds);
                };
            };
        };

    };

    
    /// <summary>
    /// Get number of pixels 
    /// </summary>
    /// <returns></returns>
    std::size_t GetPixelsCount()
    {
        return static_cast<std::size_t>(_windowHeight) * static_cast<std::size_t>(_windowWidth);
    };

    /// <summary>
    /// Get a pointer to the beggning of the pixels array
    /// </summary>
    /// <returns></returns>
    Colour* GetPixels()
    {
        return _pixelData;
    };

    /// <summary>
    /// Get a pixel based on x and y position
    /// </summary>
    /// <returns></returns>
    Colour& GetPixel(int x, int y)
    {
        // Bounds check
        if ((x > _windowWidth || x < 0) || 
            (y > _windowWidth || y < 0))
        {
            WindowsUtilities::ShowWinErrorW(L"Error. GetPixel() tried to access out of bounds memory");
            DebugBreak();
        };

        size_t pixelDataIndexer = x + _windowWidth * y;

        Colour& pixel = _pixelData[pixelDataIndexer];

        return pixel;
    };


private:

    void CreateD3D2DTexture(D3D11_TEXTURE2D_DESC& d3d2DTextureDescriptor)
    {
        d3d2DTextureDescriptor.Width = _windowWidth;
        d3d2DTextureDescriptor.Height = _windowHeight;

        d3d2DTextureDescriptor.MipLevels = 1;

        d3d2DTextureDescriptor.ArraySize = 1;

        d3d2DTextureDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

        d3d2DTextureDescriptor.SampleDesc.Count = 1;
        d3d2DTextureDescriptor.SampleDesc.Quality = 0;

        d3d2DTextureDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;

        d3d2DTextureDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

        d3d2DTextureDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

        d3d2DTextureDescriptor.MiscFlags = 0;


        WindowsUtilities::COM_CALL(_d3dDevice->CreateTexture2D(&d3d2DTextureDescriptor, nullptr, _d3d2DTexture.GetAddressOf()));
    };


    void CreateShaderResourceView(const D3D11_TEXTURE2D_DESC& d3d2DTextureDescriptor)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDescriptor = { };

        d3dShaderResourceViewDescriptor.Format = d3d2DTextureDescriptor.Format;
        d3dShaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
        d3dShaderResourceViewDescriptor.Texture2D.MipLevels = 1;

        WindowsUtilities::COM_CALL(_d3dDevice->CreateShaderResourceView(_d3d2DTexture.Get(), &d3dShaderResourceViewDescriptor, _d3dShaderResourceView.GetAddressOf()));
    };


    void CreatePixelShader()
    {
        const char* pixelShaderTargetProfile = "ps_5_0";
        const char* pixelShaderEntryPoint = "main";
        const wchar_t* pixelShaderFileName = L"DefaultPixelShader.hlsl";

        int pixelShaderFlags = D3DCOMPILE_DEBUG;

        Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob = nullptr;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

        WindowsUtilities::COM_CALL(D3DCompileFromFile(pixelShaderFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, pixelShaderEntryPoint, pixelShaderTargetProfile, pixelShaderFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));

        WindowsUtilities::COM_CALL(_d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, _d3dPixelShader.GetAddressOf()));
    };



    void CreateVertexShader()
    {
        const char* vertexShaderTargetProfile = "vs_5_0";
        const char* vertexShaderEntryPoint = "main";
        const wchar_t* vertexShaderFileName = L"DefaultVertexShader.hlsl";

        int pixelShaderFlags = D3DCOMPILE_DEBUG;

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        WindowsUtilities::COM_CALL(D3DCompileFromFile(vertexShaderFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, vertexShaderEntryPoint, vertexShaderTargetProfile, pixelShaderFlags, 0, _vertexShaderBlob.GetAddressOf(), errorBlob.GetAddressOf()));

        WindowsUtilities::COM_CALL(_d3dDevice->CreateVertexShader(_vertexShaderBlob->GetBufferPointer(), _vertexShaderBlob->GetBufferSize(), nullptr, _d3dVertexShader.GetAddressOf()));
    };



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

        WindowsUtilities::COM_CALL(_d3dDevice->CreateBuffer(&d3dBufferDescriptor, &d3dSubResourceData, _d3dVertexBuffer.GetAddressOf()));
    };


    void CreateInputLayout()
    {
        D3D11_INPUT_ELEMENT_DESC inputElementDescriptor[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        WindowsUtilities::COM_CALL(_d3dDevice->CreateInputLayout(inputElementDescriptor,
                                   // Passing 2 because the Vertex shader expects 2 arguments
                                   2,
                                   _vertexShaderBlob->GetBufferPointer(),
                                   _vertexShaderBlob->GetBufferSize(),
                                   _d3dInputLayout.GetAddressOf()));
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

        WindowsUtilities::COM_CALL(_d3dDevice->CreateSamplerState(&smaplerDescriptor, _d3dSamplerState.GetAddressOf()));
    };


    void CreateDeviceAndSwapChain(HWND hwnd)
    {

        // Craete dxgi factory
        Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
        WindowsUtilities::COM_CALL(CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));


        // Get the first graphics adapter, usually the GPU
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        WindowsUtilities::COM_CALL(dxgiFactory->EnumAdapters(0, adapter.GetAddressOf()));



        // A data descriptor for the swap chain
        DXGI_SWAP_CHAIN_DESC swapChainDescriptor = { 0 };
        swapChainDescriptor.Windowed = TRUE;
        swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
        swapChainDescriptor.BufferCount = 1;

        swapChainDescriptor.BufferDesc.Width = _windowWidth;
        swapChainDescriptor.BufferDesc.Height = _windowHeight;
        swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 1;
        swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 60;

        swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;


        swapChainDescriptor.SampleDesc.Count = 1;
        swapChainDescriptor.SampleDesc.Quality = 0;

        swapChainDescriptor.OutputWindow = hwnd;
        
        // Create D3D device and swap chain
        WindowsUtilities::COM_CALL(D3D11CreateDeviceAndSwapChain(adapter.Get(),
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
                                   _dxgiSwapChain.GetAddressOf(),
                                   _d3dDevice.GetAddressOf(),
                                   nullptr,
                                   _d3dDeviceContext.GetAddressOf()));
    };

};