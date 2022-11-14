#pragma once
#include "RepositoryItem.g.h"

#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::Data::PropertyChangedEventArgs;
using Microsoft::UI::Xaml::Data::PropertyChangedEventHandler;
using Windows::Foundation::Uri;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

struct RepositoryItem : RepositoryItemT<RepositoryItem> {
private:
  winrt::hstring name;
  winrt::hstring owner;
  winrt::event<PropertyChangedEventHandler> on_property_changed{};

public:
  RepositoryItem();
  RepositoryItem(std::wstring_view owner, std::wstring_view name);

  winrt::hstring Owner();
  void Owner(winrt::hstring const& value);

  winrt::hstring Name();
  void Name(winrt::hstring const& value);

    winrt::hstring ProjectUriString();
  Uri ProjectUri();

  winrt::event_token PropertyChanged(PropertyChangedEventHandler const& value);
  void PropertyChanged(winrt::event_token const& token);
};

} // namespace winrt::CollectionTest::implementation

namespace winrt::CollectionTest::factory_implementation {
struct RepositoryItem
    : RepositoryItemT<RepositoryItem, implementation::RepositoryItem> {};
} // namespace winrt::CollectionTest::factory_implementation
