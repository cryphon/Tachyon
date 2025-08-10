#pragma once
#include <array>
#include <cstddef>

template<typename T, size_t capacity>
struct RingBuffer {
    std::array<T, capacity> buffer;
    size_t head;
    size_t tail;
};

// Initialize buffer
template<typename T, size_t capacity>
constexpr RingBuffer<T, capacity> make_ring_buffer() {
    return RingBuffer<T, capacity>{{}, 0, 0};
}

template<typename T, size_t capacity>
bool ring_buffer_push(RingBuffer<T, capacity>& rb, const T& value) {
    /* compute next index after the head, wrapping around if cap hit */
    size_t next = (rb.head + 1) % capacity;
    if(next == rb.tail) return false; /* full */

    /* store the new value at the curr index */
    rb.buffer[rb.head] = value;

    /* move forward */
    rb.head = next;
    return true;
}

template<typename T, size_t capacity>
bool ring_buffer_pop(RingBuffer<T, capacity>&rb, T& out) {
    if(rb.head == rb.tail) return false; /* empty */

    /* cp val to out */
    out = rb.buffer[rb.tail];
    
    /* move tail forward by 1, wrap around if needed */
    rb.tail = (rb.tail + 1) % capacity;
    return true;
}


template<typename T, size_t capacity>
constexpr bool ring_buffer_is_empty(const RingBuffer<T, capacity>&rb) {
    return rb.head == rb.tail;
}

template<typename T, size_t capacity>
constexpr bool ring_buffer_is_full(const RingBuffer<T, capacity>&rb) {
    return ((rb.head + 1) % capacity) == rb.tail;
}

template<typename T, size_t capacity>
constexpr size_t ring_buffer_capacity() { return capacity; }

template<typename T, size_t capacity>
constexpr size_t ring_buffer_size(const RingBuffer<T, capacity>&rb) {
    return (rb.head + capacity - rb.tail) % capacity;
}
