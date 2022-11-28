#include "pch.h"

#include "DXGIProvider.h"
#include "DeviceProvider.h"

#pragma comment(lib, "D3D12")

namespace winrt::SwapchainPanelTest {

DeviceProvider::DeviceProvider(DXGIProvider& dxgi) noexcept(false) {
  UINT d11_flags =
      D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#if defined(_DEBUG)
  if (auto hr = D3D12GetDebugInterface(__uuidof(ID3D12Debug),
                                       debug_control.put_void());
      SUCCEEDED(hr)) {
    debug_control->EnableDebugLayer();
    d11_flags |= D3D11_CREATE_DEVICE_DEBUG;
    spdlog::info("{}: {}", "DeviceProvider", "enable ID3D12Debug");
  }
#endif
  // can we use DX12 device?
  if (FAILED(dxgi.create_device(d12_device.put(), true))) {
    // No. use DX11
    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL levels[]{D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    if (auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                    d11_flags, levels, 1, D3D11_SDK_VERSION,
                                    d11_device.put(), &level,
                                    d11_device_context.put());
        FAILED(hr))
      winrt::throw_hresult(hr);
  } else {
    // DX12 command queue
    {
      D3D12_COMMAND_QUEUE_DESC desc{};
      desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
      desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
      if (auto hr = d12_device->CreateCommandQueue(
              &desc, __uuidof(ID3D12CommandQueue),
              d12_command_queue.put_void());
          FAILED(hr))
        winrt::throw_hresult(hr);
      SetName(d12_command_queue.get(), L"d12_command_queue");
    }
    // DX11 device on the DX12 device, command queue
    {
      IUnknown* cq = static_cast<IUnknown*>(d12_command_queue.get());
      if (auto hr = D3D11On12CreateDevice(d12_device.get(), d11_flags, nullptr,
                                          0, &cq, 1, 0, d11_device.put(),
                                          d11_device_context.put(), nullptr);
          FAILED(hr))
        winrt::throw_hresult(hr);
      // query the 11on12 device
      if (auto hr = d11_device->QueryInterface(d11_on_d12_device.put());
          FAILED(hr))
        winrt::throw_hresult(hr);
    }
#if defined(_DEBUG)
    if (auto hr = d12_device->QueryInterface(d11_on_d12_info.put());
        SUCCEEDED(hr)) {
      // Suppress whole categories of messages.
      D3D12_MESSAGE_CATEGORY categories[]{
          D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION,
          D3D12_MESSAGE_CATEGORY_SHADER,
      };
      // Suppress messages based on their severity level.
      D3D12_MESSAGE_SEVERITY severities[]{
          D3D12_MESSAGE_SEVERITY_INFO,
      };
      // Suppress individual messages by their ID.
      D3D12_MESSAGE_ID denyIds[]{
          // This occurs when there are uninitialized descriptors
          // in a descriptor table, even when a shader does not access
          // the missing descriptors.
          D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
      };
      D3D12_INFO_QUEUE_FILTER filter{};
      filter.DenyList.NumCategories = _countof(categories);
      filter.DenyList.pCategoryList = categories;
      filter.DenyList.NumSeverities = _countof(severities);
      filter.DenyList.pSeverityList = severities;
      filter.DenyList.NumIDs = _countof(denyIds);
      filter.DenyList.pIDList = denyIds;
      winrt::check_hresult(d11_on_d12_info->PushStorageFilter(&filter));
    }
#endif
  }
}

bool DeviceProvider::supports_dx12() const noexcept {
  return d12_device != nullptr;
}

winrt::com_ptr<ID3D12Device> DeviceProvider::get_dx12_device() noexcept {
  return d12_device;
}

winrt::com_ptr<ID3D12CommandQueue>
DeviceProvider::get_dx12_command_queue() noexcept {
  return d12_command_queue;
}

winrt::com_ptr<ID3D11Device> DeviceProvider::get_dx11_device() noexcept {
  return d11_device;
}

winrt::com_ptr<ID3D11DeviceContext>
DeviceProvider::get_dx11_device_context() noexcept {
  return d11_device_context;
}

void DeviceProvider::reset_render_targets() noexcept {
  std::array<ID3D11RenderTargetView*, 1> targets0{};
  // no depth, stencil
  d11_device_context->OMSetRenderTargets(targets0.size(), targets0.data(),
                                         nullptr);
}

} // namespace winrt::SwapchainPanelTest
