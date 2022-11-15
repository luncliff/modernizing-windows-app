#include "pch.h"

#include "D3D1211On12.h"

void D3D1211on12::OnInit(HWND hwnd, UINT width, UINT height) {
  m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), //
                                static_cast<float>(height));
  m_scissorRect = CD3DX12_RECT(0, 0,
                               static_cast<LONG>(width), //
                               static_cast<LONG>(height));
  LoadPipeline(hwnd, width, height);
  LoadAssets();
}

// Load the rendering pipeline dependencies.
void D3D1211on12::LoadPipeline(HWND hwnd, UINT width, UINT height) {
  UINT dxgiFactoryFlags = 0;
  UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

#if defined(_DEBUG)
  // Enable the debug layer (requires the Graphics Tools "optional feature").
  // NOTE: Enabling the debug layer after device creation will invalidate the
  // active device.
  {
    winrt::com_ptr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
      debugController->EnableDebugLayer();

      // Enable additional debug layers.
      dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
      d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
      d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    }
  }
#endif

  winrt::com_ptr<IDXGIFactory4> factory;
  winrt::check_hresult(
      CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

  if (false) {
    winrt::com_ptr<IDXGIAdapter> warpAdapter;
    winrt::check_hresult(factory->EnumWarpAdapter(__uuidof(IDXGIAdapter),
                                                  warpAdapter.put_void()));

    winrt::check_hresult(D3D12CreateDevice(warpAdapter.get(),
                                           D3D_FEATURE_LEVEL_11_0,
                                           IID_PPV_ARGS(&m_d3d12Device)));
  } else {
    winrt::com_ptr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.get(), hardwareAdapter.put(), true);

    winrt::check_hresult(D3D12CreateDevice(hardwareAdapter.get(),
                                           D3D_FEATURE_LEVEL_11_0,
                                           IID_PPV_ARGS(&m_d3d12Device)));
  }

