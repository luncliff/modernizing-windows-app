#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::DispatcherCpp::implementation {

	struct enqueue_awaitable_t {
		Microsoft::UI::Dispatching::DispatcherQueue queue;

		constexpr bool await_ready() { return false; }
		bool await_suspend(std::experimental::coroutine_handle<void> task) {
			return queue.TryEnqueue(task);
		}
		constexpr void await_resume() {}
	};

	MainWindow::MainWindow() {
		InitializeComponent();
		dispatcher =
			Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
	}

	IAsyncAction MainWindow::OnUpdateMessage(IInspectable const& sender,
		RoutedEventArgs const& args) {
		co_await winrt::resume_background();
		co_await enqueue_awaitable_t{ dispatcher };
		txtMessage().Text(L"OnUpdateMessage: updated from a background thread");
	}

} // namespace winrt::DispatcherCpp::implementation
