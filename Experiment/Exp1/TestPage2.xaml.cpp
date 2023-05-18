#include "pch.h"

#include "TestPage2.xaml.h"
#if __has_include("TestPage2.g.cpp")
#include "TestPage2.g.cpp"
#endif

namespace winrt::Exp1::implementation {
using namespace Microsoft::UI::Xaml;

TestPage2::TestPage2() {
  viewmodel0 = winrt::make<implementation::RepositoryViewModel>();
}

Exp1::RepositoryViewModel TestPage2::ViewModel0() {
  return viewmodel0;
}

void TestPage2::on_button_clicked(IInspectable const& s,
                                  RoutedEventArgs const&) {
  if (auto button = s.try_as<Button>(); button != nullptr)
    spdlog::debug("{}: {}", "TestPage2", "sender is Button");
}

IAsyncAction TestPage2::on_selection_changed(IInspectable const& s,
                                             SelectionChangedEventArgs const&) {
  auto view = s.try_as<Microsoft::UI::Xaml::Controls::ListView>();
  if (view == nullptr)
    return;
  auto selected = view.SelectedItem();
  auto item = selected.try_as<RepositoryItem>();
  if (item == nullptr)
    return;

  spdlog::info("{}: selected {}", "TestPage2", winrt::to_string(item.Name()));
  auto launched =
      co_await Windows::System::Launcher::LaunchUriAsync(item.ProjectUri());
  if (launched == false)
    spdlog::error("{}: {}", "TestPage2", "launch failed");
}

} // namespace winrt::Exp1::implementation
