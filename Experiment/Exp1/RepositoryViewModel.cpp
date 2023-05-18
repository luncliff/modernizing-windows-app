#include "pch.h"

#include "RepositoryViewModel.h"
#if __has_include("RepositoryViewModel.g.cpp")
#include "RepositoryViewModel.g.cpp"
#endif
#include "RepositoryItem.h"

namespace winrt::Exp1::implementation {

RepositoryViewModel::RepositoryViewModel() {
  spdlog::info("{}: {}", "RepositoryViewModel", __func__); // ..
  items = winrt::single_threaded_observable_vector<Exp1::RepositoryItem>();
  items.Append(
      winrt::make<implementation::RepositoryItem>(L"microsoft", L"vcpkg"));
  items.Append(
      winrt::make<implementation::RepositoryItem>(L"conan-io", L"conan"));
}

IObservableVector<Exp1::RepositoryItem> RepositoryViewModel::Repositories() {
  return items;
}

} // namespace winrt::Exp1::implementation
