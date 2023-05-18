#pragma once
#include "RepositoryViewModel.g.h"

#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::Exp1::implementation {
using Microsoft::UI::Xaml::Data::PropertyChangedEventArgs;
using Microsoft::UI::Xaml::Data::PropertyChangedEventHandler;
using Windows::Foundation::Collections::IObservableVector;
using Windows::Foundation::Collections::IVector;

struct RepositoryViewModel : RepositoryViewModelT<RepositoryViewModel> {
private:
  IObservableVector<Exp1::RepositoryItem> items;

public:
  RepositoryViewModel();

  IObservableVector<Exp1::RepositoryItem> Repositories();
};
} // namespace winrt::Exp1::implementation

namespace winrt::Exp1::factory_implementation {
struct RepositoryViewModel
    : RepositoryViewModelT<RepositoryViewModel,
                           implementation::RepositoryViewModel> {};
} // namespace winrt::Exp1::factory_implementation
