#pragma once

#include <atomic>
#include <cstddef>
#include <new>
#include <vector>
#include <optional>
#include <utility>

template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2 for fast modulo masking.");

public:
    SPSCQueue() : buffer_(Capacity) {}

    // Producer thread pushes items
    template <typename... Args>
    bool emplace(Args&&... args) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        
        // Avoid atomic read on head_ if cached position allows progress
        if (current_tail - head_cached_ >= Capacity) {
            head_cached_ = head_.load(std::memory_order_acquire);
            if (current_tail - head_cached_ >= Capacity) {
                return false; // Queue is full
            }
        }

        buffer_[current_tail & MASK] = T(std::forward<Args>(args)...);
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    // Consumer thread pops items
    std::optional<T> pop() {
        const size_t current_head = head_.load(std::memory_order_relaxed);

        // Avoid atomic read on tail_ if cached position allows progress
        if (current_head == tail_cached_) {
            tail_cached_ = tail_.load(std::memory_order_acquire);
            if (current_head == tail_cached_) {
                return std::nullopt; // Queue is empty
            }
        }

        T item = std::move(buffer_[current_head & MASK]);
        head_.store(current_head + 1, std::memory_order_release);
        return item;
    }

private:
    static constexpr size_t MASK = Capacity - 1;
    std::vector<T> buffer_;

    // Align tail and head on separate 64-byte cache lines to eliminate False Sharing
    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> tail_{0};
    size_t head_cached_{0};

    alignas(std::hardware_destructive_interference_size) std::atomic<size_t> head_{0};
    size_t tail_cached_{0};
};
