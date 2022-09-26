#include "pch.h"

#include <winrt/Windows.UI.Core.h>

#include "MediaSinkPage.xaml.h"
#if __has_include("MediaSinkPage.g.cpp")
#include "MediaSinkPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using winrt::Windows::UI::Core::CoreWindow;

namespace winrt::App1::implementation {

MediaSinkPage::MediaSinkPage() {
    InitializeComponent();
    D3D_FEATURE_LEVEL device_feature_level = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL levels[]{D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    if (auto hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,
            levels, 2, D3D11_SDK_VERSION, device.put(), &device_feature_level, device_context.put());
        FAILED(hr)) {
        spdlog::error("{}: {}", "MediaSinkPage", winrt::to_string(winrt::hresult_error{hr}.message()));
        winrt::throw_hresult(hr);
    }
    winrt::com_ptr<IDXGIDevice2> dxgi_device{};
    winrt::check_hresult(device->QueryInterface(dxgi_device.put()));
    winrt::com_ptr<IDXGIAdapter> adapter{};
    winrt::check_hresult(dxgi_device->GetParent(__uuidof(IDXGIAdapter), adapter.put_void()));
    winrt::check_hresult(adapter->GetParent(__uuidof(IDXGIFactory2), factory.put_void()));

    if (device_feature_level >= D3D_FEATURE_LEVEL_11_0) {
        IUnknown* unknown = winrt::get_unknown(swapchainPanel());
        // unknown.as(__uuidof(ISwapChainPanelNative), bridge.put_void());
        if (auto hr = unknown->QueryInterface(bridge.put()); FAILED(hr))
            winrt::throw_hresult(hr);
    }
}

void MediaSinkPage::use_window(winrt::Windows::UI::Core::CoreWindow window) {
    DXGI_SWAP_CHAIN_DESC1 desc{};
    winrt::check_hresult(factory->CreateSwapChainForCoreWindow( //
        device.get(), winrt::get_unknown(window), &desc, nullptr, swapchain.put()));
    if (swapchain)
        bridge->SetSwapChain(swapchain.get());
}

void MediaSinkPage::use_window(HWND handle) {
    DXGI_SWAP_CHAIN_DESC1 desc{};
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen{};
    winrt::check_hresult(factory->CreateSwapChainForHwnd( //
        device.get(), handle, &desc, &fullscreen, nullptr, swapchain.put()));
    if (swapchain)
        bridge->SetSwapChain(swapchain.get());
}

} // namespace winrt::App1::implementation
