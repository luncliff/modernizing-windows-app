#include "pch.h"

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace winrt::SwapchainPanelTest {

auto make_logger(const char* name, FILE* fout) noexcept(false) {
  using namespace spdlog::sinks;
  std::vector<spdlog::sink_ptr> sinks{};
#if defined(_DEBUG)
  sinks.emplace_back(std::make_shared<msvc_sink_st>());
#endif
  if (fout == stdout)
    sinks.emplace_back(std::make_shared<wincolor_stdout_sink_st>());
  else if (fout == stderr)
    sinks.emplace_back(std::make_shared<wincolor_stderr_sink_st>());
  else {
    using mutex_t = spdlog::details::console_nullmutex;
    using sink_t = spdlog::sinks::stdout_sink_base<mutex_t>;
    sinks.emplace_back(std::make_shared<sink_t>(fout));
  }
  return std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
}

void set_log_stream(const char* name) {
  std::shared_ptr logger = make_logger(name, stdout);
  logger->set_pattern("%T.%e [%L] %8t %v");
#if defined(_DEBUG)
  logger->set_level(spdlog::level::level_enum::debug);
#endif
  spdlog::set_default_logger(logger);
}

std::string to_hex_string(HRESULT hr) noexcept {
  return fmt::format("{:#08x}", static_cast<uint32_t>(hr));
}

std::string w2mb(std::wstring_view in) noexcept(false) {
  std::string out{};
  out.reserve(MB_CUR_MAX * in.length());
  mbstate_t state{};
  for (wchar_t wc : in) {
    size_t len = 0;
    char mb[8]{}; // ensure null-terminated for UTF-8 (maximum 4 byte)
    if (auto ec = wcrtomb_s(&len, mb, wc, &state))
      throw std::system_error{ec, std::system_category(), "wcrtomb_s"};
    out += std::string_view{mb, len};
  }
  return out;
}

std::wstring mb2w(std::string_view in) noexcept(false) {
  std::wstring out{};
  out.reserve(in.length());
  const char* ptr = in.data();
  const char* const end = in.data() + in.length();
  mbstate_t state{};
  wchar_t wc{};
  while (size_t len = mbrtowc(&wc, ptr, end - ptr, &state)) {
    if (len == static_cast<size_t>(-1)) // bad encoding
      throw std::system_error{errno, std::system_category(), "mbrtowc"};
    if (len == static_cast<size_t>(-2)) // valid but incomplete
      break;                            // nothing to do more
    out.push_back(wc);
    ptr += len; // advance [1...n]
  }
  return out;
}

std::string to_guid_string(const GUID& guid) noexcept {
  constexpr auto bufsz = 40;
  wchar_t buf[bufsz]{};
  size_t buflen = StringFromGUID2(guid, buf, bufsz);
  return w2mb({buf + 1, buflen - 3}); // GUID requires 36 characters
}

} // namespace winrt::SwapchainPanelTest
