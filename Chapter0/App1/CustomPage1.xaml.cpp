#include "pch.h"

#include "CustomPage1.xaml.h"
#if __has_include("CustomPage1.g.cpp")
#include "CustomPage1.g.cpp"
#endif

namespace winrt::App1::implementation {

CustomPage1::CustomPage1() {
    InitializeComponent();

    auto panel = SwapchainPanel1();
    winrt::com_ptr<IUnknown> unknown = nullptr;
    if (auto hr = panel.as(__uuidof(IUnknown), unknown.put_void()); FAILED(hr))
        winrt::throw_hresult(hr);
    if (auto hr = unknown->QueryInterface(native.put()); FAILED(hr))
        winrt::throw_hresult(hr);
}

void CustomPage1::on_window_size_changed(IInspectable const&, WindowSizeChangedEventArgs const&) {
}


} // namespace winrt::App1::implementation