#if defined(_DEBUG)
  // Filter a debug error coming from the 11on12 layer.
  winrt::com_ptr<ID3D12InfoQueue> infoQueue;
  if (SUCCEEDED(m_d3d12Device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
    // Suppress whole categories of messages.
    // D3D12_MESSAGE_CATEGORY categories[] = {};

    // Suppress messages based on their severity level.
    D3D12_MESSAGE_SEVERITY severities[] = {
        D3D12_MESSAGE_SEVERITY_INFO,
    };

    // Suppress individual messages by their ID.
    D3D12_MESSAGE_ID denyIds[] = {
        // This occurs when there are uninitialized descriptors in a descriptor
        // table, even when a shader does not access the missing descriptors.
        D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
    };

    D3D12_INFO_QUEUE_FILTER filter = {};
    // filter.DenyList.NumCategories = _countof(categories);
    // filter.DenyList.pCategoryList = categories;
    filter.DenyList.NumSeverities = _countof(severities);
    filter.DenyList.pSeverityList = severities;
    filter.DenyList.NumIDs = _countof(denyIds);
    filter.DenyList.pIDList = denyIds;

    winrt::check_hresult(infoQueue->PushStorageFilter(&filter));
  }
#endif

  // Describe and create the command queue.
  D3D12_COMMAND_QUEUE_DESC queueDesc = {};
  queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

  winrt::check_hresult(m_d3d12Device->CreateCommandQueue(
      &queueDesc, IID_PPV_ARGS(&m_commandQueue)));
  SetName(m_commandQueue.get(), L"m_commandQueue");

  // Describe the swap chain.
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
  swapChainDesc.BufferCount = FrameCount;
  swapChainDesc.Width = width;
  swapChainDesc.Height = height;
  swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.SampleDesc.Count = 1;

  winrt::com_ptr<IDXGISwapChain1> swapChain;
  // Swap chain needs the queue so that it can force a flush on it.
  winrt::check_hresult(factory->CreateSwapChainForHwnd(
      m_commandQueue.get(), hwnd, &swapChainDesc, nullptr, nullptr,
      swapChain.put()));

  // This sample does not support fullscreen transitions.
  winrt::check_hresult(
      factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

  winrt::check_hresult(swapChain->QueryInterface(m_swapChain.put()));
  m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

  // Create an 11 device wrapped around the 12 device and share
  // 12's command queue.
  winrt::com_ptr<ID3D11Device> d3d11Device;
  IUnknown* cq = m_commandQueue.get();
  winrt::check_hresult(D3D11On12CreateDevice(
      m_d3d12Device.get(), d3d11DeviceFlags, nullptr, 0, &cq, 1, 0,
      d3d11Device.put(), m_d3d11DeviceContext.put(), nullptr));

  // Query the 11On12 device from the 11 device.
  winrt::check_hresult(d3d11Device->QueryInterface(m_d3d11On12Device.put()));

  // Create D2D/DWrite components.
  {
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions =
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
    winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                           d2dFactoryOptions,
                                           m_d2dFactory.put()));
    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    winrt::check_hresult(m_d3d11On12Device->QueryInterface(dxgiDevice.put()));
    winrt::check_hresult(
        m_d2dFactory->CreateDevice(dxgiDevice.get(), m_d2dDevice.put()));
    winrt::check_hresult(m_d2dDevice->CreateDeviceContext(
        deviceOptions, m_d2dDeviceContext.put()));
    winrt::check_hresult(DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(m_dWriteFactory.put())));
  }

  // Query the desktop's dpi settings, which will be used to create
  // D2D's render targets.
  float dpiX;
  float dpiY;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
  m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)
  D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
      D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
      D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
      dpiX, dpiY);

  // Create descriptor heaps.
  {
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    winrt::check_hresult(m_d3d12Device->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    m_rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  }

  // Create frame resources.
  {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    // Create a RTV, D2D render target, and a command allocator for each frame.
    for (UINT n = 0; n < FrameCount; n++) {
      winrt::check_hresult(
          m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
      m_d3d12Device->CreateRenderTargetView(m_renderTargets[n].get(), nullptr,
                                            rtvHandle);

      NAME_D3D12_OBJECT_INDEXED(m_renderTargets, n);

      // Create a wrapped 11On12 resource of this back buffer. Since we are
      // rendering all D3D12 content first and then all D2D content, we specify
      // the In resource state as RENDER_TARGET - because D3D12 will have last
      // used it in this state - and the Out resource state as PRESENT. When
      // ReleaseWrappedResources() is called on the 11On12 device, the resource
      // will be transitioned to the PRESENT state.
      D3D11_RESOURCE_FLAGS d3d11Flags = {D3D11_BIND_RENDER_TARGET};
      winrt::check_hresult(m_d3d11On12Device->CreateWrappedResource(
          m_renderTargets[n].get(), &d3d11Flags,
          D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
          IID_PPV_ARGS(&m_wrappedBackBuffers[n])));

      // Create a render target for D2D to draw directly to this back buffer.
      winrt::com_ptr<IDXGISurface> surface;
      winrt::check_hresult(
          m_wrappedBackBuffers[n]->QueryInterface(surface.put()));
      winrt::check_hresult(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
          surface.get(), &bitmapProperties, m_d2dRenderTargets[n].put()));

      rtvHandle.Offset(1, m_rtvDescriptorSize);

      winrt::check_hresult(m_d3d12Device->CreateCommandAllocator(
          D3D12_COMMAND_LIST_TYPE_DIRECT,
          IID_PPV_ARGS(&m_commandAllocators[n])));
    }
  }
}

