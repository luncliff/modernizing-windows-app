#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.ApplicationModel.Activation.h>

namespace winrt::ActivationCpp::implementation
{
using namespace Microsoft::UI::Xaml;
using Microsoft::Windows::AppLifecycle::AppActivationArguments;
using Microsoft::Windows::AppLifecycle::AppInstance;
using Microsoft::Windows::AppLifecycle::ExtendedActivationKind;
using Windows::ApplicationModel::Activation::FileActivatedEventArgs;

App::App()
{
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([](IInspectable const &, UnhandledExceptionEventArgs const &e) {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

void App::OnLaunched([[maybe_unused]] LaunchActivatedEventArgs const &e)
{
    window = make<MainWindow>();
    auto app = AppInstance::GetCurrent();
    auto eventArgs = app.GetActivatedEventArgs();
    if (eventArgs.Kind() == ExtendedActivationKind::File)
    {
        auto fileActivationArguments = eventArgs.Data().as<FileActivatedEventArgs>();
        auto item = fileActivationArguments.Files().GetAt(0);
        auto impl = window.as<ActivationCpp::implementation::MainWindow>();
        impl->FilePath(item);
    }
    window.Activate();
}
} // namespace winrt::ActivationCpp::implementation
