#pragma once
#include "App.xaml.g.h"

//#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::App1::implementation {

using Microsoft::UI::Xaml::Window;
using Microsoft::UI::Xaml::Controls::Page;

using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
using Microsoft::UI::Xaml::UnhandledExceptionEventArgs;
using Windows::System::DispatcherQueue;
using Windows::System::DispatcherQueueController;

/**
 * @see http://aka.ms/winui-project-info
 */
struct App : AppT<App> {
    App();

    void OnLaunched(LaunchActivatedEventArgs const&);
    void on_unhandled_exception(IInspectable const&, UnhandledExceptionEventArgs const& e);

  private:
    DispatcherQueueController controller = nullptr;
    DispatcherQueue queue = nullptr;
    Window window1 = nullptr;
    Page page1 = nullptr;
};

} // namespace winrt::App1::implementation
