#pragma once
#include "App.xaml.g.h"

#include "DeviceProvider.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

namespace winrt::Exp1::implementation {
using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
using Microsoft::UI::Xaml::UnhandledExceptionEventArgs;
using Microsoft::UI::Xaml::Window;
using Microsoft::UI::Xaml::Controls::Page;
using Microsoft::Windows::ApplicationModel::Resources::ResourceContext;
using Microsoft::Windows::ApplicationModel::Resources::ResourceManager;
using Microsoft::Windows::ApplicationModel::Resources::ResourceMap;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct App : AppT<App> {
  App();

  void OnLaunched(LaunchActivatedEventArgs const&);
  void on_unhandled_exception(IInspectable const&, UnhandledExceptionEventArgs const& e);

private:
  ResourceManager resource_manager{};
  ResourceContext resource_context = nullptr;
  ResourceMap resource_map = nullptr;
  DXGIProvider dxgi{};
  DeviceProvider devices{dxgi};

  Window window = nullptr;
};

} // namespace winrt::Exp1::implementation
