#include "pch.h"

#include "TestPage2.xaml.h"
#if __has_include("TestPage2.g.cpp")
#include "TestPage2.g.cpp"
#endif

namespace winrt::Exp1::implementation {
using namespace Microsoft::UI::Xaml;

TestPage2::TestPage2() {
  InitializeComponent();
  viewmodel0 = winrt::make<implementation::RepositoryViewModel>();
}

Exp1::RepositoryViewModel TestPage2::ViewModel0() {
  return viewmodel0;
}

fire_and_forget TestPage2::on_button_clicked(IInspectable const& s, RoutedEventArgs const&) {
  if (auto button = s.try_as<Button>(); button != OpenButton())
    spdlog::debug("{}: {}", "TestPage2", "sender is OpenButton");
  if (selected == nullptr)
    co_return;
  auto launched = co_await Windows::System::Launcher::LaunchUriAsync(selected.ProjectUri());
  if (launched == false)
    spdlog::error("{}: {}", "TestPage2", "launch failed");
}

void TestPage2::on_selection_changed(IInspectable const& s, SelectionChangedEventArgs const&) {
  auto view = s.try_as<Microsoft::UI::Xaml::Controls::ListView>();
  if (view == nullptr)
    return;
  auto item = view.SelectedItem();
  selected = item.try_as<RepositoryItem>();
  spdlog::info("{}: selected {}", "TestPage2", winrt::to_string(selected.Name()));
}

} // namespace winrt::Exp1::implementation
