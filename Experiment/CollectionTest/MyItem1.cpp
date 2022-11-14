#include "pch.h"

#include "MyItem1.h"
#if __has_include("MyItem1.g.cpp")
#include "MyItem1.g.cpp"
#endif
#include "MyItem1.g.h"

namespace winrt::CollectionTest::implementation {

MyItem1::MyItem1(hstring const& name) : m_name{name}, m_propertyChanged{} {
}

hstring MyItem1::Name() {
  return m_name;
}

void MyItem1::Name(hstring const& value) {
  if (m_name == value)
    return;
  m_name = value;
  m_propertyChanged(*this, PropertyChangedEventArgs{L"Name"});
}

winrt::event_token
MyItem1::PropertyChanged(PropertyChangedEventHandler const& value) {
  return m_propertyChanged.add(value);
}

void MyItem1::PropertyChanged(winrt::event_token const& token) {
  m_propertyChanged.remove(token);
}

} // namespace winrt::CollectionTest::implementation
