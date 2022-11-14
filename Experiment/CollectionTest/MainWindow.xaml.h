#pragma once
#include "MainWindow.g.h"

#include "RepositoryViewModel.h"

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::Controls::Button;
using Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

/// @see
/// https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.primitives.selector.selectionchanged
struct MainWindow : MainWindowT<MainWindow> {
private:
  CollectionTest::RepositoryViewModel viewModel0;

public:
  MainWindow();

  CollectionTest::RepositoryViewModel FirstViewModel();

  void on_button_clicked(IInspectable const&, RoutedEventArgs const&);
  IAsyncAction on_selection_changed(IInspectable const&,
                            SelectionChangedEventArgs const&);
};
} // namespace winrt::CollectionTest::implementation

namespace winrt::CollectionTest::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::CollectionTest::factory_implementation
