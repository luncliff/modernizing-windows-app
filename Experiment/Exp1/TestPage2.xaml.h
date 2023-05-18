#pragma once
#include "TestPage2.g.h"

#include "RepositoryViewModel.h"

namespace winrt::Exp1::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::Controls::Button;
using Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

struct TestPage2 : TestPage2T<TestPage2> {
  TestPage2();

  Exp1::RepositoryViewModel ViewModel0();

  void on_button_clicked(IInspectable const&, RoutedEventArgs const&);
  IAsyncAction on_selection_changed(IInspectable const&,
                                    SelectionChangedEventArgs const&);

private:
  Exp1::RepositoryViewModel viewmodel0 = nullptr;
};
} // namespace winrt::Exp1::implementation

namespace winrt::Exp1::factory_implementation {
struct TestPage2 : TestPage2T<TestPage2, implementation::TestPage2> {};
} // namespace winrt::Exp1::factory_implementation
