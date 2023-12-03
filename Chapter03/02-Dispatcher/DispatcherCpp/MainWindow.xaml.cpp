#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Microsoft.UI.Dispatching.h>

namespace winrt::DispatcherCpp::implementation
{
using Microsoft::UI::Dispatching::DispatcherQueue;

auto resume_on_queue(DispatcherQueue queue)
{
    struct awaiter_t
    {
        DispatcherQueue queue;

        constexpr bool await_ready() noexcept
        {
            return false;
        }
        bool await_suspend(std::coroutine_handle<void> coro)
        {
            return queue.TryEnqueue(coro);
        }
        constexpr void await_resume()
        {
        }
    };
    return awaiter_t{queue};
}

IAsyncAction MainWindow::OnUpdateMessage(IInspectable const &, RoutedEventArgs const &)
{
    auto queue = DispatcherQueue::GetForCurrentThread();
    co_await resume_on_queue(queue);
    txtMessage().Text(L"This message has been updated from a background thread");
}

IAsyncAction MainWindow::OnUpdateMessageWithToolkit(IInspectable const &, RoutedEventArgs const &)
{
    auto queue = DispatcherQueue::GetForCurrentThread();
    co_await resume_on_queue(queue);
    txtMessage().Text(L"This message has been updated from a background thread");
}

} // namespace winrt::DispatcherCpp::implementation
