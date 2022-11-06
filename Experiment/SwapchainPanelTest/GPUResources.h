#pragma once
#include <winrt/Windows.Foundation.h>

#include <d3d11_4.h>
#include <d3d11on12.h>
#include <dxgi1_6.h>

class GPUResources {
public:
  winrt::com_ptr<ID3D11Device> device{};
  winrt::com_ptr<ID3D11DeviceContext> context{};
  winrt::com_ptr<ID3D10Multithread> threading{};
  winrt::com_ptr<IDXGIAdapter> dxgi_adapter{};

public:
  GPUResources() noexcept(false);
};