// Load the sample assets.
void D3D1211on12::LoadAssets() {
  // Create an empty root signature.
  {
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(
        0, nullptr, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    winrt::com_ptr<ID3DBlob> signature;
    winrt::com_ptr<ID3DBlob> error;
    winrt::check_hresult(D3D12SerializeRootSignature(
        &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.put(),
        error.put()));
    winrt::check_hresult(m_d3d12Device->CreateRootSignature(
        0, signature->GetBufferPointer(), signature->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)));
    NAME_D3D12_OBJECT(m_rootSignature);
  }

  // Create the pipeline state, which includes compiling and loading shaders.
  {
    winrt::com_ptr<ID3DBlob> vertexShader;
    winrt::com_ptr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    winrt::check_hresult(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr,
                                            "VSMain", "vs_5_0", compileFlags, 0,
                                            vertexShader.put(), nullptr));
    winrt::check_hresult(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr,
                                            "PSMain", "ps_5_0", compileFlags, 0,
                                            pixelShader.put(), nullptr));

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
    psoDesc.pRootSignature = m_rootSignature.get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    winrt::check_hresult(m_d3d12Device->CreateGraphicsPipelineState(
        &psoDesc, IID_PPV_ARGS(&m_pipelineState)));
    NAME_D3D12_OBJECT(m_pipelineState);
  }

  winrt::check_hresult(m_d3d12Device->CreateCommandList(
      0, D3D12_COMMAND_LIST_TYPE_DIRECT,
      m_commandAllocators[m_frameIndex].get(), m_pipelineState.get(),
      IID_PPV_ARGS(&m_commandList)));
  NAME_D3D12_OBJECT(m_commandList);

  // Create D2D/DWrite objects for rendering text.
  {
    winrt::check_hresult(m_d2dDeviceContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black), m_textBrush.put()));
    winrt::check_hresult(m_dWriteFactory->CreateTextFormat(
        L"Verdana", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 50, L"en-us", m_textFormat.put()));
    winrt::check_hresult(
        m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
    winrt::check_hresult(
        m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
  }

  // Note: ComPtr's are CPU objects but this resource needs to stay in scope
  // until the command list that references it has finished executing on the
  // GPU. We will flush the GPU at the end of this method to ensure the resource
  // is not prematurely destroyed.
  winrt::com_ptr<ID3D12Resource> vertexBufferUpload;

  // Create the vertex buffer.
  {
    // Define the geometry for a triangle.
    float m_aspectRatio = 1.0f;
    Vertex triangleVertices[] = {
        {{0.0f, 0.25f * m_aspectRatio, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.25f, -0.25f * m_aspectRatio, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};

    const UINT vertexBufferSize = sizeof(triangleVertices);

    winrt::check_hresult(m_d3d12Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));

    winrt::check_hresult(m_d3d12Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&vertexBufferUpload)));

    NAME_D3D12_OBJECT(m_vertexBuffer);

    // Copy data to the intermediate upload heap and then schedule a copy
    // from the upload heap to the vertex buffer.
    D3D12_SUBRESOURCE_DATA vertexData = {};
    vertexData.pData = reinterpret_cast<UINT8*>(triangleVertices);
    vertexData.RowPitch = vertexBufferSize;
    vertexData.SlicePitch = vertexData.RowPitch;

    UpdateSubresources<1>(m_commandList.get(), m_vertexBuffer.get(),
                          vertexBufferUpload.get(), 0, 0, 1, &vertexData);
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_vertexBuffer.get(), D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    m_commandList->ResourceBarrier(1, &barrier);

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
  }

  // Close the command list and execute it to begin the vertex buffer copy into
  // the default heap.
  winrt::check_hresult(m_commandList->Close());
  ID3D12CommandList* ppCommandLists[] = {m_commandList.get()};
  m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

  // Create synchronization objects and wait until assets have been uploaded to
  // the GPU.
  {
    winrt::check_hresult(m_d3d12Device->CreateFence(
        m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE,
        __uuidof(ID3D12Fence), m_fence.put_void()));
    m_fenceValues[m_frameIndex]++;

    // Create an event handle to use for frame synchronization.
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr) {
      winrt::check_hresult(HRESULT_FROM_WIN32(GetLastError()));
    }

    // Wait for the command list to execute; we are reusing the same command
    // list in our main loop but for now, we just want to wait for setup to
    // complete before continuing.
    WaitForGpu();
  }
}

// Update frame-based values.
void D3D1211on12::OnUpdate() {
}

// Render the scene.
void D3D1211on12::OnRender() {
  PIXBeginEvent(m_commandQueue.get(), 0, L"Render 3D");

  // Record all the commands we need to render the scene into the command list.
  PopulateCommandList();

  // Execute the command list.
  ID3D12CommandList* ppCommandLists[] = {m_commandList.get()};
  m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

  PIXEndEvent(m_commandQueue.get());

  PIXBeginEvent(m_commandQueue.get(), 0, L"Render UI");
  RenderUI();
  PIXEndEvent(m_commandQueue.get());

  // Present the frame.
  winrt::check_hresult(m_swapChain->Present(1, 0));

  MoveToNextFrame();
}

void D3D1211on12::OnDestroy() {
  // Ensure that the GPU is no longer referencing resources that are about to be
  // cleaned up by the destructor.
  WaitForGpu();

  CloseHandle(m_fenceEvent);
}

void D3D1211on12::PopulateCommandList() {
  // Command list allocators can only be reset when the associated
  // command lists have finished execution on the GPU; apps should use
  // fences to determine GPU execution progress.
  winrt::check_hresult(m_commandAllocators[m_frameIndex]->Reset());

  // However, when ExecuteCommandList() is called on a particular command
  // list, that command list can then be reset at any time and must be before
  // re-recording.
  winrt::check_hresult(m_commandList->Reset(
      m_commandAllocators[m_frameIndex].get(), m_pipelineState.get()));

  // Set necessary state.
  m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
  m_commandList->RSSetViewports(1, &m_viewport);
  m_commandList->RSSetScissorRects(1, &m_scissorRect);

  // Indicate that the back buffer will be used as a render target.
  auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      m_renderTargets[m_frameIndex].get(), D3D12_RESOURCE_STATE_PRESENT,
      D3D12_RESOURCE_STATE_RENDER_TARGET);
  m_commandList->ResourceBarrier(1, &barrier);

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
      m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
      m_rtvDescriptorSize);
  m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

  // Record commands.
  const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
  m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
  m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
  m_commandList->DrawInstanced(3, 1, 0, 0);

  // Note: do not transition the render target to present here.
  // the transition will occur when the wrapped 11On12 render
  // target resource is released.

  winrt::check_hresult(m_commandList->Close());
}

