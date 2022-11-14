#pragma once
#include "RepositoryViewModel.g.h"

#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::CollectionTest::implementation {
using Microsoft::UI::Xaml::Data::PropertyChangedEventArgs;
using Microsoft::UI::Xaml::Data::PropertyChangedEventHandler;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

struct RepositoryViewModel : RepositoryViewModelT<RepositoryViewModel> {
private:
  IObservableVector<CollectionTest::RepositoryItem> items;

public:
  RepositoryViewModel();

  IObservableVector<CollectionTest::RepositoryItem> Repositories();
};
} // namespace winrt::CollectionTest::implementation

namespace winrt::CollectionTest::factory_implementation {
struct RepositoryViewModel
    : RepositoryViewModelT<RepositoryViewModel,
                           implementation::RepositoryViewModel> {};
} // namespace winrt::CollectionTest::factory_implementation
