
C++ version of Dispatcher.csproj.
The DispatcherCpp project showes how to port `async void` function with `IAsyncAction`, which supports C++ Coroutines.

```csharp
public sealed partial class MainWindow : Window
{
    // ...
    private async void OnUpdateMessage(object sender, RoutedEventArgs e)
    {
        var dispatcher = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread();
        await Task.Run(() =>
        {
            dispatcher.TryEnqueue(() =>
            {
                txtMessage.Text = "This message has been updated from a background thread";
            });
        });
    }
}
```

Defind asynchronous method in MIDL file.

```midl
// MainWindow.idl
namespace DispatcherCpp
{
    [default_interface]
    runtimeclass MainWindow : Microsoft.UI.Xaml.Window
    {
        MainWindow();
        Windows.Foundation.IAsyncAction OnUpdateMessage(IInspectable sender, Microsoft.UI.Xaml.RoutedEventArgs e);
    }
}
```

Then add the method with `winrt::Windows::Foundation`, `winrt::Microsoft::UI::Xaml` namespace.
For the implementation, see [MainWindow.xaml.cpp](./MainWindow.xaml.cpp)

```c++
namespace winrt::DispatcherCpp::implementation
{
    using Windows::Foundation::IAsyncAction;
    using Windows::Foundation::IInspectable;
    using Microsoft::UI::Xaml::RoutedEventArgs;

    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        // ...
        IAsyncAction OnUpdateMessage(IInspectable const& sender, RoutedEventArgs const& args);
    };
}
```
