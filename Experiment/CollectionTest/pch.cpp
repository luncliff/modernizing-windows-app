#include "pch.h"

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace winrt::CollectionTest {

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

} // namespace winrt::CollectionTest
