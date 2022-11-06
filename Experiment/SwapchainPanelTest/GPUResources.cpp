#include "pch.h"

#include "GPUResources.h"

GPUResources::GPUResources() noexcept(false) {
  UINT flags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#if defined(_DEBUG)
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  D3D_FEATURE_LEVEL level{};
  D3D_FEATURE_LEVEL levels[]{D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
                             D3D_FEATURE_LEVEL_11_0};
  if (auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                  flags, levels, 3, D3D11_SDK_VERSION,
                                  device.put(), &level, context.put());
      FAILED(hr)) {
    spdlog::error("{}: {:#08x}", "D3D11CreateDevice",
                  static_cast<uint32_t>(hr));
    winrt::throw_hresult(hr);
  }
  if (auto hr = device->QueryInterface(threading.put()); SUCCEEDED(hr))
    threading->SetMultithreadProtected(true);
}
