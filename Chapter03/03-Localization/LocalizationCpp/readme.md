
### AppResourceManager

```csharp
using Microsoft.Windows.ApplicationModel.Resources;
```

```c++
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
```

Check [Localization/AppResourceManager.cs](../Localization/AppResourceManager.cs) for C# implementation.

Here I will use [pch.h](./pch.h).

```c++
namespace winrt::LocalizationCpp
{
    using Microsoft::Windows::ApplicationModel::Resources::ResourceContext;
    using Microsoft::Windows::ApplicationModel::Resources::ResourceManager;
    using Microsoft::Windows::ApplicationModel::Resources::ResourceMap;

    class AppResourceManager final {
        ResourceManager manager{};
        ResourceContext context = nullptr;
        ResourceMap main = nullptr;

    private:
        AppResourceManager() noexcept(false);

    public:
        static AppResourceManager& Instance();

        std::wstring MakeKey(winrt::hstring name);
        winrt::hstring GetString(winrt::hstring name);
        winrt::com_array<uint8_t> GetBytes(winrt::hstring name);
    };

} // namespace winrt::LocalizationCpp
```

Here, we will use https://github.com/fmtlib/fmt for string generation.

```c++
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/xchar.h>

namespace winrt::LocalizationCpp
{
    std::wstring AppResourceManager::MakeKey(winrt::hstring name) {
        auto key = fmt::format(L"Resources/{}", static_cast<std::wstring_view>(name));
        for (auto& c : key)
            if (c == L'.') c = L'/';
        return key;
    }
}
```

With the key, we can implement `GetString`.

```c++
namespace winrt::LocalizationCpp
{
    AppResourceManager::AppResourceManager() noexcept(false) {
        context = manager.CreateResourceContext();
        main = manager.MainResourceMap();
    }

    AppResourceManager& AppResourceManager::Instance() {
        static AppResourceManager instance{};
        return instance;
    }

    winrt::hstring AppResourceManager::GetString(winrt::hstring name) {
        return main.GetValue(MakeKey(name)).ValueAsString();
    }
}
```

### ResourceString

```csharp
using Microsoft.UI.Xaml.Markup;
```

```c++
#include <winrt/Microsoft.UI.Xaml.Markup.h>
```

[MainWindow.xaml](./MainWindow.xaml) contains `"{local:ResourceString}"`.

```xaml
<StackPanel Margin="12">
    <!-- localization using x:Uid -->
    <Button x:Uid="MyButton" Click="OnGetTranslation" />
    <!-- localization using custom markup expression -->
    <Button Content="{local:ResourceString Name=MyButton.Content}" />
    <TextBlock x:Uid="MyTextBlock" />
</StackPanel>
```

Unlike C# project, in C++ project, we have to provide .idl file so XAML compiler can recognize it.

```csharp
namespace Localization
{
    [MarkupExtensionReturnType(ReturnType = typeof(string))]
    public sealed class ResourceString : MarkupExtension
    {
        public string Name { get; set; }
        protected override object ProvideValue()
        {
            return AppResourceManager.Instance.GetString(Name);
        }
    }
}
```

```midl
namespace Localization
{
    runtimeclass ResourceString : Microsoft.UI.Xaml.Markup.MarkupExtension
    {
        ResourceString();

        String Name;
        IInspectable ProvideValue();
    }
}
```

Be careful. [In the project, namespace is LocalizationCpp](./ResourceString.idl).

When we use MIDL-generated type, the `${type}.g.h` and `${type}.g.cpp` will be generated for it.
We need matching implementation source for them.
For example, 

* `ResourceString.g.h` --> `ResourceString.h`
* `ResourceString.g.cpp` --> `ResourceString.cpp`

```c++
// ResourceString.h
#pragma once
#include "ResourceString.g.h"

namespace winrt::LocalizationCpp::implementation
{
    // ...
}

namespace winrt::LocalizationCpp::factory_implementation
{
    // ...
}
```

```c++
// ResourceString.cpp
#include "pch.h"

#include "ResourceString.h"
#if __has_include("ResourceString.g.cpp")
#include "ResourceString.g.cpp"
#endif

namespace winrt::LocalizationCpp::implementation
{
    // ...
}
```

After the implementation, update the [pch.h](./pch.h) to make `implementation::ResourceString` visible to `XamlTypeInfo.g.cpp`.
