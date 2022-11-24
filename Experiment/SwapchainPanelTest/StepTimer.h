//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <Windows.h>

/// @brief Helper class for animation and simulation timing.
/// @see https://github.com/microsoft/DirectX-Graphics-Samples
///      Samples/UWP/D3D12Bundles/src/StepTimer.h
class StepTimer final {
public:
  StepTimer() noexcept;

  UINT64 GetElapsedTicks() const noexcept;
  double GetElapsedSeconds() const noexcept;

  // Get total time since the start of the program.
  UINT64 GetTotalTicks() const noexcept;
  double GetTotalSeconds() const noexcept;

  // Get total number of updates since start of the program.
  UINT32 GetFrameCount() const noexcept;

  // Get the current framerate.
  UINT32 GetFramesPerSecond() const noexcept;

  // Set whether to use fixed or variable timestep mode.
  void SetFixedTimeStep(bool isFixedTimestep) noexcept;

  // Set how often to call Update when in fixed timestep mode.
  void SetTargetElapsedTicks(UINT64 targetElapsed) noexcept;
  void SetTargetElapsedSeconds(double targetElapsed) noexcept;

  static double TicksToSeconds(UINT64 ticks) noexcept;
  static UINT64 SecondsToTicks(double seconds) noexcept;

  // After an intentional timing discontinuity (for instance a blocking IO
  // operation) call this to avoid having the fixed timestep logic attempt a set
  // of catch-up Update calls.

  void ResetElapsedTime() noexcept;

  typedef void (*LPUPDATEFUNC)(void);

  // Update timer state, calling the specified Update function the appropriate
  // number of times.
  void Tick(LPUPDATEFUNC update = nullptr) noexcept;

private:
  // Source timing data uses QPC units.
  LARGE_INTEGER m_qpcFrequency;
  LARGE_INTEGER m_qpcLastTime;
  UINT64 m_qpcMaxDelta;

  // Derived timing data uses a canonical tick format.
  UINT64 m_elapsedTicks;
  UINT64 m_totalTicks;
  UINT64 m_leftOverTicks;

  // Members for tracking the framerate.
  UINT32 m_frameCount;
  UINT32 m_framesPerSecond;
  UINT32 m_framesThisSecond;
  UINT64 m_qpcSecondCounter;

  // Members for configuring fixed timestep mode.
  bool m_isFixedTimeStep;
  UINT64 m_targetElapsedTicks;
};
