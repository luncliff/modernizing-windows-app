#pragma once

#include "MyItem1.g.h"

#include <winrt/Microsoft.UI.Xaml.Data.h>

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::Data::PropertyChangedEventArgs;
using Microsoft::UI::Xaml::Data::PropertyChangedEventHandler;

struct MyItem1 : MyItem1T<MyItem1> {
  MyItem1() = delete;
  MyItem1(winrt::hstring const& name);

  winrt::hstring Name();
  void Name(winrt::hstring const& value);

  winrt::event_token PropertyChanged(PropertyChangedEventHandler const& value);
  void PropertyChanged(winrt::event_token const& token);

private:
  winrt::hstring m_name;
  winrt::event<PropertyChangedEventHandler> m_propertyChanged;
};
} // namespace winrt::CollectionTest::implementation

namespace winrt::CollectionTest::factory_implementation {
struct MyItem1 : MyItem1T<MyItem1, implementation::MyItem1> {};
} // namespace winrt::CollectionTest::factory_implementation
