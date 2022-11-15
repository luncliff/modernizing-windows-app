//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "D3D12Helper.h"
#include "DeviceResources.h"

namespace winrt::SwapchainPanelTest {

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the
// CPU, it has no understanding of the lifetime of resources on the GPU. Apps
// must account for the GPU lifetime of resources to avoid destroying objects
// that may still be referenced by the GPU. An example of this can be found in
// the class method: OnDestroy().

class D3D1211on12 {
public:
  D3D1211on12() = default;

  virtual void OnInit(HWND hwnd, UINT width, UINT height);
  virtual void OnUpdate();
  virtual void OnRender();
  virtual void OnDestroy();

  void LoadPipeline(HWND hwnd, UINT width, UINT height);
  void LoadAssets();
  void PopulateCommandList();
  void WaitForGpu();
  void MoveToNextFrame();
  void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
                          bool requestHighPerformanceAdapter);

private:
  static const UINT FrameCount = 3;

  struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
  };

  // Pipeline objects.
  CD3DX12_VIEWPORT m_viewport;
  CD3DX12_RECT m_scissorRect;
  winrt::com_ptr<IDXGISwapChain3> m_swapChain;
  winrt::com_ptr<ID3D11DeviceContext> m_d3d11DeviceContext;
  winrt::com_ptr<ID3D11On12Device> m_d3d11On12Device;
  winrt::com_ptr<ID3D12Device> m_d3d12Device;
  winrt::com_ptr<IDWriteFactory> m_dWriteFactory;
  winrt::com_ptr<ID3D12Resource> m_renderTargets[FrameCount];
  winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
  winrt::com_ptr<ID3D12CommandQueue> m_commandQueue;
  winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
  winrt::com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
  winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
  winrt::com_ptr<ID3D12GraphicsCommandList> m_commandList;

  // App resources.
  UINT m_rtvDescriptorSize;
  winrt::com_ptr<ID3D12Resource> m_vertexBuffer;
  D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

  // Synchronization objects.
  UINT m_frameIndex;
  HANDLE m_fenceEvent;
  winrt::com_ptr<ID3D12Fence> m_fence;
  UINT64 m_fenceValues[FrameCount];
};

} // namespace winrt::SwapchainPanelTest
