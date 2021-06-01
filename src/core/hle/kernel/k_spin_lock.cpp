// Copyright 2021 yuzu Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/kernel/k_spin_lock.h"

#if _MSC_VER
#include <intrin.h>
#if _M_AMD64
#define __x86_64__ 1
#endif
#if _M_ARM64
#define __aarch64__ 1
#endif
#else
#if __x86_64__
#include <xmmintrin.h>
#endif
#endif

namespace {

void ThreadPause() {
#if __x86_64__
    _mm_pause();
#elif __aarch64__ && _MSC_VER
    __yield();
#elif __aarch64__
    asm("yield");
#endif
}

} // namespace

namespace Kernel {

void KSpinLock::Lock() {
    while (lck.test_and_set(std::memory_order_acquire)) {
        ThreadPause();
    }
}

void KSpinLock::Unlock() {
    lck.clear(std::memory_order_release);
}

bool KSpinLock::TryLock() {
    if (lck.test_and_set(std::memory_order_acquire)) {
        return false;
    }
    return true;
}

} // namespace Kernel
