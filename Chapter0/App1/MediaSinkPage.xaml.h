#pragma once

#include "MediaSinkPage.g.h"

namespace winrt::App1::implementation {

/// @see https://github.com/KlearTouch/KlearTouch.MediaPlayer
struct MediaSinkPage : MediaSinkPageT<MediaSinkPage> {
    MediaSinkPage();
};

} // namespace winrt::App1::implementation

namespace winrt::App1::factory_implementation {
struct MediaSinkPage : MediaSinkPageT<MediaSinkPage, implementation::MediaSinkPage> {};
} // namespace winrt::App1::factory_implementation
