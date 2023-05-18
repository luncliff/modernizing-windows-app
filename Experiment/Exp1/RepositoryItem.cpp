#include "pch.h"

#include "RepositoryItem.h"
#if __has_include("RepositoryItem.g.cpp")
#include "RepositoryItem.g.cpp"
#endif

namespace winrt::Exp1::implementation {

RepositoryItem::RepositoryItem() : name{}, owner{}, on_property_changed{} {
  // ...
}
RepositoryItem::RepositoryItem(std::wstring_view owner, std::wstring_view name)
    : RepositoryItem{} {
  this->owner = winrt::hstring{owner};
  this->name = winrt::hstring{name};
}

winrt::event_token
RepositoryItem::PropertyChanged(PropertyChangedEventHandler const& value) {
  return on_property_changed.add(value);
}

void RepositoryItem::PropertyChanged(winrt::event_token const& token) {
  on_property_changed.remove(token);
}

winrt::hstring RepositoryItem::Owner() {
  return owner;
}

void RepositoryItem::Owner(winrt::hstring const& value) {
  if (owner == value)
    return;
  owner = value;
  on_property_changed(*this, PropertyChangedEventArgs{L"Owner"});
}

winrt::hstring RepositoryItem::Name() {
  return name;
}

void RepositoryItem::Name(winrt::hstring const& value) {
  if (name == value)
    return;
  name = value;
  on_property_changed(*this, PropertyChangedEventArgs{L"Name"});
}

winrt::hstring RepositoryItem::ProjectUriString() {
  std::wstring value =
      fmt::format(L"https://github.com/{}/{}", owner.c_str(), name.c_str());
  return winrt::hstring{std::move(value)};
}

Uri RepositoryItem::ProjectUri() {
  return Uri{ProjectUriString()};
}

} // namespace winrt::Exp1::implementation