#include "pch.h"

#include "StepTimer.h"

// Integer format represents time using 10,000,000 ticks per second.
constexpr UINT64 TicksPerSecond = 10'000'000;

double StepTimer::TicksToSeconds(UINT64 ticks) noexcept {
  return static_cast<double>(ticks) / TicksPerSecond;
}
UINT64 StepTimer::SecondsToTicks(double seconds) noexcept {
  return static_cast<UINT64>(seconds * TicksPerSecond);
}

StepTimer::StepTimer() noexcept
    : m_elapsedTicks(0), m_totalTicks(0), m_leftOverTicks(0), m_frameCount(0),
      m_framesPerSecond(0), m_framesThisSecond(0), m_qpcSecondCounter(0),
      m_isFixedTimeStep(false), m_targetElapsedTicks(TicksPerSecond / 60) {
  QueryPerformanceFrequency(&m_qpcFrequency);
  QueryPerformanceCounter(&m_qpcLastTime);

  // Initialize max delta to 1/10 of a second.
  m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
}

UINT64 StepTimer::GetElapsedTicks() const noexcept {
  return m_elapsedTicks;
}
double StepTimer::GetElapsedSeconds() const noexcept {
  return TicksToSeconds(m_elapsedTicks);
}

UINT64 StepTimer::GetTotalTicks() const noexcept {
  return m_totalTicks;
}
double StepTimer::GetTotalSeconds() const noexcept {
  return TicksToSeconds(m_totalTicks);
}

UINT32 StepTimer::GetFrameCount() const noexcept {
  return m_frameCount;
}

UINT32 StepTimer::GetFramesPerSecond() const noexcept {
  return m_framesPerSecond;
}

void StepTimer::SetFixedTimeStep(bool isFixedTimestep) noexcept {
  m_isFixedTimeStep = isFixedTimestep;
}

void StepTimer::SetTargetElapsedTicks(UINT64 targetElapsed) noexcept {
  m_targetElapsedTicks = targetElapsed;
}
void StepTimer::SetTargetElapsedSeconds(double targetElapsed) noexcept {
  m_targetElapsedTicks = SecondsToTicks(targetElapsed);
}

// After an intentional timing discontinuity (for instance a blocking IO
// operation) call this to avoid having the fixed timestep logic attempt a set
// of catch-up Update calls.

void StepTimer::ResetElapsedTime() noexcept {
  QueryPerformanceCounter(&m_qpcLastTime);

  m_leftOverTicks = 0;
  m_framesPerSecond = 0;
  m_framesThisSecond = 0;
  m_qpcSecondCounter = 0;
}

void StepTimer::Tick(LPUPDATEFUNC update, void* context) noexcept {
  // Query the current time.
  LARGE_INTEGER currentTime;

  QueryPerformanceCounter(&currentTime);

  UINT64 timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

  m_qpcLastTime = currentTime;
  m_qpcSecondCounter += timeDelta;

  // Clamp excessively large time deltas (e.g. after paused in the debugger).
  if (timeDelta > m_qpcMaxDelta) {
    timeDelta = m_qpcMaxDelta;
  }

  // Convert QPC units into a canonical tick format. This cannot overflow due
  // to the previous clamp.
  timeDelta *= TicksPerSecond;
  timeDelta /= m_qpcFrequency.QuadPart;

  UINT32 lastFrameCount = m_frameCount;

  if (m_isFixedTimeStep) {
    // Fixed timestep update logic

    // If the app is running very close to the target elapsed time (within 1/4
    // of a millisecond) just clamp the clock to exactly match the target
    // value. This prevents tiny and irrelevant errors from accumulating over
    // time. Without this clamping, a game that requested a 60 fps fixed
    // update, running with vsync enabled on a 59.94 NTSC display, would
    // eventually accumulate enough tiny errors that it would drop a frame. It
    // is better to just round small deviations down to zero to leave things
    // running smoothly.

    if (abs(static_cast<int>(timeDelta - m_targetElapsedTicks)) <
        TicksPerSecond / 4000) {
      timeDelta = m_targetElapsedTicks;
    }

    m_leftOverTicks += timeDelta;

    while (m_leftOverTicks >= m_targetElapsedTicks) {
      m_elapsedTicks = m_targetElapsedTicks;
      m_totalTicks += m_targetElapsedTicks;
      m_leftOverTicks -= m_targetElapsedTicks;
      m_frameCount++;

      if (update) {
        update(context);
      }
    }
  } else {
    // Variable timestep update logic.
    m_elapsedTicks = timeDelta;
    m_totalTicks += timeDelta;
    m_leftOverTicks = 0;
    m_frameCount++;

    if (update) {
      update(context);
    }
  }

  // Track the current framerate.
  if (m_frameCount != lastFrameCount) {
    m_framesThisSecond++;
  }

  if (m_qpcSecondCounter >= static_cast<UINT64>(m_qpcFrequency.QuadPart)) {
    m_framesPerSecond = m_framesThisSecond;
    m_framesThisSecond = 0;
    m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
  }
}
