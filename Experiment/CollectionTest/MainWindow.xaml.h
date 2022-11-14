#pragma once
#include "MainWindow.g.h"

#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::Controls::Button;
using Windows::Foundation::IInspectable;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

struct MainWindow : MainWindowT<MainWindow> {
private:
  IObservableVector<CollectionTest::MyItem1> items;

public:
  MainWindow();

  IObservableVector<CollectionTest::MyItem1> Item1Collection();

  void on_button_clicked(IInspectable const&, RoutedEventArgs const&);
};
} // namespace winrt::CollectionTest::implementation

namespace winrt::CollectionTest::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::CollectionTest::factory_implementation
