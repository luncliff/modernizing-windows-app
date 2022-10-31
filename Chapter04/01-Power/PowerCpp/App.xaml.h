#pragma once

#include "App.xaml.g.h"

namespace winrt::PowerCpp::implementation {

using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
using Microsoft::UI::Xaml::Window;

struct App : AppT<App> {
  App();

  void OnLaunched(LaunchActivatedEventArgs const &);

private:
  Window window{nullptr};
};

} // namespace winrt::PowerCpp::implementation
