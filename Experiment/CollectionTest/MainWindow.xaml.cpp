#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "MyItem1.h"

namespace winrt::CollectionTest::implementation {
using namespace Microsoft::UI::Xaml;

/// @see
/// https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/collections
MainWindow::MainWindow() {
  items = winrt::single_threaded_observable_vector<CollectionTest::MyItem1>();
  for (auto i = 0; i < 3; ++i) {
    winrt::hstring name{fmt::format(L"item-{}", i)};
    // implementation::MyItem1 item{winrt::hstring(name)};
    items.Append(winrt::make<implementation::MyItem1>(name));
  }
  InitializeComponent();
}

IObservableVector<CollectionTest::MyItem1> MainWindow::Item1Collection() {
  spdlog::info("{}: {}", __func__, items.Size());
  return items;
}

void MainWindow::on_button_clicked(IInspectable const& s,
                                   RoutedEventArgs const&) {
  auto button = s.as<Button>();
  button.Content(box_value(L"Clicked"));

  winrt::hstring name{fmt::format(L"item-{}", items.Size())};
  // implementation::MyItem1 item{winrt::hstring(name)};
  items.Append(winrt::make<implementation::MyItem1>(name));
}

} // namespace winrt::CollectionTest::implementation
