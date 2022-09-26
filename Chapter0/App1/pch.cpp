#include "pch.h"

#include <winrt/Windows.Data.Xml.Dom.h>

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
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
    std::vector<spdlog::sink_ptr> sinks{};
#if defined(_DEBUG)
    sinks.emplace_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
#endif
    if (fout == stdout)
        sinks.emplace_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>());
    else if (fout == stderr)
        sinks.emplace_back(std::make_shared<spdlog::sinks::wincolor_stderr_sink_st>());
    else {
        using mutex_t = spdlog::details::console_nullmutex;
        using sink_t = spdlog::sinks::stdout_sink_base<mutex_t>;
        sinks.emplace_back(std::make_shared<sink_t>(fout));
    }
    return std::make_unique<spdlog::logger>(name, sinks.begin(), sinks.end());
}

void set_log_stream(const char* name) {
    std::shared_ptr logger = make_logger("test", stdout);
    logger->set_pattern("%T.%e [%L] %8t %v");
#if defined(_DEBUG)
    logger->set_level(spdlog::level::level_enum::debug);
#endif
    spdlog::set_default_logger(logger);
}

/// @see https://learn.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/send-local-toast-cpp-uwp
void send_notification(winrt::hstring title, winrt::hstring message) {
    using winrt::Windows::Data::Xml::Dom::XmlDocument;
    using winrt::Windows::UI::Notifications::ToastNotification;
    using winrt::Windows::UI::Notifications::ToastNotificationManager;
    using winrt::Windows::UI::Notifications::ToastNotifier;
    spdlog::info("{}: {}", winrt::to_string(title), winrt::to_string(message));

    XmlDocument doc{};
    doc.LoadXml(L"\
    <toast>\
        <visual>\
            <binding template=\"ToastGeneric\">\
                <text></text>\
                <text></text>\
            </binding>\
        </visual>\
    </toast>");

    // Populate with text and values
    //doc.DocumentElement().SetAttribute(L"launch", L"action=viewConversation&conversationId=9813");
    doc.SelectSingleNode(L"//text[1]").InnerText(title);
    doc.SelectSingleNode(L"//text[2]").InnerText(message);

    // Construct the notification
    // And show it!
    ToastNotification notification{doc};
    ToastNotificationManager manager{};
    ToastNotifier notifier = manager.CreateToastNotifier();
    notifier.Show(notification);
}

void print_version_info() {
    spdlog::info("WinRT:");
    spdlog::info("  version: {:s}", CPPWINRT_VERSION); // WINRT_version
    spdlog::info("Media Foundation:");
    spdlog::info("  SDK: {:X}", MF_SDK_VERSION);
    spdlog::info("  API: {:X}", MF_API_VERSION);
}
