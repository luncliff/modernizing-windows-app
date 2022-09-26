#pragma once

#include "MediaSinkPage.g.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <evr.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>

#include <microsoft.ui.xaml.media.dxinterop.h> // NOT <windows.ui.xaml.media.dxinterop.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.h>
#include <windows.graphics.directx.direct3d11.interop.h>

namespace winrt::App1::implementation {

/// @see https://learn.microsoft.com/en-us/windows/uwp/composition/visual-layer
/// @see https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.swapchainpanel
/// @see https://github.com/KlearTouch/KlearTouch.MediaPlayer
struct MediaSinkPage : MediaSinkPageT<MediaSinkPage> {
    MediaSinkPage();

    /// @todo export with IDL
    void use_window(winrt::Windows::UI::Core::CoreWindow);
    void use_window(HWND handle);

  private:
    winrt::com_ptr<ID3D11Device> device = nullptr;
    winrt::com_ptr<ID3D11DeviceContext> device_context = nullptr;
    winrt::com_ptr<IDXGIFactory2> factory = nullptr;
    winrt::com_ptr<ISwapChainPanelNative> bridge = nullptr;
    winrt::com_ptr<IDXGISwapChain1> swapchain = nullptr;
};

} // namespace winrt::App1::implementation

namespace winrt::App1::factory_implementation {
struct MediaSinkPage : MediaSinkPageT<MediaSinkPage, implementation::MediaSinkPage> {};
} // namespace winrt::App1::factory_implementation
