#include "pch.h"

#include "TestPage2.xaml.h"
#if __has_include("TestPage2.g.cpp")
#include "TestPage2.g.cpp"
#endif

namespace winrt::Exp1::implementation {
using namespace Microsoft::UI::Xaml;

int32_t TestPage2::MyProperty() {
  throw hresult_not_implemented();
}

void TestPage2::MyProperty(int32_t /* value */) {
  throw hresult_not_implemented();
}

} // namespace winrt::Exp1::implementation
