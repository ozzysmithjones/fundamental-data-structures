#include "fundamental.hpp"

#ifdef NDEBUG
heap_allocator global_heap_allocator{};
#else
static heap_allocator underlying_heap_allocator{};
// debug allocator that wraps the global heap allocator and tracks allocations and deallocations to help find memory leaks.
debug_allocator global_heap_allocator{ underlying_heap_allocator };
#endif

void* heap_allocator::malloc(size_t alignment, size_t bytes) {
    ASSERT(alignment <= alignof(std::max_align_t), return nullptr, "Requested alignment {} exceeds max alignment {}, cannot use default heap allocator (which uses malloc under the hood).", alignment, alignof(std::max_align_t));
    return std::malloc(bytes);
}

void* heap_allocator::realloc(void* ptr, size_t alignment, size_t old_bytes, size_t new_bytes) {
    ASSERT(alignment <= alignof(std::max_align_t), return nullptr, "Requested alignment {} exceeds max alignment {}, cannot use default heap allocator (which uses realloc under the hood).", alignment, alignof(std::max_align_t));
    return std::realloc(ptr, new_bytes);
}

void heap_allocator::free(void* ptr, size_t alignment, size_t bytes) {
    ASSERT(alignment <= alignof(std::max_align_t), return, "Requested alignment {} exceeds max alignment {}, cannot use default heap allocator (which uses free under the hood).", alignment, alignof(std::max_align_t));
    std::free(ptr);
}

debug_allocator::debug_allocator(allocator_base& wrapped_allocator) : wrapped_allocator(wrapped_allocator) {
}

debug_allocator::~debug_allocator() {
    if (current_allocation_count != 0 || current_allocated_bytes != 0) {
        BUG("Memory leak detected in debug_allocator: {} allocations still active, {} bytes still allocated.", current_allocation_count, current_allocated_bytes);
    }
}

void* debug_allocator::malloc(size_t alignment, size_t bytes) {
    void* ptr = wrapped_allocator.malloc(alignment, bytes);
    if (!ptr) {
        return nullptr;
    }

    ++total_allocation_count;
    ++current_allocation_count;
    if (current_allocation_count > peak_allocation_count) {
        peak_allocation_count = current_allocation_count;
    }

    total_allocated_bytes += bytes;
    current_allocated_bytes += bytes;
    if (current_allocated_bytes > peak_allocated_bytes) {
        peak_allocated_bytes = current_allocated_bytes;
    }
    return ptr;
}

void* debug_allocator::realloc(void* ptr, size_t alignment, size_t old_bytes, size_t new_bytes) {
    ptr = wrapped_allocator.realloc(ptr, alignment, old_bytes, new_bytes);
    if (!ptr) {
        return nullptr;
    }
    if (new_bytes > old_bytes) {
        if (old_bytes == 0) {
            ++total_allocation_count;
            ++current_allocation_count;
            if (current_allocation_count > peak_allocation_count) {
                peak_allocation_count = current_allocation_count;
            }
        }

        total_allocated_bytes += (new_bytes - old_bytes);
        current_allocated_bytes += (new_bytes - old_bytes);
        if (current_allocated_bytes > peak_allocated_bytes) {
            peak_allocated_bytes = current_allocated_bytes;
        }
    }
    else if (old_bytes > new_bytes) {
        total_freed_bytes += (old_bytes - new_bytes);
        current_allocated_bytes -= (old_bytes - new_bytes);
    }
    return ptr;
}

void debug_allocator::free(void* ptr, size_t alignment, size_t bytes) {
    if (!ptr) {
        return;
    }
    wrapped_allocator.free(ptr, alignment, bytes);
    ASSERT(current_allocation_count > 0, current_allocation_count = 1, "Unnecessary call to free() detected in debug_allocator, current allocation count is already zero. This means there is probably a double free or memory corruption.");
    ASSERT(current_allocated_bytes >= bytes, current_allocated_bytes = bytes, "Trying to free() more bytes than currently allocated");
    --current_allocation_count;
    total_freed_bytes += bytes;
    current_allocated_bytes -= bytes;
}
