#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <new>
#include <utility>
#include <cassert>

template <typename T, size_t BlockCount>
class FixedBlockAllocator {
public:
    FixedBlockAllocator() {
        // Pre-allocate contiguous memory for all blocks
        pool_ = static_cast<Node*>(::operator new[](BlockCount * sizeof(Node), std::align_val_t{alignof(Node)}));
        
        // Initialize free list linked through the unused memory blocks
        free_list_ = &pool_[0];
        for (size_t i = 0; i < BlockCount - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[BlockCount - 1].next = nullptr;
    }

    ~FixedBlockAllocator() {
        ::operator delete[](pool_, std::align_val_t{alignof(Node)});
    }

    // Disable copy constructor and assignment
    FixedBlockAllocator(const FixedBlockAllocator&) = delete;
    FixedBlockAllocator& operator=(const FixedBlockAllocator&) = delete;

    // $O(1)$ Deterministic Block Allocation
    template <typename... Args>
    T* allocate(Args&&... args) {
        if (!free_list_) [[unlikely]] {
            return nullptr; // Out of memory blocks
        }

        // Pop node from free list
        Node* node = free_list_;
        free_list_ = free_list_->next;

        // Construct object in-place using placement new
        T* ptr = reinterpret_cast<T*>(node);
        ::new (static_cast<void*>(ptr)) T{std::forward<Args>(args)...};
        return ptr;
    }

    // $O(1)$ Deterministic Block Deallocation
    void deallocate(T* ptr) {
        if (!ptr) [[likely]] return;

        // Call object destructor
        ptr->~T();

        // Push block back to free list
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
    }

private:
    // Union trick: active block holds object payload; free block holds pointer to next free block
    union Node {
        alignas(alignof(T)) char storage[sizeof(T)];
        Node* next;
    };

    Node* pool_{nullptr};
    Node* free_list_{nullptr};
};