// Render text over D3D12 using D2D via the 11On12 device.
void D3D1211on12::RenderUI() {
  D2D1_SIZE_F rtSize = m_d2dRenderTargets[m_frameIndex]->GetSize();
  D2D1_RECT_F textRect = D2D1::RectF(0, 0, rtSize.width, rtSize.height);
  static const WCHAR text[] = L"11On12";

  ID3D11Resource* res = m_wrappedBackBuffers[m_frameIndex].get();
  // Acquire our wrapped render target resource for the current back buffer.
  m_d3d11On12Device->AcquireWrappedResources(&res, 1);

  // Render text directly to the back buffer.
  m_d2dDeviceContext->SetTarget(m_d2dRenderTargets[m_frameIndex].get());
  m_d2dDeviceContext->BeginDraw();
  m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
  m_d2dDeviceContext->DrawText(text, _countof(text) - 1, m_textFormat.get(),
                               &textRect, m_textBrush.get());
  winrt::check_hresult(m_d2dDeviceContext->EndDraw());

  // Release our wrapped render target resource. Releasing
  // transitions the back buffer resource to the state specified
  // as the OutState when the wrapped resource was created.
  m_d3d11On12Device->ReleaseWrappedResources(&res, 1);

  // Flush to submit the 11 command list to the shared command queue.
  m_d3d11DeviceContext->Flush();
}

// Wait for pending GPU work to complete.
void D3D1211on12::WaitForGpu() {
  // Schedule a Signal command in the queue.
  winrt::check_hresult(
      m_commandQueue->Signal(m_fence.get(), m_fenceValues[m_frameIndex]));

  // Wait until the fence has been processed.
  winrt::check_hresult(
      m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
  WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

  // Increment the fence value for the current frame.
  m_fenceValues[m_frameIndex]++;
}

// Prepare to render the next frame.
void D3D1211on12::MoveToNextFrame() {
  // Schedule a Signal command in the queue.
  const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
  winrt::check_hresult(
      m_commandQueue->Signal(m_fence.get(), currentFenceValue));

  // Update the frame index.
  m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

  // If the next frame is not ready to be rendered yet, wait until it is ready.
  if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex]) {
    winrt::check_hresult(m_fence->SetEventOnCompletion(
        m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
  }

  // Set the fence value for the next frame.
  m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

// Helper function for acquiring the first available hardware adapter that
// supports Direct3D 12. If no such adapter can be found, *ppAdapter will be set
// to nullptr.
_Use_decl_annotations_ void
D3D1211on12::GetHardwareAdapter(IDXGIFactory1* pFactory,
                                IDXGIAdapter1** ppAdapter,
                                bool requestHighPerformanceAdapter) {
  *ppAdapter = nullptr;

  winrt::com_ptr<IDXGIAdapter1> adapter;

  winrt::com_ptr<IDXGIFactory6> factory6;
  if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6)))) {
    for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
             adapterIndex,
             requestHighPerformanceAdapter == true
                 ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                 : DXGI_GPU_PREFERENCE_UNSPECIFIED,
             IID_PPV_ARGS(&adapter)));
         ++adapterIndex) {
      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // Don't select the Basic Render Driver adapter.
        // If you want a software adapter, pass in "/warp" on the command line.
        continue;
      }

      // Check to see whether the adapter supports Direct3D 12, but don't create
      // the actual device yet.
      if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0,
                                      _uuidof(ID3D12Device), nullptr))) {
        break;
      }
    }
  }

  if (adapter.get() == nullptr) {
    for (UINT adapterIndex = 0;
         SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, adapter.put()));
         ++adapterIndex) {
      DXGI_ADAPTER_DESC1 desc;
      adapter->GetDesc1(&desc);

      if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
        // Don't select the Basic Render Driver adapter.
        // If you want a software adapter, pass in "/warp" on the command line.
        continue;
      }

      // Check to see whether the adapter supports Direct3D 12, but don't create
      // the actual device yet.
      if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0,
                                      _uuidof(ID3D12Device), nullptr))) {
        break;
      }
    }
  }

  *ppAdapter = adapter.get();
  adapter->AddRef();
}