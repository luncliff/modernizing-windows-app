#include "pch.h"

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

bool has_env(const char* key) noexcept {
    size_t len = 0;
    char buf[40]{};
    if (auto ec = getenv_s(&len, buf, key); ec != 0)
        return false;
    std::string_view value{buf, len};
    return value.empty() == false;
}

auto make_logger(const char* name, FILE* fout) noexcept(false) {
    using mutex_t = spdlog::details::console_nullmutex;
    using sink_t = spdlog::sinks::stdout_sink_base<mutex_t>;
    return std::make_unique<spdlog::logger>(name, std::make_shared<sink_t>(fout));
}

void set_log_stream(const char* name) {
    std::shared_ptr logger = make_logger("test", stdout);
    logger->set_pattern("%T.%e [%L] %8t %v");
#if defined(_DEBUG)
    logger->set_level(spdlog::level::level_enum::debug);
#endif
    spdlog::set_default_logger(logger);
}
