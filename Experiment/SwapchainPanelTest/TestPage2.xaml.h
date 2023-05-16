﻿#pragma once
#include "TestPage2.g.h"

namespace winrt::SwapchainPanelTest::implementation {
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct TestPage2 : TestPage2T<TestPage2> {
  TestPage2() = default;

  int32_t MyProperty();
  void MyProperty(int32_t value);
};
} // namespace winrt::SwapchainPanelTest::implementation

namespace winrt::SwapchainPanelTest::factory_implementation {
struct TestPage2 : TestPage2T<TestPage2, implementation::TestPage2> {};
} // namespace winrt::SwapchainPanelTest::factory_implementation
