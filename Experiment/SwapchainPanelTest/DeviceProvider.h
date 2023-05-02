#pragma once
#include <d3d11on12.h>
#include <d3d12.h>
#include <winrt/Microsoft.Graphics.DirectX.h>
#include <winrt/Windows.Foundation.h>

#include "DXGIProvider.h"

namespace winrt::SwapchainPanelTest {

using Windows::Foundation::IAsyncAction;

class DeviceProvider final {
  winrt::com_ptr<ID3D12Debug> debug_control = nullptr;
  winrt::com_ptr<ID3D12InfoQueue> d11_on_d12_info = nullptr;
  winrt::com_ptr<ID3D11On12Device> d11_on_d12_device = nullptr;
  winrt::com_ptr<ID3D12Device> d12_device = nullptr;
  winrt::com_ptr<ID3D12CommandQueue> d12_command_queue = nullptr;
  winrt::com_ptr<ID3D11Device> d11_device = nullptr;
  winrt::com_ptr<ID3D11DeviceContext> d11_device_context = nullptr;

public:
  explicit DeviceProvider(DXGIProvider& dxgi) noexcept(false);

  bool supports_dx12() const noexcept;
  winrt::com_ptr<ID3D12Device> get_dx12_device() noexcept;
  winrt::com_ptr<ID3D12CommandQueue> get_dx12_command_queue() noexcept;
  winrt::com_ptr<ID3D11Device> get_dx11_device() noexcept;
  winrt::com_ptr<ID3D11DeviceContext> get_dx11_device_context() noexcept;
};

} // namespace winrt::SwapchainPanelTest
