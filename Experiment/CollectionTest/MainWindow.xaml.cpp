#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Microsoft.Windows.System.h>

namespace winrt::CollectionTest::implementation {
using namespace Microsoft::UI::Xaml;
using Windows::System::Launcher;

/// @see
/// https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/collections
MainWindow::MainWindow() {
  spdlog::info("{}: {}", "MainWindow", __func__);
  viewModel0 = winrt::make<implementation::RepositoryViewModel>();
  InitializeComponent();
}

CollectionTest::RepositoryViewModel MainWindow::FirstViewModel() {
  return viewModel0;
}

void MainWindow::on_button_clicked(IInspectable const& s,
                                   RoutedEventArgs const&) {
  if (auto button = s.try_as<Button>(); button != nullptr)
    // button.Content(box_value(L"Clicked"));
    spdlog::debug("{}: {}", "MainWindow", "sender is Button");
}

IAsyncAction
MainWindow::on_selection_changed(IInspectable const& s,
                                 SelectionChangedEventArgs const&) {
  auto view = s.try_as<Microsoft::UI::Xaml::Controls::ListView>();
  if (view == nullptr)
    return;
  auto selected = view.SelectedItem();
  auto item = selected.try_as<RepositoryItem>();
  if (item == nullptr)
    return;

  spdlog::info("{}: selected {}", "MainWindow", winrt::to_string(item.Name()));
  auto launched =
      co_await Windows::System::Launcher::LaunchUriAsync(item.ProjectUri());
  if (launched == false)
    spdlog::error("{}: {}", "MainWindow", "launch failed");
}

} // namespace winrt::CollectionTest::implementation
