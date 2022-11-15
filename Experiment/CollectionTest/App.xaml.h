#pragma once

#include "App.xaml.g.h"

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
using Microsoft::UI::Xaml::Window;

struct App : AppT<App> {
  App();

  void OnLaunched(LaunchActivatedEventArgs const&);

private:
  Window window = nullptr;
};
} // namespace winrt::CollectionTest::implementation