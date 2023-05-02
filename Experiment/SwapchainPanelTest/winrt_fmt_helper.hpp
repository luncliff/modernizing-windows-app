#pragma once
#include <fmt/format.h>
#include <string_view>
#include <winrt/base.h>

namespace fmt {

template <>
struct fmt::formatter<winrt::hstring> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end)
      it++;
    if (it != end && *it != '}')
      throw format_error("bad format - winrt::hstring");
    return it;
  }

  template <typename FormatContext>
  auto format(const winrt::hstring& s, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), L"{:s}",
                          static_cast<std::wstring_view>(s));
  }
};

/**
 * @see https://fmt.dev/latest/api.html#format-api
 * @note support "{}" expression
 */
template <>
struct fmt::formatter<winrt::hresult_error> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end)
      it++;
    if (it != end && *it != '}')
      throw format_error("bad format - winrt::hresult_error");
    return it;
  }

  template <typename FormatContext>
  auto format(const winrt::hresult_error& ex, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), L"{:#08x} {:s}",
                          static_cast<uint32_t>(ex.code()),
                          static_cast<std::wstring_view>(ex.message()));
  }
};

} // namespace fmt
