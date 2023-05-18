#include "pch.h"

#include "DeviceProvider.h"

namespace winrt::Exp1 {

void DXGIProvider::GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** output,
                                      DXGI_GPU_PREFERENCE preference) noexcept(false) {
  *output = nullptr;
  winrt::com_ptr<IDXGIFactory6> factory6;
  if (auto hr = factory->QueryInterface(factory6.put()); FAILED(hr))
    winrt::throw_hresult(hr);
  // ...
  winrt::com_ptr<IDXGIAdapter1> adapter = nullptr;
  for (UINT index = 0;
       SUCCEEDED(factory6->EnumAdapterByGpuPreference(index, preference, __uuidof(IDXGIAdapter1), adapter.put_void()));
       ++index) {
    DXGI_ADAPTER_DESC1 desc{};
    if (auto hr = adapter->GetDesc1(&desc); FAILED(hr))
      FAILED(hr);
    // Don't select the Basic Render Driver adapter.
    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) 
      continue;
    // Support DirectX 11?
    if (SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
      break;
    adapter = nullptr;
  }
  if (adapter)
    adapter->AddRef();
  *output = adapter.get();
}

HRESULT DXGIProvider::create_device(ID3D12Device** device, bool hardware) noexcept(false) {
  try {
    if (hardware)
      GetHardwareAdapter(factory.get(), adapter.put());
    else if (auto hr = factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), adapter.put_void()); FAILED(hr)) {
      spdlog::warn("{}: {}", "EnumWarpAdapter", hr);
      winrt::throw_hresult(hr);
    }
    // create device with the adpater
    if (auto hr = D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device)); FAILED(hr))
      winrt::throw_hresult(hr);
  } catch (const winrt::hresult_error& ex) {
    spdlog::error("{}: {}", __func__, winrt::to_string(ex.message()));
    return ex.code();
  }
  return S_OK;
}

HRESULT
DXGIProvider::create_swapchain(const DXGI_SWAP_CHAIN_DESC1& desc, ID3D11Device* device,
                               IDXGISwapChain1** output) noexcept(false) {
  if (auto hr = factory->CreateSwapChainForComposition(device, &desc, nullptr, output); FAILED(hr))
    winrt::throw_hresult(hr);
  spdlog::info("{}: {}", "DXGIProvider", "created swapchain");
  return S_OK;
}

HRESULT
DXGIProvider::create_swapchain(const DXGI_SWAP_CHAIN_DESC1& desc, ID3D11Device* device,
                               IDXGISwapChain3** output) noexcept(false) {
  winrt::com_ptr<IDXGISwapChain1> swapchain1 = nullptr;
  if (auto hr = create_swapchain(desc, device, swapchain1.put()); FAILED(hr))
    return hr;
  return swapchain1->QueryInterface(output);
}

winrt::com_ptr<IDXGIFactory4> DXGIProvider::get_factory() noexcept {
  return factory;
}

DXGIProvider::DXGIProvider() noexcept(false) {
  UINT dxgi_flags = 0;
#if defined(_DEBUG)
  dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
  if (auto hr = CreateDXGIFactory2(dxgi_flags, __uuidof(IDXGIFactory4), factory.put_void()); FAILED(hr))
    winrt::throw_hresult(hr);
}

} // namespace winrt::Exp1
